/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

/**
 * @file
 * @brief Device discovery functionality.
 */
#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_DISCOVERY_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_DISCOVERY_H

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "CommonTypes.h"

namespace bci::abs {

/// Structure containing information about a discovered Ethernet device.
struct EthernetDevice {
  std::string ip;      ///< IP address.
  std::string serial;  ///< Serial number.
};

/// Structure containing information about a discovered serial device.
struct SerialDevice {
  std::uint8_t id;     ///< Unit ID.
  std::string serial;  ///< Serial number.
};

/// List of discovered Ethernet devices.
using EthernetDeviceList = std::vector<EthernetDevice>;

/// List of discovered serial devices.
using SerialDeviceList = std::vector<SerialDevice>;

/**
 * @brief Discover any units on the network over UDP multicast.
 *
 * This function operates by sending an identification query to the multicast
 * group and enumerating all devices that reply.
 *
 * @param[in] interface_ip IP address of the local NIC to use
 *
 * @return Result containing a list of devices or an error code.
 */
Result<EthernetDeviceList> MulticastDiscovery(std::string_view interface_ip);

/**
 * @brief Discover any units on a serial bus.
 *
 * This function operates by sending an identification query to each address in
 * the specified range and enumerating all devices that reply.
 *
 * @param[in] port serial port
 * @param[in] first_id first serial ID to query, 0-31
 * @param[in] last_id last serial ID to query, 0-31 (inclusive)
 *
 * @return Result containing a list of devices or an error code.
 */
Result<SerialDeviceList> SerialDiscovery(std::string_view port,
                                         std::uint8_t first_id,
                                         std::uint8_t last_id);

}  // namespace bci::abs

#endif /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_DISCOVERY_H */
