/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include <bci/abs/ScpiClient.h>
#include <fmt/core.h>

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "Util.h"

namespace bci::abs {

static constexpr unsigned int kWriteTimeoutMs = 10;

using util::Err;
using ec = ErrorCode;

unsigned int ScpiClient::Version() noexcept {
  return ABSSCPI_VERSION;
}

ScpiClient::ScpiClient() noexcept : ScpiClient(nullptr) {}

ScpiClient::ScpiClient(std::shared_ptr<drivers::CommDriver> driver) noexcept
    : driver_{std::move(driver)}, read_timeout_ms_{150U} {}

ScpiClient::ScpiClient(ScpiClient&& other) noexcept
    : driver_{std::move(other.driver_)},
      read_timeout_ms_{std::move(other.read_timeout_ms_)} {}

ScpiClient& ScpiClient::operator=(ScpiClient&& rhs) noexcept {
  driver_ = std::move(rhs.driver_);
  read_timeout_ms_ = std::move(rhs.read_timeout_ms_);
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

unsigned int ScpiClient::SetReadTimeout(unsigned int timeout_ms) noexcept {
  return std::exchange(read_timeout_ms_, timeout_ms);
}

ErrorCode ScpiClient::SetTargetDeviceID(unsigned int id) {
  if (driver_) {
    driver_->SetDeviceID(id);
    return ec::kSuccess;
  }
  return ec::kInvalidDriverHandle;
}

Result<unsigned int> ScpiClient::GetTargetDeviceID() const {
  if (!driver_) {
    return Err(ec::kInvalidDriverHandle);
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
  return driver_->ReadLine(read_timeout_ms_);
}

}  // namespace bci::abs
