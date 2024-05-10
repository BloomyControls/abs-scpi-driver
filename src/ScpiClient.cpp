#include <bci/abs/ScpiClient.h>
#include <fmt/core.h>

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "Util.h"
#include "ScpiUtil.h"

namespace bci::abs {

static const unsigned int kWriteTimeoutMs = 250;
static const unsigned int kReadTimeoutMs = 500;

using util::Err;
using ec = ErrorCode;

ScpiClient::ScpiClient(std::shared_ptr<drivers::CommDriver> driver)
    : driver_(std::move(driver)) {}

ScpiClient::~ScpiClient() {}

ErrorCode ScpiClient::EnableCell(unsigned int cell, bool en) const {
  if (cell >= kCellCount) {
    return ec::kChannelIndexOutOfRange;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP{} {:d}\r\n", cell + 1, en);

  return driver_->Write(buf, kWriteTimeoutMs);

  return ec::kSuccess;
}

Result<float> ScpiClient::MeasureCellVoltage(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS{}:VOLT?\r\n", cell + 1);

  return WriteAndRead(buf).and_then(scpi::ParseFloatResponse);
}

Result<std::string> ScpiClient::WriteAndRead(std::string_view buf) const {
  auto res = driver_->Write(buf, kWriteTimeoutMs);
  if (res != ErrorCode::kSuccess) {
    return Err(res);
  }
  return driver_->ReadLine(kReadTimeoutMs);
}

}  // namespace bci::abs
