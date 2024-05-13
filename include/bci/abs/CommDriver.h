#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMDRIVER_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMDRIVER_H

#include <string>
#include <string_view>

#include "CommonTypes.h"

namespace bci::abs::drivers {

class CommDriver {
 public:
  virtual ~CommDriver() = default;

  virtual ErrorCode Write(std::string_view data,
                          unsigned int timeout_ms) const = 0;

  virtual Result<std::string> ReadLine(unsigned int timeout_ms) const = 0;

  virtual void SetDeviceID(unsigned int id) { static_cast<void>(id); }

  // useful for things like UDP broadcast and multicast, or RS-485 with an ID
  // higher than 255
  virtual bool IsSendOnly() const { return false; }
};

}  // namespace bci::abs::drivers

#endif /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMDRIVER_H */
