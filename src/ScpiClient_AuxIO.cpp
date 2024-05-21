/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include <bci/abs/ScpiClient.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <array>
#include <span>
#include <string_view>

#include "ScpiUtil.h"
#include "Util.h"

namespace bci::abs {

static constexpr unsigned int kDigitalOutputsMask =
    ((1U << kDigitalOutputCount) - 1);

static constexpr float kMaxAnalogOutVoltage = 10.0f;

using util::Err;
using ec = ErrorCode;

ErrorCode ScpiClient::SetAnalogOutput(unsigned int channel,
                                      float voltage) const {
  if (channel >= kAnalogOutputCount) {
    return ec::kChannelIndexOutOfRange;
  }

  voltage = std::clamp(voltage, -kMaxAnalogOutVoltage, kMaxAnalogOutVoltage);

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:AUX:OUT{} {:.3f}\r\n",
                   channel + 1, voltage);

  return Send(buf);
}

ErrorCode ScpiClient::SetAllAnalogOutputs(float voltage) const {
  voltage = std::clamp(voltage, -kMaxAnalogOutVoltage, kMaxAnalogOutVoltage);

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:AUX:OUT {:.3f},(@1:{})\r\n",
                   voltage, kAnalogOutputCount);

  return Send(buf);
}

ErrorCode ScpiClient::SetAllAnalogOutputs(const float* voltages,
                                          std::size_t count) const {
  if ((count > 0 && !voltages) || count > kAnalogOutputCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  std::string buf;
  // try to do only one allocation
  buf.reserve(count * 21 + 2);
  for (std::size_t i = 0; i < count; ++i) {
    buf += fmt::format(
        ":SOUR:AUX:OUT{} {:.3f};", i + 1,
        std::clamp(voltages[i], -kMaxAnalogOutVoltage, kMaxAnalogOutVoltage));
  }
  buf += "\r\n";

  return Send(buf);
}

ErrorCode ScpiClient::SetAllAnalogOutputs(
    std::span<const float> voltages) const {
  return SetAllAnalogOutputs(voltages.data(), voltages.size());
}

ErrorCode ScpiClient::SetAllAnalogOutputs(
    const std::array<float, kAnalogOutputCount>& voltages) const {
  return SetAllAnalogOutputs(voltages.data(), voltages.size());
}

Result<float> ScpiClient::GetAnalogOutput(unsigned int channel) const {
  if (channel >= kAnalogOutputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:AUX:OUT{}?\r\n", channel + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
}

Result<std::array<float, kAnalogOutputCount>> ScpiClient::GetAllAnalogOutputs()
    const {
  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:AUX:OUT? (@1:{})\r\n",
                   kAnalogOutputCount);
  return SendAndRecv(buf).and_then(
      scpi::ParseRespFloatArray<kAnalogOutputCount>);
}

ErrorCode ScpiClient::GetAllAnalogOutputs(float* voltages,
                                          std::size_t count) const {
  if ((!voltages && count > 0) || count > kAnalogOutputCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:AUX:OUT? (@1:{})\r\n", count);

  auto resp = SendAndRecv(buf);
  if (!resp) {
    return resp.error();
  }

  return scpi::SplitRespFloats(*resp, std::span{voltages, count});
}

ErrorCode ScpiClient::GetAllAnalogOutputs(
    std::array<float, kAnalogOutputCount>& voltages) const {
  return GetAllAnalogOutputs(voltages.data(), voltages.size());
}

ErrorCode ScpiClient::GetAllAnalogOutputs(std::span<float> voltages) const {
  return GetAllAnalogOutputs(voltages.data(), voltages.size());
}

ErrorCode ScpiClient::SetDigitalOutput(unsigned int channel, bool level) const {
  if (channel >= kDigitalOutputCount) {
    return ec::kChannelIndexOutOfRange;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:DAUX:OUT{} {:d}\r\n",
                   channel + 1, level);

  return Send(buf);
}

ErrorCode ScpiClient::SetAllDigitalOutputs(bool level) const {
  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:DAUX:OUT {:d},(@1:{})\r\n",
                   level, kDigitalOutputCount);
  return Send(buf);
}

ErrorCode ScpiClient::SetAllDigitalOutputsMasked(unsigned int channels,
                                                 bool level) const {
  channels &= kDigitalOutputsMask;
  if (channels != 0) {
    // avoid allocation by pre-computing the indices so fmt can format them at
    // once
    std::array<unsigned int, kDigitalOutputCount> which_chans{};
    std::size_t count = 0;
    for (unsigned int i = 0; i < kDigitalOutputCount && channels != 0;
         ++i, channels >>= 1) {
      if (channels & 1) {
        which_chans[count++] = i + 1;
      }
    }
    std::span chan_list{which_chans.data(), count};
    char buf[64]{};
    fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:DAUX:OUT {:d},(@{})\r\n",
                     level, fmt::join(chan_list, ","));
    return Send(buf);
  }
  return ec::kSuccess;
}

ErrorCode ScpiClient::SetAllDigitalOutputs(
    const std::array<bool, kDigitalOutputCount>& levels) const {
  return SetAllDigitalOutputs(std::span{levels});
}

ErrorCode ScpiClient::SetAllDigitalOutputs(std::span<const bool> levels) const {
  const auto count =
      std::min(levels.size(), static_cast<std::size_t>(kDigitalOutputCount));
  if (count == 0) {
    return ec::kSuccess;
  }

  unsigned int mask_high{};
  unsigned int mask_low{};

  for (std::size_t i = 0; i < count; ++i) {
    if (levels[i]) {
      mask_high |= (1U << i);
    } else {
      mask_low |= (1U << i);
    }
  }

  if (mask_high) {
    auto e = SetAllDigitalOutputsMasked(mask_high, true);
    if (e != ec::kSuccess) {
      return e;
    }
  }

  if (mask_low) {
    auto e = SetAllDigitalOutputsMasked(mask_low, false);
    if (e != ec::kSuccess) {
      return e;
    }
  }

  return ec::kSuccess;
}

Result<bool> ScpiClient::GetDigitalOutput(unsigned int channel) const {
  if (channel >= kDigitalOutputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:DAUX:OUT{}?\r\n", channel + 1);

  return SendAndRecv(buf).and_then(scpi::ParseBoolResponse);
}

Result<std::array<bool, kDigitalOutputCount>> ScpiClient::GetAllDigitalOutputs()
    const {
  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:DAUX:OUT? (@1:{})\r\n",
                   kDigitalOutputCount);
  return SendAndRecv(buf).and_then(
      scpi::ParseRespBoolArray<kDigitalOutputCount>);
}

Result<unsigned int> ScpiClient::GetAllDigitalOutputsMasked() const {
  auto resp = GetAllDigitalOutputs();
  if (!resp) {
    return Err(resp.error());
  }

  unsigned int mask{};
  for (std::size_t i = 0; i < resp->size(); ++i) {
    if (resp->at(i)) {
      mask |= (1U << i);
    }
  }

  return mask;
}

Result<float> ScpiClient::MeasureAnalogInput(unsigned int channel) const {
  if (channel >= kAnalogInputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:AUX:IN{}?\r\n", channel + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
}

Result<std::array<float, kAnalogInputCount>>
ScpiClient::MeasureAllAnalogInputs() const {
  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:AUX:IN? (@1:{})\r\n",
                   kAnalogInputCount);
  return SendAndRecv(buf).and_then(
      scpi::ParseRespFloatArray<kAnalogInputCount>);
}

ErrorCode ScpiClient::MeasureAllAnalogInputs(float* voltages,
                                             std::size_t count) const {
  if ((!voltages && count > 0) || count > kAnalogInputCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:AUX:IN? (@1:{})\r\n", count);

  auto resp = SendAndRecv(buf);
  if (!resp) {
    return resp.error();
  }

  return scpi::SplitRespFloats(*resp, std::span{voltages, count});
}

ErrorCode ScpiClient::MeasureAllAnalogInputs(
    std::array<float, kAnalogInputCount>& voltages) const {
  return MeasureAllAnalogInputs(voltages.data(), voltages.size());
}

ErrorCode ScpiClient::MeasureAllAnalogInputs(std::span<float> voltages) const {
  return MeasureAllAnalogInputs(voltages.data(), voltages.size());
}

Result<bool> ScpiClient::MeasureDigitalInput(unsigned int channel) const {
  if (channel >= kDigitalInputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:DAUX:IN{}?\r\n", channel + 1);

  return SendAndRecv(buf).and_then(scpi::ParseBoolResponse);
}

Result<std::array<bool, kDigitalInputCount>>
ScpiClient::MeasureAllDigitalInputs() const {
  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:DAUX:IN? (@1:{})\r\n",
                   kDigitalInputCount);
  return SendAndRecv(buf).and_then(
      scpi::ParseRespBoolArray<kDigitalInputCount>);
}

Result<unsigned int> ScpiClient::MeasureAllDigitalInputsMasked() const {
  auto resp = MeasureAllDigitalInputs();
  if (!resp) {
    return Err(resp.error());
  }

  unsigned int mask{};
  for (std::size_t i = 0; i < resp->size(); ++i) {
    if (resp->at(i)) {
      mask |= (1U << i);
    }
  }

  return mask;
}

}  // namespace bci::abs
