#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_TCPDRIVER_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_TCPDRIVER_H

#include <memory>
#include <string>
#include <string_view>

#include "CommDriver.h"

namespace bci::abs::drivers {

class TcpDriver final : public CommDriver {
 public:
  TcpDriver();

  ~TcpDriver();

  ErrorCode Connect(std::string_view ip, unsigned int timeout_ms);

  void Close() noexcept;

  ErrorCode Write(std::string_view data, unsigned int timeout_ms) const;

  Result<std::string> ReadLine(unsigned int timeout_ms) const;

 private:
  struct Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace bci::abs::drivers

#endif /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_TCPDRIVER_H */
