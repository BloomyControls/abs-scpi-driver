#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_DISCOVERY_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_DISCOVERY_H

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "CommonTypes.h"

namespace bci::abs {

struct EthernetDevice {
  std::string ip;
  std::string serial;
};

struct SerialDevice {
  std::uint8_t id;
  std::string serial;
};

using EthernetDeviceList = std::vector<EthernetDevice>;

using SerialDeviceList = std::vector<SerialDevice>;

Result<EthernetDeviceList> MulticastDiscovery(std::string_view interface_ip);

Result<SerialDeviceList> SerialDiscovery(std::string_view port,
                                         unsigned int max_devices);

}  // namespace bci::abs

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_DISCOVERY_H */
