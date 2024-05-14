#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMONTYPES_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMONTYPES_H

#include <cstdint>
#include <string>

#include "util/expected.hpp"

namespace bci::abs {

/// Total cell count.
inline constexpr unsigned int kCellCount = 8U;
/// Total analog input count.
inline constexpr unsigned int kAnalogInputCount = 8U;
/// Total analog output count.
inline constexpr unsigned int kAnalogOutputCount = 8U;
/// Total digital input count.
inline constexpr unsigned int kDigitalInputCount = 4U;
/// Total digital output count.
inline constexpr unsigned int kDigitalOutputCount = 4U;

/// Cell fault states.
enum class CellFault {
  kNone,          ///< No fault
  kOpenCircuit,   ///< Open circuit
  kShortCircuit,  ///< Short circuit
  kPolarity,      ///< Polarity fault
};

/// Cell current sense ranges. Defaults to auto at power up.
enum class CellSenseRange {
  kAuto,  ///< Automatic based on current limit (default)
  kLow,   ///< Low range (1A)
  kHigh,  ///< High range (5A)
};

/// Cell operating modes.
enum class CellMode {
  kConstantVoltage,  ///< Constant voltage (normal)
  kCurrentLimited,   ///< Current limited
};

/// General device info.
struct DeviceInfo {
  std::string part_number;  ///< Part number
  std::string serial;       ///< Serial number
  std::string version;      ///< Firmware version
};

/// Ethernet configuration structure.
struct EthernetConfig {
  std::string ip;       ///< IP address
  std::string netmask;  ///< Subnet mask
};

/// SCPI error read from device error queue.
struct ScpiError {
  std::int16_t err_code;  ///< Error code
  std::string err_msg;    ///< Error message
};

/// Bits and masks for interpreting alarms.
namespace alarms {

/// Recoverable alarms mask.
inline constexpr std::uint32_t kRecoverableMask = 0x0000FF;
/// Critical alarms mask.
inline constexpr std::uint32_t kCriticalMask = 0x00FF00;
/// Fatal alarms mask.
inline constexpr std::uint32_t kFatalMask = 0xFF0000;

/* Recoverable */

/// Software interlock.
inline constexpr std::uint32_t kSoftInterlock = 0x000001;

/* Critical */

/// Fan 0 fault. Cells 1 and 2 are disabled.
inline constexpr std::uint32_t kFan0Fault = 0x000100;
/// Fan 1 fault. Cells 3 and 4 are disabled.
inline constexpr std::uint32_t kFan1Fault = 0x000200;
/// Fan 2 fault. Cells 5 and 6 are disabled.
inline constexpr std::uint32_t kFan2Fault = 0x000400;
/// Fan 3 fault. Cells 7 and 8 are disabled.
inline constexpr std::uint32_t kFan3Fault = 0x000800;

/* Fatal */

/// Fan 4 fault. All cells are disabled.
inline constexpr std::uint32_t kFan4Fault = 0x010000;
/// Hardware fault. All cells are disabled.
inline constexpr std::uint32_t kHardwareFault = 0x020000;

}  // namespace alarms

/// Error codes returned by driver functions.
enum class ErrorCode : int {
  kSuccess = 0,                    ///< Success (no error)
  kChannelIndexOutOfRange = -1,    ///< Channel index out of range
  kInvalidIPAddress = -2,          ///< Invalid IP address
  kConnectionTimedOut = -3,        ///< Connection timed out
  kConnectionFailed = -4,          ///< Connection failed
  kSendFailed = -5,                ///< Failed to send message
  kSendTimedOut = -6,              ///< Send timed out
  kReadFailed = -7,                ///< Failed to read message
  kReadTimedOut = -8,              ///< Read timed out
  kNotConnected = -9,              ///< Not connected
  kAddressResolutionFailed = -10,  ///< Address resolution failed
  kInvalidResponse = -11,          ///< Invalid response from the unit
  kInvalidFaultType = -12,         ///< Invalid fault type
  kInvalidSenseRange = -13,        ///< Invalid sense range
  kInvalidArgument = -14,          ///< Invalid argument
  kInvalidDriverHandle = -15,      ///< Invalid driver handle
  kReceiveNotAllowed = -16,        ///< Receive not allowed by driver
  kAlreadyConnected = -17,         ///< Already connected
  kSocketError = -18,              ///< Unexpected socket error
  kFailedToBindSocket = -19,       ///< Failed to bind socket
  kOpeningSerialPortFailed = -20,  ///< Failed to open serial port
  kFailedToConfigurePort = -21,    ///< Failed to configure serial port
  kFailedToJoinGroup = -22,        ///< Failed to join multicast group
};

/**
 * @brief Result type used to return values or error codes from driver
 * functions.
 *
 * `tl::expected` models C++23's `std::expected` and adds some other useful
 * functions as well.
 *
 * @tparam T type of the "expected" response
 */
template <class T>
using Result = tl::expected<T, ErrorCode>;

/**
 * @brief Get an error message string for a given error code.
 *
 * @param[in] ec error code
 *
 * @return Error message.
 */
const char* ErrorMessage(ErrorCode ec) noexcept;

}  // namespace bci::abs

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMONTYPES_H */
