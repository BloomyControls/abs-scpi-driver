/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include <bci/abs/TcpDriver.h>

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <chrono>
#include <memory>
#include <string>
#include <string_view>

#include "Util.h"

using boost::asio::ip::tcp;

namespace bci::abs::drivers {

using util::Err;

struct TcpDriver::Impl {
  Impl();

  ~Impl();

  ErrorCode Connect(std::string_view ip, unsigned int timeout_ms);

  void Close() noexcept;

  ErrorCode Write(std::string_view data, unsigned int timeout_ms);

  Result<std::string> ReadLine(unsigned int timeout_ms);

 private:
  boost::asio::io_service io_service_;
  boost::asio::ip::tcp::socket socket_;
  boost::asio::streambuf input_buffer_;

  bool Run(unsigned int timeout_ms);
};

TcpDriver::TcpDriver() : impl_(std::make_shared<Impl>()) {}

TcpDriver::~TcpDriver() { Close(); }

ErrorCode TcpDriver::Connect(std::string_view ip, unsigned int timeout_ms) {
  return impl_->Connect(ip, timeout_ms);
}

void TcpDriver::Close() noexcept { impl_->Close(); }

ErrorCode TcpDriver::Write(std::string_view data,
                           unsigned int timeout_ms) const {
  return impl_->Write(data, timeout_ms);
}

Result<std::string> TcpDriver::ReadLine(unsigned int timeout_ms) const {
  return impl_->ReadLine(timeout_ms);
}

TcpDriver::Impl::Impl()
    : io_service_(),
      socket_(io_service_),
      input_buffer_() {
  boost::system::error_code ignored;
  socket_.set_option(boost::asio::socket_base::linger(false, 0), ignored);
  socket_.set_option(boost::asio::socket_base::keep_alive(true), ignored);
}

TcpDriver::Impl::~Impl() { Close(); }

ErrorCode TcpDriver::Impl::Connect(std::string_view ip,
                                   unsigned int timeout_ms) {
  boost::system::error_code ec{};

  auto addr = boost::asio::ip::make_address_v4(ip, ec);
  if (ec) {
    return ErrorCode::kInvalidIPAddress;
  }

  tcp::endpoint endpoint(addr, 5025);

  ec = {};
  const auto connect_handler = [&](auto&& e) { ec = e; };
  socket_.async_connect(endpoint, connect_handler);

  if (!Run(timeout_ms)) {
    return ErrorCode::kConnectionTimedOut;
  }

  if (ec) {
    return ErrorCode::kConnectionFailed;
  }

  return ErrorCode::kSuccess;
}

void TcpDriver::Impl::Close() noexcept {
  boost::system::error_code ignored;
  socket_.cancel(ignored);
  socket_.shutdown(tcp::socket::shutdown_send, ignored);
  socket_.shutdown(tcp::socket::shutdown_receive, ignored);
  socket_.close(ignored);
}

ErrorCode TcpDriver::Impl::Write(std::string_view data,
                                 unsigned int timeout_ms) {
  if (!socket_.is_open()) {
    return ErrorCode::kNotConnected;
  }

  boost::system::error_code ec{};

  const auto write_handler = [&](auto&& e, auto&&) { ec = e; };
  boost::asio::async_write(socket_, boost::asio::buffer(data), write_handler);

  if (!Run(timeout_ms)) {
    return ErrorCode::kSendTimedOut;
  }

  if (ec) {
    return ErrorCode::kSendFailed;
  }

  return ErrorCode::kSuccess;
}

Result<std::string> TcpDriver::Impl::ReadLine(unsigned int timeout_ms) {
  if (!socket_.is_open()) {
    return Err(ErrorCode::kNotConnected);
  }

  boost::system::error_code ec{};

  const auto read_handler = [&](auto&& e, auto&&) { ec = e; };
  boost::asio::async_read_until(socket_, input_buffer_, '\n', read_handler);

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

bool TcpDriver::Impl::Run(unsigned int timeout_ms) {
  io_service_.restart();

  io_service_.run_for(std::chrono::milliseconds(timeout_ms));

  if (!io_service_.stopped()) {
    boost::system::error_code ignored;
    socket_.cancel(ignored);
    io_service_.run();
    return false;
  }

  return true;
}

}  // namespace bci::abs::drivers
