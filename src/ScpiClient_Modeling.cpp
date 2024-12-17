/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include <bci/abs/ScpiClient.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <array>
#include <iterator>
#include <span>
#include <string_view>

#include "ScpiUtil.h"
#include "Util.h"

namespace bci::abs {

using util::Err;
using ec = ErrorCode;

Result<std::uint8_t> ScpiClient::GetModelStatus() const {
  return SendAndRecv("MOD:STAT?\r\n")
      .and_then(scpi::ParseIntResponse<std::uint8_t>);
}

ErrorCode ScpiClient::LoadModel() const { return Send("MOD:LOAD\r\n"); }

ErrorCode ScpiClient::StartModel() const { return Send("MOD:START\r\n"); }

ErrorCode ScpiClient::StopModel() const { return Send("MOD:STOP\r\n"); }

ErrorCode ScpiClient::UnloadModel() const { return Send("MOD:UNLOAD\r\n"); }

Result<ModelInfo> ScpiClient::GetModelInfo() const {
  auto res =
      SendAndRecv("MOD:INFO?\r\n").and_then(scpi::ParseStringArrayResponse<2>);
  if (!res) {
    return Err(res.error());
  }
  return ModelInfo{std::move(res->at(0)), std::move(res->at(1))};
}

Result<std::string> ScpiClient::GetModelId() const {
  return SendAndRecv("MOD:ID?\r\n").and_then(scpi::ParseStringResponse);
}

ErrorCode ScpiClient::SetGlobalModelInput(unsigned int index,
                                          float value) const {
  if (index >= kGlobalModelInputCount) {
    return ec::kChannelIndexOutOfRange;
  }

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MOD:GLOB{} {}\r\n", index + 1, value);
  return Send(buf);
}

ErrorCode ScpiClient::SetAllGlobalModelInputs(float value) const {
  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MOD:GLOB {},(@1:{})\r\n", value,
                   kGlobalModelInputCount);
  return Send(buf);
}

ErrorCode ScpiClient::SetAllGlobalModelInputs(const float* values,
                                              std::size_t count) const {
  if ((count > 0 && !values) || count > kGlobalModelInputCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  std::string buf;
  // try to do one allocation
  buf.reserve(count * 30);
  for (std::size_t i = 0; i < count; ++i) {
    fmt::format_to(std::back_inserter(buf), ":MOD:GLOB{} {};", i + 1,
                   values[i]);
  }
  buf += "\r\n";

  return Send(buf);
}

ErrorCode ScpiClient::SetAllGlobalModelInputs(
    std::span<const float> values) const {
  return SetAllGlobalModelInputs(values.data(), values.size());
}

ErrorCode ScpiClient::SetAllGlobalModelInputs(
    const std::array<float, kGlobalModelInputCount>& values) const {
  return SetAllGlobalModelInputs(values.data(), values.size());
}

Result<float> ScpiClient::GetGlobalModelInput(unsigned int index) const {
  if (index >= kGlobalModelInputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MOD:GLOB{}?\r\n", index + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
}

Result<std::array<float, kGlobalModelInputCount>>
ScpiClient::GetAllGlobalModelInputs() const {
  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MOD:GLOB? (@1:{})\r\n",
                   kGlobalModelInputCount);
  return SendAndRecv(buf).and_then(
      scpi::ParseRespFloatArray<kGlobalModelInputCount>);
}

ErrorCode ScpiClient::GetAllGlobalModelInputs(float* values,
                                              std::size_t count) const {
  if ((!values && count > 0) || count > kGlobalModelInputCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MOD:GLOB? (@1:{})\r\n", count);

  auto resp = SendAndRecv(buf);
  if (!resp) {
    return resp.error();
  }

  return scpi::SplitRespFloats(*resp, std::span{values, count});
}

ErrorCode ScpiClient::GetAllGlobalModelInputs(
    std::array<float, kGlobalModelInputCount>& values) const {
  return GetAllGlobalModelInputs(values.data(), values.size());
}

ErrorCode ScpiClient::GetAllGlobalModelInputs(std::span<float> values) const {
  return GetAllGlobalModelInputs(values.data(), values.size());
}

ErrorCode ScpiClient::SetLocalModelInput(unsigned int index,
                                         float value) const {
  if (index >= kLocalModelInputCount) {
    return ec::kChannelIndexOutOfRange;
  }

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MOD:LOC{} {}\r\n", index + 1, value);
  return Send(buf);
}

ErrorCode ScpiClient::SetAllLocalModelInputs(float value) const {
  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MOD:LOC {},(@1:{})\r\n", value,
                   kLocalModelInputCount);
  return Send(buf);
}

ErrorCode ScpiClient::SetAllLocalModelInputs(const float* values,
                                             std::size_t count) const {
  if ((count > 0 && !values) || count > kLocalModelInputCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  std::string buf;
  // try to do one allocation
  buf.reserve(count * 30);
  for (std::size_t i = 0; i < count; ++i) {
    fmt::format_to(std::back_inserter(buf), ":MOD:LOC{} {};", i + 1, values[i]);
  }
  buf += "\r\n";

  return Send(buf);
}

ErrorCode ScpiClient::SetAllLocalModelInputs(
    std::span<const float> values) const {
  return SetAllLocalModelInputs(values.data(), values.size());
}

ErrorCode ScpiClient::SetAllLocalModelInputs(
    const std::array<float, kLocalModelInputCount>& values) const {
  return SetAllLocalModelInputs(values.data(), values.size());
}

Result<float> ScpiClient::GetLocalModelInput(unsigned int index) const {
  if (index >= kLocalModelInputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MOD:LOC{}?\r\n", index + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
}

Result<std::array<float, kLocalModelInputCount>>
ScpiClient::GetAllLocalModelInputs() const {
  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MOD:LOC? (@1:{})\r\n",
                   kLocalModelInputCount);
  return SendAndRecv(buf).and_then(
      scpi::ParseRespFloatArray<kLocalModelInputCount>);
}

ErrorCode ScpiClient::GetAllLocalModelInputs(float* values,
                                             std::size_t count) const {
  if ((!values && count > 0) || count > kLocalModelInputCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MOD:LOC? (@1:{})\r\n", count);

  auto resp = SendAndRecv(buf);
  if (!resp) {
    return resp.error();
  }

  return scpi::SplitRespFloats(*resp, std::span{values, count});
}

ErrorCode ScpiClient::GetAllLocalModelInputs(
    std::array<float, kLocalModelInputCount>& values) const {
  return GetAllLocalModelInputs(values.data(), values.size());
}

ErrorCode ScpiClient::GetAllLocalModelInputs(std::span<float> values) const {
  return GetAllLocalModelInputs(values.data(), values.size());
}

Result<float> ScpiClient::GetModelOutput(unsigned int index) const {
  if (index >= kModelOutputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MOD:OUT{}?\r\n", index + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
}

Result<std::array<float, kModelOutputCount>> ScpiClient::GetAllModelOutputs()
    const {
  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MOD:OUT? (@1:{})\r\n",
                   kModelOutputCount);

  return SendAndRecv(buf).and_then(
      scpi::ParseRespFloatArray<kModelOutputCount>);
}

ErrorCode ScpiClient::GetAllModelOutputs(float* outputs,
                                         std::size_t count) const {
  if ((!outputs && count > 0) || count > kModelOutputCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MOD:OUT? (@1:{})\r\n",
                   kModelOutputCount);

  auto resp = SendAndRecv(buf);
  if (!resp) {
    return resp.error();
  }

  ec e = scpi::SplitRespFloats(*resp, std::span{outputs, count});
  if (e != ec::kSuccess) {
    return e;
  }

  return ec::kSuccess;
}

ErrorCode ScpiClient::GetAllModelOutputs(
    std::array<float, kModelOutputCount>& outputs) const {
  return GetAllModelOutputs(outputs.data(), outputs.size());
}

ErrorCode ScpiClient::GetAllModelOutputs(std::span<float> outputs) const {
  return GetAllModelOutputs(outputs.data(), outputs.size());
}

}  // namespace bci::abs
