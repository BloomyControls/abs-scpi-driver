#include <bci/abs/Discovery.h>

#include <array>
#include <string>
#include <string_view>

#include <bci/abs/CommonTypes.h>
#include <bci/abs/SerialDriver.h>
#include <bci/abs/UdpMulticastDriver.h>

#include "ScpiUtil.h"
#include "Util.h"

namespace bci::abs {

using ec = ErrorCode;
using util::Err;

Result<EthernetDeviceList> MulticastDiscovery(std::string_view interface_ip) {
  EthernetDeviceList devices;

  drivers::UdpMcastDriver driver;

  ec ret = driver.Open(interface_ip);
  if (ret != ec::kSuccess) {
    return Err(ret);
  }

  ret = driver.Write("*IDN?\r\n", 100);
  if (ret != ec::kSuccess) {
    return Err(ret);
  }

  bool done = false;
  while (!done) {
    if (auto resp = driver.ReadLineFrom(100)) {
      std::array<std::string_view, 4> idn;
      if (scpi::SplitRespMnemonics(resp->data, idn) != ErrorCode::kSuccess) {
        return Err(ErrorCode::kInvalidResponse);
      }
      devices.emplace_back(resp->ip, std::string(idn[2]));
    } else {
      done = true;
      if (resp.error() != ec::kReadTimedOut) {
        return Err(resp.error());
      }
    }
  }

  return devices;
}

Result<SerialDeviceList> SerialDiscovery(std::string_view port,
                                         unsigned int max_devices) {
  SerialDeviceList devices;

  if (max_devices == 0 || max_devices > 256) {
    max_devices = 256;
  }

  drivers::SerialDriver driver;

  ec ret = driver.Open(std::string(port));
  if (ret != ec::kSuccess) {
    return Err(ret);
  }

  for (std::uint8_t id = 0; max_devices > 0; ++id, --max_devices) {
    driver.SetDeviceID(id);
    ret = driver.Write("*IDN?\r\n", 100);
    if (auto resp = driver.ReadLine(10)) {
      std::array<std::string_view, 4> idn;
      if (scpi::SplitRespMnemonics(*resp, idn) != ErrorCode::kSuccess) {
        return Err(ErrorCode::kInvalidResponse);
      }
      devices.emplace_back(id, std::string(idn[2]));
    } else {
      if (resp.error() != ec::kReadTimedOut) {
        return Err(resp.error());
      }
    }
  }

  return devices;
}

}  // namespace bci::abs
