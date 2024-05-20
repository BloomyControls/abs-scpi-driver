/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include <bci/abs/ScpiClient.h>
#include <fmt/core.h>

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

#include "ScpiUtil.h"
#include "Util.h"

namespace bci::abs {

using util::Err;
using ec = ErrorCode;

Result<DeviceInfo> ScpiClient::GetDeviceInfo() const {
  auto res = SendAndRecv("*IDN?\r\n");
  if (!res) {
    return Err(res.error());
  }

  std::array<std::string_view, 4> idn;
  if (scpi::SplitRespMnemonics(*res, idn) != ErrorCode::kSuccess) {
    return Err(ErrorCode::kInvalidResponse);
  }

  return DeviceInfo{std::string(idn[1]), std::string(idn[2]),
                    std::string(idn[3])};
}

Result<std::uint8_t> ScpiClient::GetDeviceId() const {
  return SendAndRecv("CONF:COMM:SER:ID?\r\n")
      .and_then(scpi::ParseIntResponse<std::uint8_t>);
}

Result<EthernetConfig> ScpiClient::GetIPAddress() const {
  auto resp = SendAndRecv("CONF:COMM:SOCK:ADDR?\r\n")
                  .and_then(scpi::ParseStringArrayResponse<2>);
  if (!resp) {
    return Err(resp.error());
  }
  return EthernetConfig{std::move(resp->at(0)), std::move(resp->at(1))};
}

ErrorCode ScpiClient::SetIPAddress(std::string_view ip,
                                   std::string_view netmask) const {
  if (ip.size() > 15 || netmask.size() > 15) {
    return ec::kInvalidIPAddress;
  }

  if (ip.find('"') != ip.npos || netmask.find('"') != ip.npos) {
    return ec::kInvalidIPAddress;
  }

  char buf[128]{};
  fmt::format_to_n(buf, sizeof(buf) - 1,
                   "CONF:COMM:SOCK:ADDR \"{}\",\"{}\"\r\n", ip, netmask);

  return Send(buf);
}

Result<std::string> ScpiClient::GetCalibrationDate() const {
  return SendAndRecv("CAL:DATE?\r\n").and_then(scpi::ParseStringResponse);
}

Result<int> ScpiClient::GetErrorCount() const {
  return SendAndRecv("SYST:ERR:COUN?\r\n")
      .and_then(scpi::ParseIntResponse<int>);
}

Result<ScpiError> ScpiClient::GetNextError() const {
  return SendAndRecv("SYST:ERR?\r\n").and_then(scpi::ParseScpiError);
}

ErrorCode ScpiClient::ClearErrors() const { return Send("*CLS\r\n"); }

Result<std::uint32_t> ScpiClient::GetAlarms() const {
  return SendAndRecv("SYST:ALARM?\r\n")
      .and_then(scpi::ParseIntResponse<std::uint32_t>);
}

ErrorCode ScpiClient::AssertSoftwareInterlock() const {
  return Send("SYST:ALARM:RAISE\r\n");
}

ErrorCode ScpiClient::ClearRecoverableAlarms() const {
  return Send("SYST:ALARM:CLEAR\r\n");
}

ErrorCode ScpiClient::Reboot() const { return Send("*RST\r\n"); }

}  // namespace bci::abs
