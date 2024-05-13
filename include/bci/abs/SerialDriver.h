#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SERIALDRIVER_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SERIALDRIVER_H

#include <memory>
#include <string>
#include <string_view>

#include "CommDriver.h"
#include "CommonTypes.h"

namespace bci::abs::drivers {

class SerialDriver final : public CommDriver {
 public:
  SerialDriver();

  ~SerialDriver();

  ErrorCode Open(const std::string& port);

  void Close() noexcept;

  ErrorCode Write(std::string_view data, unsigned int timeout_ms) const;

  Result<std::string> ReadLine(unsigned int timeout_ms) const;

  void SetDeviceID(unsigned int id);

  bool IsSendOnly() const;

 private:
  struct Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace bci::abs::drivers

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SERIALDRIVER_H */
