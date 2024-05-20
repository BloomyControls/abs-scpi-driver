/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_UDPMULTICASTDRIVER_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_UDPMULTICASTDRIVER_H

#include <memory>
#include <string>
#include <string_view>

#include "CommDriver.h"
#include "CommonTypes.h"

namespace bci::abs::drivers {

/**
 * @brief UDP multicast driver for addressing multiple units at the same time.
 */
class UdpMcastDriver final : public CommDriver {
 public:
  /// Structure containing a response and the IP from which it was received.
  struct AddressedResponse {
    /// Source IP address.
    std::string ip;
    /// Response data.
    std::string data;
  };

  /// CTOR.
  UdpMcastDriver();

  /// DTOR.
  ~UdpMcastDriver();

  /**
   * @brief Open a socket for communication with the ABS.
   *
   * @param[in] interface_ip address of local interface to bind to
   *
   * @return An error code.
   */
  ErrorCode Open(std::string_view interface_ip);

  /// Close the socket.
  void Close() noexcept;

  /**
   * @brief Send data over UDP multicast.
   *
   * @param[in] data data to send
   * @param[in] timeout_ms send timeout in milliseconds
   *
   * @return An error code.
   */
  ErrorCode Write(std::string_view data, unsigned int timeout_ms) const;

  /**
   * @brief Read a line over UDP multicast. This is not a very useful function
   * with multicast, as there may be many responses to one query. See
   * ReadLineFrom() for a more useful function.
   *
   * @param[in] timeout_ms read timeout in milliseconds
   *
   * @return Result containing the line read or an error code.
   */
  Result<std::string> ReadLine(unsigned int timeout_ms) const;

  /**
   * @brief Read a line over UDP multicast, returning the line and the sender's
   * IP address. This is primarily intended to be used for device discovery.
   *
   * @param[in] timeout_ms read timeout in milliseconds
   *
   * @return Result containing the response or an error code.
   */
  Result<AddressedResponse> ReadLineFrom(unsigned int timeout_ms) const;

  /**
   * @brief Whether the device is send-only in the general case. Always true for
   * multicast devices.
   *
   * @return True.
   */
  bool IsSendOnly() const { return true; }

 private:
  struct Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace bci::abs::drivers

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_UDPMULTICASTDRIVER_H */
