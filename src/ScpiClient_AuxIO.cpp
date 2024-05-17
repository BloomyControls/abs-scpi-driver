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

ErrorCode ScpiClient::SetAllAnalogOutput(float voltage) const {
  voltage = std::clamp(voltage, -kMaxAnalogOutVoltage, kMaxAnalogOutVoltage);

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:AUX:OUT {:.3f},(@1:{})\r\n",
                   voltage, kAnalogOutputCount);

  return Send(buf);
}

ErrorCode ScpiClient::SetAllAnalogOutput(const float* voltages,
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

ErrorCode ScpiClient::SetAllAnalogOutput(
    std::span<const float> voltages) const {
  return SetAllAnalogOutput(voltages.data(), voltages.size());
}

ErrorCode ScpiClient::SetAllAnalogOutput(
    const std::array<float, kAnalogOutputCount>& voltages) const {
  return SetAllAnalogOutput(voltages.data(), voltages.size());
}

Result<float> ScpiClient::GetAnalogOutput(unsigned int channel) const {
  if (channel >= kAnalogOutputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:AUX:OUT{}?\r\n", channel + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
}

Result<std::array<float, kAnalogOutputCount>> ScpiClient::GetAllAnalogOutput()
    const {
  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:AUX:OUT? (@1:{})\r\n",
                   kAnalogOutputCount);
  return SendAndRecv(buf).and_then(
      scpi::ParseRespFloatArray<kAnalogOutputCount>);
}

ErrorCode ScpiClient::GetAllAnalogOutput(float* voltages,
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

ErrorCode ScpiClient::GetAllAnalogOutput(
    std::array<float, kAnalogOutputCount>& voltages) const {
  return GetAllAnalogOutput(voltages.data(), voltages.size());
}

ErrorCode ScpiClient::GetAllAnalogOutput(std::span<float> voltages) const {
  return GetAllAnalogOutput(voltages.data(), voltages.size());
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

ErrorCode ScpiClient::SetAllDigitalOutput(bool level) const {
  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:DAUX:OUT {:d},(@1:{})\r\n",
                   level, kDigitalOutputCount);
  return Send(buf);
}

ErrorCode ScpiClient::SetAllDigitalOutputMasked(unsigned int channels,
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

ErrorCode ScpiClient::SetAllDigitalOutput(
    const std::array<bool, kDigitalOutputCount>& levels) const {
  return SetAllDigitalOutput(std::span{levels});
}

ErrorCode ScpiClient::SetAllDigitalOutput(std::span<const bool> levels) const {
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
    auto e = SetAllDigitalOutputMasked(mask_high, true);
    if (e != ec::kSuccess) {
      return e;
    }
  }

  if (mask_low) {
    auto e = SetAllDigitalOutputMasked(mask_low, false);
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

Result<std::array<bool, kDigitalOutputCount>> ScpiClient::GetAllDigitalOutput()
    const {
  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:DAUX:OUT? (@1:{})\r\n",
                   kDigitalOutputCount);
  return SendAndRecv(buf).and_then(
      scpi::ParseRespBoolArray<kDigitalOutputCount>);
}

Result<unsigned int> ScpiClient::GetAllDigitalOutputMasked() const {
  auto resp = GetAllDigitalOutput();
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

Result<std::array<float, kAnalogInputCount>> ScpiClient::MeasureAllAnalogInput()
    const {
  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:AUX:IN? (@1:{})\r\n",
                   kAnalogInputCount);
  return SendAndRecv(buf).and_then(
      scpi::ParseRespFloatArray<kAnalogInputCount>);
}

ErrorCode ScpiClient::MeasureAllAnalogInput(float* voltages,
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

ErrorCode ScpiClient::MeasureAllAnalogInput(
    std::array<float, kAnalogInputCount>& voltages) const {
  return MeasureAllAnalogInput(voltages.data(), voltages.size());
}

ErrorCode ScpiClient::MeasureAllAnalogInput(std::span<float> voltages) const {
  return MeasureAllAnalogInput(voltages.data(), voltages.size());
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
ScpiClient::MeasureAllDigitalInput() const {
  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:DAUX:IN? (@1:{})\r\n",
                   kDigitalInputCount);
  return SendAndRecv(buf).and_then(
      scpi::ParseRespBoolArray<kDigitalInputCount>);
}

Result<unsigned int> ScpiClient::MeasureAllDigitalInputMasked() const {
  auto resp = MeasureAllDigitalInput();
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
