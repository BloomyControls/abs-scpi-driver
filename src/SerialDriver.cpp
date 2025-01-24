/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include <bci/abs/SerialDriver.h>
#include <fmt/core.h>

#include <algorithm>
#include <array>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <chrono>
#include <memory>
#include <string>
#include <string_view>

#include "Util.h"

namespace bci::abs::drivers {

using util::Err;

struct SerialDriver::Impl {
  Impl();

  ~Impl();

  ErrorCode Open(const std::string& port);

  void Close() noexcept;

  ErrorCode Write(std::string_view data, unsigned int timeout_ms);

  Result<std::string> ReadLine(unsigned int timeout_ms);

  void SetDeviceID(unsigned int id);

  unsigned int GetDeviceID() const;

  bool IsBroadcast() const;

 private:
  boost::asio::io_service io_service_;
  boost::asio::serial_port port_;
  boost::asio::streambuf input_buffer_;
  unsigned int dev_id_;

  bool Run(unsigned int timeout_ms);
};

SerialDriver::SerialDriver() : impl_(std::make_shared<Impl>()) {}

SerialDriver::~SerialDriver() { Close(); }

ErrorCode SerialDriver::Open(const std::string& port) {
  return impl_->Open(port);
}

void SerialDriver::Close() noexcept { impl_->Close(); }

ErrorCode SerialDriver::Write(std::string_view data,
                              unsigned int timeout_ms) const {
  return impl_->Write(data, timeout_ms);
}

Result<std::string> SerialDriver::ReadLine(unsigned int timeout_ms) const {
  return impl_->ReadLine(timeout_ms);
}

void SerialDriver::SetDeviceID(unsigned int id) { impl_->SetDeviceID(id); }

unsigned int SerialDriver::GetDeviceID() const { return impl_->GetDeviceID(); }

bool SerialDriver::IsSendOnly() const { return impl_->IsBroadcast(); }

SerialDriver::Impl::Impl()
    : io_service_(),
      port_(io_service_),
      input_buffer_(),
      dev_id_{} { }

SerialDriver::Impl::~Impl() { Close(); }

ErrorCode SerialDriver::Impl::Open(const std::string& port) {
  boost::system::error_code ec{};

  if (port_.is_open()) {
    return ErrorCode::kAlreadyConnected;
  }

  port_.open(port, ec);
  if (ec) {
    return ErrorCode::kOpeningSerialPortFailed;
  }

  using boost::asio::serial_port;

  port_.set_option(serial_port::baud_rate(115200), ec);
  if (ec) {
    return ErrorCode::kFailedToConfigurePort;
  }

  port_.set_option(serial_port::character_size(8), ec);
  if (ec) {
    return ErrorCode::kFailedToConfigurePort;
  }

  port_.set_option(serial_port::parity(serial_port::parity::type::none), ec);
  if (ec) {
    return ErrorCode::kFailedToConfigurePort;
  }

  port_.set_option(serial_port::stop_bits(serial_port::stop_bits::type::one),
                   ec);
  if (ec) {
    return ErrorCode::kFailedToConfigurePort;
  }

  port_.set_option(
      serial_port::flow_control(serial_port::flow_control::type::none), ec);
  if (ec) {
    return ErrorCode::kFailedToConfigurePort;
  }

  return ErrorCode::kSuccess;
}

void SerialDriver::Impl::Close() noexcept {
  boost::system::error_code ignored;
  port_.close(ignored);
}

ErrorCode SerialDriver::Impl::Write(std::string_view data,
                                    unsigned int timeout_ms) {
  static_cast<void>(timeout_ms);

  if (!port_.is_open()) {
    return ErrorCode::kNotConnected;
  }

  char dev_id_buf[8]{};
  fmt::format_to_n(dev_id_buf, sizeof(dev_id_buf) - 1, "@{} ", dev_id_);
  std::string_view dev_id_str{dev_id_buf};

  std::array<boost::asio::const_buffer, 2> bufs = {
    boost::asio::buffer(dev_id_str),
    boost::asio::buffer(data),
  };

  boost::system::error_code ec{};

  boost::asio::write(port_, bufs, ec);
  if (ec) {
    return ErrorCode::kSendFailed;
  }

  return ErrorCode::kSuccess;
}

Result<std::string> SerialDriver::Impl::ReadLine(unsigned int timeout_ms) {
  if (!port_.is_open()) {
    return Err(ErrorCode::kNotConnected);
  }

  boost::system::error_code ec;

  const auto read_handler = [&](auto&& e, auto&&) { ec = e; };
  boost::asio::async_read_until(port_, input_buffer_, '\n', read_handler);

  if (!Run(timeout_ms)) {
    return Err(ErrorCode::kReadTimedOut);
  }

  if (ec) {
    return Err(ErrorCode::kReadFailed);
  }

  std::string line;
  std::istream is(&input_buffer_);
  std::getline(is, line);

  return line;
}

void SerialDriver::Impl::SetDeviceID(unsigned int id) {
  dev_id_ = std::clamp(id, 0U, 32U);
}

unsigned int SerialDriver::Impl::GetDeviceID() const { return dev_id_; }

bool SerialDriver::Impl::IsBroadcast() const { return dev_id_ > 31; }

bool SerialDriver::Impl::Run(unsigned int timeout_ms) {
  io_service_.restart();

  io_service_.run_for(std::chrono::milliseconds(timeout_ms));

  if (!io_service_.stopped()) {
    boost::system::error_code ignored;
    port_.cancel(ignored);
    io_service_.run();
    return false;
  }

  return true;
}

}  // namespace bci::abs::drivers
