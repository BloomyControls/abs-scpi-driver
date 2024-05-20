#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_TCPDRIVER_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_TCPDRIVER_H

#include <memory>
#include <string>
#include <string_view>

#include "CommDriver.h"
#include "CommonTypes.h"

namespace bci::abs::drivers {

/**
 * @brief TCP driver.
 *
 * @note TCP is much slower and less deterministic than UDP. When possible, it's
 * strongly recommended that UDP be preferred over TCP.
 */
class TcpDriver final : public CommDriver {
 public:
  /// CTOR.
  TcpDriver();

  /// DTOR. Closes any ongoing connection.
  ~TcpDriver();

  /**
   * @brief Connect to the ABS.
   *
   * @param[in] ip device IP address
   * @param[in] timeout_ms connection timeout in milliseconds
   *
   * @return An error code.
   */
  ErrorCode Connect(std::string_view ip, unsigned int timeout_ms);

  /// Close the connection to the ABS.
  void Close() noexcept;

  /**
   * @brief Send data over TCP.
   *
   * @param[in] data data to send
   * @param[in] timeout_ms send timeout in milliseconds
   *
   * @return An error code.
   */
  ErrorCode Write(std::string_view data, unsigned int timeout_ms) const;

  /**
   * @brief Read a line over TCP.
   *
   * @param[in] timeout_ms read timeout in milliseconds
   *
   * @return Result containing the line read or an error code.
   */
  Result<std::string> ReadLine(unsigned int timeout_ms) const;

 private:
  struct Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace bci::abs::drivers

#endif /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_TCPDRIVER_H */
