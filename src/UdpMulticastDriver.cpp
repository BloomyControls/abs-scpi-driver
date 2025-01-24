/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include <bci/abs/UdpMulticastDriver.h>

#include <array>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
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

static constexpr std::string_view kMulticastAddr = "239.188.26.181";

using util::Err;

struct UdpMcastDriver::Impl {
  Impl();

  ~Impl();

  ErrorCode Open(std::string_view interface_ip);

  void Close() noexcept;

  ErrorCode Write(std::string_view data, unsigned int timeout_ms);

  Result<std::string> ReadLine(unsigned int timeout_ms);

  Result<AddressedResponse> ReadLineFrom(unsigned int timeout_ms);

 private:
  static constexpr std::size_t kBufLen = 8192;

  boost::asio::io_service io_service_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint endpoint_;
  std::array<std::uint8_t, kBufLen> buf_;

  bool Run(unsigned int timeout_ms);
};

UdpMcastDriver::UdpMcastDriver() : impl_(std::make_shared<Impl>()) {}

UdpMcastDriver::~UdpMcastDriver() { Close(); }

ErrorCode UdpMcastDriver::Open(std::string_view interface_ip) {
  return impl_->Open(interface_ip);
}

void UdpMcastDriver::Close() noexcept { impl_->Close(); }

ErrorCode UdpMcastDriver::Write(std::string_view data,
                                unsigned int timeout_ms) const {
  return impl_->Write(data, timeout_ms);
}

Result<std::string> UdpMcastDriver::ReadLine(unsigned int timeout_ms) const {
  return impl_->ReadLine(timeout_ms);
}

Result<UdpMcastDriver::AddressedResponse> UdpMcastDriver::ReadLineFrom(
    unsigned int timeout_ms) const {
  return impl_->ReadLineFrom(timeout_ms);
}

UdpMcastDriver::Impl::Impl()
    : io_service_(),
      socket_(io_service_),
      endpoint_(),
      buf_{} { }

UdpMcastDriver::Impl::~Impl() { Close(); }

ErrorCode UdpMcastDriver::Impl::Open(std::string_view interface_ip) {
  if (socket_.is_open()) {
    return ErrorCode::kAlreadyConnected;
  }

  boost::system::error_code ec{};
  auto local_address = boost::asio::ip::make_address_v4(interface_ip, ec);
  if (ec) {
    return ErrorCode::kInvalidIPAddress;
  }

  auto remote_address = boost::asio::ip::make_address_v4(kMulticastAddr, ec);
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

  boost::asio::ip::multicast::outbound_interface iface_opt(local_address);
  socket_.set_option(iface_opt, ec);
  if (ec) {
    socket_.close();
    return ErrorCode::kSocketError;
  }

  boost::asio::ip::multicast::join_group join_opt(remote_address);
  socket_.set_option(join_opt, ec);
  if (ec) {
    socket_.close();
    return ErrorCode::kFailedToJoinGroup;
  }

  return ErrorCode::kSuccess;
}

void UdpMcastDriver::Impl::Close() noexcept {
  boost::system::error_code ignored;
  if (socket_.is_open()) {
    socket_.close(ignored);
  }
}

ErrorCode UdpMcastDriver::Impl::Write(std::string_view data,
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

Result<std::string> UdpMcastDriver::Impl::ReadLine(unsigned int timeout_ms) {
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

Result<UdpMcastDriver::AddressedResponse> UdpMcastDriver::Impl::ReadLineFrom(
    unsigned int timeout_ms) {
  if (!socket_.is_open()) {
    return Err(ErrorCode::kNotConnected);
  }

  boost::system::error_code ec;

  std::size_t read_len{};
  udp::endpoint source;

  const auto read_handler = [&](auto&& e, std::size_t len) {
    ec = e;
    read_len = len;
  };
  socket_.async_receive_from(boost::asio::buffer(buf_), source, read_handler);

  if (!Run(timeout_ms)) {
    return Err(ErrorCode::kReadTimedOut);
  }

  if (ec) {
    return Err(ErrorCode::kReadFailed);
  }

  std::string line((const char*)buf_.data(), read_len);

  return AddressedResponse{source.address().to_string(), std::move(line)};
}

bool UdpMcastDriver::Impl::Run(unsigned int timeout_ms) {
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
