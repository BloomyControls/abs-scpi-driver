#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_DISCOVERY_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_DISCOVERY_H

#include <string>
#include <string_view>
#include <vector>

#include "CommonTypes.h"

namespace bci::abs {

struct EthernetDevice {
  std::string ip;
  std::string serial;
};

using EthernetDeviceList = std::vector<EthernetDevice>;

Result<EthernetDeviceList> MulticastDiscover(std::string_view interface_ip);

}  // namespace bci::abs

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_DISCOVERY_H */
