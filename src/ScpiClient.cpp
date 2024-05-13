#include <bci/abs/ScpiClient.h>
#include <fmt/core.h>

#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include "ScpiUtil.h"
#include "Util.h"

namespace bci::abs {

static constexpr unsigned int kWriteTimeoutMs = 250;
static constexpr unsigned int kReadTimeoutMs = 500;

using util::Err;
using ec = ErrorCode;

ScpiClient::ScpiClient(std::shared_ptr<drivers::CommDriver> driver)
    : driver_(std::move(driver)) {}

ScpiClient::~ScpiClient() {}

ErrorCode ScpiClient::Send(std::string_view buf) const {
  if (!driver_) {
    return ec::kInvalidDriverHandle;
  }

  return driver_->Write(buf, kWriteTimeoutMs);
}

Result<std::string> ScpiClient::SendAndRecv(std::string_view buf) const {
  if (!driver_) {
    return Err(ec::kInvalidDriverHandle);
  }

  auto res = driver_->Write(buf, kWriteTimeoutMs);
  if (res != ErrorCode::kSuccess) {
    return Err(res);
  }
  return driver_->ReadLine(kReadTimeoutMs);
}

}  // namespace bci::abs
