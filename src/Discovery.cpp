#include <bci/abs/CommonTypes.h>
#include <bci/abs/Discovery.h>
#include <bci/abs/SerialDriver.h>
#include <bci/abs/UdpMulticastDriver.h>

#include <array>
#include <string>
#include <string_view>

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
                                         std::uint8_t first_id,
                                         std::uint8_t last_id) {
  SerialDeviceList devices;

  if (last_id < first_id) {
    return Err(ec::kInvalidArgument);
  }

  drivers::SerialDriver driver;

  ec ret = driver.Open(std::string(port));
  if (ret != ec::kSuccess) {
    return Err(ret);
  }

  int total = last_id - first_id + 1;

  for (int i = 0; i < total; ++i) {
    driver.SetDeviceID(first_id + i);
    ret = driver.Write("*IDN?\r\n", 100);
    if (auto resp = driver.ReadLine(50)) {
      std::array<std::string_view, 4> idn;
      if (scpi::SplitRespMnemonics(*resp, idn) != ErrorCode::kSuccess) {
        return Err(ErrorCode::kInvalidResponse);
      }
      devices.emplace_back(first_id + i, std::string(idn[2]));
    } else {
      if (resp.error() != ec::kReadTimedOut) {
        return Err(resp.error());
      }
    }
  }

  return devices;
}

}  // namespace bci::abs
