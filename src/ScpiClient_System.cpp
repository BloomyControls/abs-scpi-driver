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

Result<int> ScpiClient::GetErrorCount() const {
  return SendAndRecv("SYST:ERR:COUN?\r\n")
      .and_then(scpi::ParseIntResponse<int>);
}

Result<ScpiError> ScpiClient::GetNextError() const {
  return SendAndRecv("SYST:ERR?\r\n").and_then(scpi::ParseScpiError);
}

ErrorCode ScpiClient::ClearErrors() const { return Send("*CLS\r\n"); }

ErrorCode ScpiClient::Reboot() const { return Send("*RST\r\n"); }

}  // namespace bci::abs
