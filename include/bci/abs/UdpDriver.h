/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_UDPDRIVER_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_UDPDRIVER_H

#include <memory>
#include <string>
#include <string_view>

#include "CommDriver.h"
#include "CommonTypes.h"

namespace bci::abs::drivers {

/**
 * @brief UDP driver.
 */
class UdpDriver final : public CommDriver {
 public:
  /// CTOR.
  UdpDriver();

  /// DTOR.
  ~UdpDriver();

  /**
   * @brief Open a socket for communication with the ABS.
   *
   * @param[in] ip device IP address
   *
   * @return An error code.
   */
  ErrorCode Open(std::string_view ip);

  /**
   * @brief Open a socket for communication with the ABS bound to a specific
   * local IP address. Prefer this function if you have more than one NIC.
   *
   * @param[in] local_ip local interface IP to bind to
   * @param[in] target_ip device IP address
   *
   * @return An error code.
   */
  ErrorCode Open(std::string_view local_ip, std::string_view target_ip);

  /// Close the socket.
  void Close() noexcept;

  /**
   * @brief Send data over UDP.
   *
   * @param[in] data data to send
   * @param[in] timeout_ms send timeout in milliseconds
   *
   * @return An error code.
   */
  ErrorCode Write(std::string_view data, unsigned int timeout_ms) const;

  /**
   * @brief Read a line over UDP.
   *
   * @param[in] timeout_ms read timeout in milliseconds
   *
   * @return Result containing the line read or an error code.
   */
  Result<std::string> ReadLine(unsigned int timeout_ms) const;

 private:
  struct Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace bci::abs::drivers

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_UDPDRIVER_H */
