#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_UDPMULTICASTDRIVER_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_UDPMULTICASTDRIVER_H

#include <memory>
#include <string>
#include <string_view>

#include "CommDriver.h"
#include "CommonTypes.h"

namespace bci::abs::drivers {

class UdpMcastDriver final : public CommDriver {
 public:
  UdpMcastDriver();

  ~UdpMcastDriver();

  ErrorCode Open();

  void Close() noexcept;

  ErrorCode Write(std::string_view data, unsigned int timeout_ms) const;

  Result<std::string> ReadLine(unsigned int timeout_ms) const;

  bool IsSendOnly() const { return true; }

 private:
  struct Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace bci::abs::drivers

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_UDPMULTICASTDRIVER_H */
