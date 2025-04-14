/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include <bci/abs/UdpDriver.h>

#include <array>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/write.hpp>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include "Util.h"

using boost::asio::ip::udp;

namespace bci::abs::drivers {

using util::Err;

struct UdpDriver::Impl {
  Impl();

  ~Impl();

  ErrorCode Open(std::string_view ip);

  ErrorCode Open(std::string_view local_ip, std::string_view target_ip);

  void Close() noexcept;

  ErrorCode Write(std::string_view data, unsigned int timeout_ms);

  Result<std::string> ReadLine(unsigned int timeout_ms);

 private:
  static constexpr std::size_t kBufLen = 8192;

  boost::asio::io_context io_context_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint endpoint_;
  std::array<std::uint8_t, kBufLen> buf_;

  bool Run(unsigned int timeout_ms);
};

UdpDriver::UdpDriver() : impl_(std::make_shared<Impl>()) {}

UdpDriver::~UdpDriver() { Close(); }

ErrorCode UdpDriver::Open(std::string_view ip) { return impl_->Open(ip); }

ErrorCode UdpDriver::Open(std::string_view local_ip,
                          std::string_view target_ip) {
  return impl_->Open(local_ip, target_ip);
}

void UdpDriver::Close() noexcept { impl_->Close(); }

ErrorCode UdpDriver::Write(std::string_view data,
                           unsigned int timeout_ms) const {
  return impl_->Write(data, timeout_ms);
}

Result<std::string> UdpDriver::ReadLine(unsigned int timeout_ms) const {
  return impl_->ReadLine(timeout_ms);
}

UdpDriver::Impl::Impl()
    : io_context_(),
      socket_(io_context_),
      endpoint_(),
      buf_{} { }

UdpDriver::Impl::~Impl() { Close(); }

ErrorCode UdpDriver::Impl::Open(std::string_view ip) {
  return Open("0.0.0.0", ip);
}

ErrorCode UdpDriver::Impl::Open(std::string_view local_ip,
                                std::string_view target_ip) {
  if (socket_.is_open()) {
    return ErrorCode::kAlreadyConnected;
  }

  boost::system::error_code ec{};
  auto local_address = boost::asio::ip::make_address_v4(local_ip, ec);
  if (ec) {
    return ErrorCode::kInvalidIPAddress;
  }

  auto remote_address = boost::asio::ip::make_address_v4(target_ip, ec);
  if (ec) {
    return ErrorCode::kInvalidIPAddress;
  }

  endpoint_ = boost::asio::ip::udp::endpoint(remote_address, 5025);

  socket_.open(boost::asio::ip::udp::v4(), ec);
  if (ec) {
    return ErrorCode::kSocketError;
  }

  // TODO: is this a reasonable size?
  boost::asio::socket_base::receive_buffer_size rx_buf_opt{1024 * 64};
  socket_.set_option(rx_buf_opt, ec);
  if (ec) {
    socket_.close();
    return ErrorCode::kSocketError;
  }

  socket_.set_option(udp::socket::reuse_address(true), ec);
  if (ec) {
    socket_.close();
    return ErrorCode::kSocketError;
  }

  boost::asio::ip::udp::endpoint local_endpoint(local_address, 0);

  socket_.bind(local_endpoint, ec);
  if (ec) {
    socket_.close();
    return ErrorCode::kFailedToBindSocket;
  }

  return ErrorCode::kSuccess;
}

void UdpDriver::Impl::Close() noexcept {
  boost::system::error_code ignored;
  if (socket_.is_open()) {
    socket_.close(ignored);
  }
}

ErrorCode UdpDriver::Impl::Write(std::string_view data,
                                 unsigned int timeout_ms) {
  if (!socket_.is_open()) {
    return ErrorCode::kNotConnected;
  }

  boost::system::error_code ec;

  const auto write_handler = [&](auto&& e, auto&&) { ec = e; };
  socket_.async_send_to(boost::asio::buffer(data), endpoint_, write_handler);

  if (!Run(timeout_ms)) {
    return ErrorCode::kSendTimedOut;
  }

  if (ec) {
    return ErrorCode::kSendFailed;
  }

  return ErrorCode::kSuccess;
}

Result<std::string> UdpDriver::Impl::ReadLine(unsigned int timeout_ms) {
  if (!socket_.is_open()) {
    return Err(ErrorCode::kNotConnected);
  }

  boost::system::error_code ec;

  std::size_t read_len{};

  const auto read_handler = [&](auto&& e, std::size_t len) {
    ec = e;
    read_len = len;
  };
  socket_.async_receive(boost::asio::buffer(buf_), read_handler);

  if (!Run(timeout_ms)) {
    return Err(ErrorCode::kReadTimedOut);
  }

  if (ec) {
    return Err(ErrorCode::kReadFailed);
  }

  std::string line((const char*)buf_.data(), read_len);

  return line;
}

bool UdpDriver::Impl::Run(unsigned int timeout_ms) {
  io_context_.restart();

  io_context_.run_for(std::chrono::milliseconds(timeout_ms));

  if (!io_context_.stopped()) {
    boost::system::error_code ignored;
    socket_.cancel(ignored);
    io_context_.run();
    return false;
  }

  return true;
}

}  // namespace bci::abs::drivers
