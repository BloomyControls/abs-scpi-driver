#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_UDPDRIVER_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_UDPDRIVER_H

#include <memory>
#include <string>
#include <string_view>

#include "CommDriver.h"
#include "CommonTypes.h"

namespace bci::abs::drivers {

class UdpDriver final : public CommDriver {
 public:
  UdpDriver();

  ~UdpDriver();

  ErrorCode Open(std::string_view ip);

  ErrorCode Open(std::string_view local_ip, std::string_view target_ip);

  void Close() noexcept;

  ErrorCode Write(std::string_view data, unsigned int timeout_ms) const;

  Result<std::string> ReadLine(unsigned int timeout_ms) const;

 private:
  struct Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace bci::abs::drivers

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_UDPDRIVER_H */
