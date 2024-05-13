#include <bci/abs/UdpMulticastDriver.h>

#include <array>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/write.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include "Util.h"

using boost::asio::deadline_timer;
using boost::asio::ip::udp;

namespace bci::abs::drivers {

static constexpr std::string_view kMulticastAddr = "239.188.26.181";

using util::Err;

struct UdpMcastDriver::Impl {
  Impl();

  ErrorCode Open();

  ErrorCode Open(std::string_view local_ip);

  void Close() noexcept;

  ErrorCode Write(std::string_view data, unsigned int timeout_ms);

  Result<std::string> ReadLine(unsigned int timeout_ms);

 private:
  static constexpr std::size_t kBufLen = 8192;

  boost::asio::io_service io_service_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::deadline_timer deadline_;
  boost::asio::ip::udp::endpoint endpoint_;
  std::array<std::uint8_t, kBufLen> buf_;

  void CheckDeadline();
};

UdpMcastDriver::UdpMcastDriver() : impl_(std::make_shared<Impl>()) {}

UdpMcastDriver::~UdpMcastDriver() { Close(); }

ErrorCode UdpMcastDriver::Open() { return impl_->Open(); }

void UdpMcastDriver::Close() noexcept { impl_->Close(); }

ErrorCode UdpMcastDriver::Write(std::string_view data,
                           unsigned int timeout_ms) const {
  return impl_->Write(data, timeout_ms);
}

Result<std::string> UdpMcastDriver::ReadLine(unsigned int timeout_ms) const {
  return impl_->ReadLine(timeout_ms);
}

UdpMcastDriver::Impl::Impl()
    : io_service_(),
      socket_(io_service_),
      deadline_(io_service_),
      endpoint_(),
      buf_{} {
  deadline_.expires_at(boost::posix_time::pos_infin);
  CheckDeadline();
}

ErrorCode UdpMcastDriver::Impl::Open() {
  return Open("0.0.0.0");
}

ErrorCode UdpMcastDriver::Impl::Open(std::string_view local_ip) {
  if (socket_.is_open()) {
    return ErrorCode::kAlreadyConnected;
  }

  boost::system::error_code ec{};
  auto local_address = boost::asio::ip::make_address_v4(local_ip, ec);
  if (ec) {
    return ErrorCode::kInvalidIPAddress;
  }

  auto remote_address = boost::asio::ip::make_address_v4(kMulticastAddr, ec);
  if (ec) {
    return ErrorCode::kInvalidIPAddress;
  }

  endpoint_ = boost::asio::ip::udp::endpoint(remote_address, 5025);

  socket_.open(endpoint_.protocol(), ec);
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

#if 0
  boost::asio::ip::udp::endpoint local_endpoint(local_address, 0);

  socket_.bind(local_endpoint, ec);
  if (ec) {
    socket_.close();
    return ErrorCode::kFailedToBindSocket;
  }
#endif

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

  deadline_.expires_from_now(boost::posix_time::milliseconds(timeout_ms));

  boost::system::error_code ec = boost::asio::error::would_block;

  const auto write_handler = [&](auto&& e, auto&&) { ec = e; };
  socket_.async_send_to(boost::asio::buffer(data), endpoint_, write_handler);

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

Result<std::string> UdpMcastDriver::Impl::ReadLine(unsigned int timeout_ms) {
  if (!socket_.is_open()) {
    return Err(ErrorCode::kNotConnected);
  }

  deadline_.expires_from_now(boost::posix_time::milliseconds(timeout_ms));

  boost::system::error_code ec = boost::asio::error::would_block;

  std::size_t read_len{};

  const auto read_handler = [&](auto&& e, std::size_t len) {
    ec = e;
    read_len = len;
  };
  socket_.async_receive(boost::asio::buffer(buf_), read_handler);

  do {
    io_service_.run_one();
  } while (ec == boost::asio::error::would_block);

  if (ec) {
    return Err(ErrorCode::kReadFailed);
  }

  if (!socket_.is_open()) {
    return Err(ErrorCode::kReadTimedOut);
  }

  std::string line((const char*)buf_.data(), read_len);

  return line;
}

void UdpMcastDriver::Impl::CheckDeadline() {
  if (deadline_.expires_at() <= deadline_timer::traits_type::now()) {
    Close();
    deadline_.expires_at(boost::posix_time::pos_infin);
  }

  deadline_.async_wait([&](auto&&) { this->CheckDeadline(); });
}

}  // namespace bci::abs::drivers
