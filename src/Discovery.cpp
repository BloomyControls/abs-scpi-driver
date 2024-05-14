#include <bci/abs/Discovery.h>

#include <memory>
#include <string_view>

#include <bci/abs/CommonTypes.h>
#include <bci/abs/UdpMulticastDriver.h>

#include "ScpiUtil.h"
#include "Util.h"

namespace bci::abs {

using ec = ErrorCode;
using util::Err;

Result<EthernetDeviceList> MulticastDiscover(std::string_view interface_ip) {
  EthernetDeviceList devices;

  auto driver = std::make_shared<drivers::UdpMcastDriver>();

  ec ret = driver->Open(interface_ip);
  if (ret != ec::kSuccess) {
    return Err(ret);
  }

  ret = driver->Write("*IDN?\r\n", 100);
  if (ret != ec::kSuccess) {
    return Err(ret);
  }

  bool done = false;
  while (!done) {
    if (auto resp = driver->ReadLineFrom(100)) {
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

}  // namespace bci::abs
