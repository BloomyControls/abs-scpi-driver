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
#include <memory>
#include <string>
#include <string_view>

#include "Util.h"

using boost::asio::deadline_timer;
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
  boost::asio::deadline_timer deadline_;
  boost::asio::streambuf input_buffer_;

  void CheckDeadline();
};

TcpDriver::TcpDriver() : impl_(std::make_shared<Impl>()) {}

TcpDriver::~TcpDriver() { Close(); }

ErrorCode TcpDriver::Connect(std::string_view ip,
                             unsigned int timeout_ms) {
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
      deadline_(io_service_),
      input_buffer_() {
  deadline_.expires_at(boost::posix_time::pos_infin);
  CheckDeadline();
}

TcpDriver::Impl::~Impl() {}

ErrorCode TcpDriver::Impl::Connect(std::string_view ip,
                                   unsigned int timeout_ms) {
  boost::system::error_code ec{};

  auto addr = boost::asio::ip::make_address_v4(ip, ec);
  if (ec) {
    return ErrorCode::kInvalidIPAddress;
  }

  tcp::endpoint endpoint(addr, 5025);

  deadline_.expires_from_now(boost::posix_time::milliseconds(timeout_ms));

  // would_block is never set from async_functions, so it's a safe way to signal
  // an incomplete async operation
  ec = boost::asio::error::would_block;

  const auto connect_handler = [&](auto&& e) { ec = e; };
  socket_.async_connect(endpoint, connect_handler);

  do {
    io_service_.run_one();
  } while (ec == boost::asio::error::would_block);

  if (ec) {
    return ErrorCode::kConnectionFailed;
  }

  if (!socket_.is_open()) {
    return ErrorCode::kConnectionTimedOut;
  }

  return ErrorCode::kSuccess;
}

void TcpDriver::Impl::Close() noexcept {
  boost::system::error_code ignored;
  socket_.shutdown(tcp::socket::shutdown_send, ignored);
  socket_.shutdown(tcp::socket::shutdown_receive, ignored);
  socket_.close(ignored);
}

ErrorCode TcpDriver::Impl::Write(std::string_view data,
                                 unsigned int timeout_ms) {
  if (!socket_.is_open()) {
    return ErrorCode::kNotConnected;
  }

  deadline_.expires_from_now(boost::posix_time::milliseconds(timeout_ms));

  boost::system::error_code ec = boost::asio::error::would_block;

  const auto write_handler = [&](auto&& e, auto&&) { ec = e; };
  boost::asio::async_write(socket_, boost::asio::buffer(data), write_handler);

  do {
    io_service_.run_one();
  } while (ec == boost::asio::error::would_block);

  if (ec) {
    return ErrorCode::kSendFailed;
  }

  if (!socket_.is_open()) {
    return ErrorCode::kSendTimedOut;
  }

  return ErrorCode::kSuccess;
}

Result<std::string> TcpDriver::Impl::ReadLine(unsigned int timeout_ms) {
  if (!socket_.is_open()) {
    return Err(ErrorCode::kNotConnected);
  }

  deadline_.expires_from_now(boost::posix_time::milliseconds(timeout_ms));

  boost::system::error_code ec = boost::asio::error::would_block;

  const auto read_handler = [&](auto&& e, auto&&) { ec = e; };
  boost::asio::async_read_until(socket_, input_buffer_, '\n', read_handler);

  do {
    io_service_.run_one();
  } while (ec == boost::asio::error::would_block);

  if (ec) {
    return Err(ErrorCode::kReadFailed);
  }

  if (!socket_.is_open()) {
    return Err(ErrorCode::kReadTimedOut);
  }

  std::string line;
  std::istream is(&input_buffer_);
  std::getline(is, line);

  return line;
}

void TcpDriver::Impl::CheckDeadline() {
  if (deadline_.expires_at() <= deadline_timer::traits_type::now()) {
    Close();
    deadline_.expires_at(boost::posix_time::pos_infin);
  }

  deadline_.async_wait([&](auto&&) { this->CheckDeadline(); });
}

}  // namespace bci::abs::drivers
