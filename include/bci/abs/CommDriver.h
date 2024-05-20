#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMDRIVER_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMDRIVER_H

#include <string>
#include <string_view>

#include "CommonTypes.h"

namespace bci::abs::drivers {

/**
 * @brief Base class for all ABS SCPI comm drivers.
 */
class CommDriver {
 public:
  virtual ~CommDriver() = default;

  /**
   * @brief Write data with a timeout. The concrete driver type may or may not
   * support a write timeout feature.
   *
   * @param[in] data data to send
   * @param[in] timeout_ms send timeout in milliseconds (may be ignored by some
   * drivers)
   *
   * @return An error code.
   */
  virtual ErrorCode Write(std::string_view data,
                          unsigned int timeout_ms) const = 0;

  /**
   * @brief Read a line from the device with a timeout.
   *
   * @param[in] timeout_ms read timeout in milliseconds
   *
   * @return Result containing the line read or an error code.
   */
  virtual Result<std::string> ReadLine(unsigned int timeout_ms) const = 0;

  /**
   * @brief Set the target device ID. Not implemented by most drivers.
   *
   * @param[in] id the device's serial ID 0-255, or 256+ to broadcast to all
   * devices on the bus
   */
  virtual void SetDeviceID(unsigned int id) { static_cast<void>(id); }

  /**
   * @brief Get the target device ID. Not implemented by most drivers.
   *
   * @return The target device ID.
   */
  virtual unsigned int GetDeviceID() const { return 0; }

  /**
   * @brief Determines whether a driver is send-only in the average case.
   * UDP multicast is always send-only, and RS-485 is send-only depending on the
   * device ID.
   *
   * @return Whether the driver is send-only.
   */
  virtual bool IsSendOnly() const { return false; }
};

}  // namespace bci::abs::drivers

#endif /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMDRIVER_H */
