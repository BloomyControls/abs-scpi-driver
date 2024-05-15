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
 * the specified range and enumerating all devices that reply. Because this is
 * much more time consuming than the multicast approach, it's recommended to
 * limit the range of ID searched to a range more relevant to a system. With
 * no units on the bus, a discovery of the full ID range 0-255 takes
 * approximately 15 seconds or more, though this may depend on the exact
 * hardware and operating system used. Checking 0-5 (6 addresses) took about
 * half a second on the same setup.
 *
 * @param[in] port serial port
 * @param[in] first_id first serial ID to query
 * @param[in] last_id last serial ID to query (inclusive)
 *
 * @return Result containing a list of devices or an error code.
 */
Result<SerialDeviceList> SerialDiscovery(std::string_view port,
                                         std::uint8_t first_id,
                                         std::uint8_t last_id);

}  // namespace bci::abs

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_DISCOVERY_H */
