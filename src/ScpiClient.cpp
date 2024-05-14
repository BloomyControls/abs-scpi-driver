#include <bci/abs/ScpiClient.h>
#include <fmt/core.h>

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "Util.h"

namespace bci::abs {

static constexpr unsigned int kWriteTimeoutMs = 250;
static constexpr unsigned int kReadTimeoutMs = 500;

using util::Err;
using ec = ErrorCode;

ScpiClient::ScpiClient(std::shared_ptr<drivers::CommDriver> driver) noexcept
    : driver_{std::move(driver)} {}

ScpiClient::ScpiClient(ScpiClient&& other) noexcept
    : driver_{std::move(other.driver_)} {}

ScpiClient& ScpiClient::operator=(ScpiClient&& rhs) noexcept {
  driver_ = std::move(rhs.driver_);
  return *this;
}

std::shared_ptr<drivers::CommDriver> ScpiClient::GetDriver() noexcept {
  return driver_;
}

std::shared_ptr<const drivers::CommDriver> ScpiClient::GetDriver()
    const noexcept {
  return driver_;
}

void ScpiClient::SetDriver(
    std::shared_ptr<drivers::CommDriver> driver) noexcept {
  driver_ = std::move(driver);
}

void ScpiClient::SetTargetDeviceID(unsigned int id) {
  if (driver_) {
    driver_->SetDeviceID(id);
  }
}

unsigned int ScpiClient::GetTargetDeviceID() const {
  if (!driver_) {
    return 0;
  }
  return driver_->GetDeviceID();
}

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

  if (driver_->IsSendOnly()) {
    return Err(ec::kReceiveNotAllowed);
  }

  auto res = driver_->Write(buf, kWriteTimeoutMs);
  if (res != ErrorCode::kSuccess) {
    return Err(res);
  }
  return driver_->ReadLine(kReadTimeoutMs);
}

}  // namespace bci::abs
