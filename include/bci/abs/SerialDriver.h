#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SERIALDRIVER_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SERIALDRIVER_H

#include <memory>
#include <string>
#include <string_view>

#include "CommDriver.h"
#include "CommonTypes.h"

namespace bci::abs::drivers {

/**
 * @brief Serial (RS-485) driver.
 */
class SerialDriver final : public CommDriver {
 public:
  /// CTOR.
  SerialDriver();

  /// DTOR.
  ~SerialDriver();

  /**
   * @brief Open the serial port.
   *
   * @param[in] port the serial port to open, such as COM5 or /dev/ttyS2
   *
   * @return An error code.
   */
  ErrorCode Open(const std::string& port);

  /// Close the serial port.
  void Close() noexcept;

  /**
   * @brief Write data over the serial port.
   *
   * @param[in] data data to send
   * @param[in] timeout_ms send timeout in milliseconds (ignored; unsupported
   * by serial ports)
   *
   * @return An error code.
   */
  ErrorCode Write(std::string_view data, unsigned int timeout_ms) const;

  /**
   * @brief Read a line from  the serial port.
   *
   * @param[in] timeout_ms read timeout in milliseconds
   *
   * @return Result containing the line read or an error code.
   */
  Result<std::string> ReadLine(unsigned int timeout_ms) const;

  /**
   * @brief Set the target device ID.
   *
   * @param[in] id target device ID, 0-255 or 256+ to broadcast to all devices
   * on the bus
   */
  void SetDeviceID(unsigned int id);

  /**
   * @brief Get the target device ID.
   *
   * @return Target device ID.
   */
  unsigned int GetDeviceID() const;

  /**
   * @brief Determine whether the port is send-only. This is true when the
   * target device ID is 256+.
   *
   * @return Whether the port is send-only.
   */
  bool IsSendOnly() const;

 private:
  struct Impl;
  std::shared_ptr<Impl> impl_;
};

}  // namespace bci::abs::drivers

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SERIALDRIVER_H */
