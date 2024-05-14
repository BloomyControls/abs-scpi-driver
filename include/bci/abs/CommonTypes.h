#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMONTYPES_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMONTYPES_H

#include <cstdint>
#include <string>

#include "util/expected.hpp"

namespace bci::abs {

inline constexpr unsigned int kCellCount = 8U;
inline constexpr unsigned int kAnalogInputCount = 8U;
inline constexpr unsigned int kAnalogOutputCount = 8U;
inline constexpr unsigned int kDigitalInputCount = 4U;
inline constexpr unsigned int kDigitalOutputCount = 4U;

enum class CellFault {
  kNone,
  kOpenCircuit,
  kShortCircuit,
  kPolarity,
};

enum class CellSenseRange {
  kAuto,
  kLow,
  kHigh,
};

enum class CellMode {
  kConstantVoltage,
  kCurrentLimited,
};

struct DeviceInfo {
  std::string part_number;
  std::string serial;
  std::string version;
};

struct EthernetConfig {
  std::string ip;
  std::string netmask;
};

struct ScpiError {
  std::int16_t err_code;
  std::string err_msg;
};

namespace alarms {

inline constexpr std::uint32_t kRecoverableMask = 0x0000FF;
inline constexpr std::uint32_t kCriticalMask = 0x00FF00;
inline constexpr std::uint32_t kFatalMask = 0xFF0000;

/* Recoverable */
inline constexpr std::uint32_t kSoftInterlock = 0x000001;

/* Critical */
inline constexpr std::uint32_t kFan0Fault = 0x000100;
inline constexpr std::uint32_t kFan1Fault = 0x000200;
inline constexpr std::uint32_t kFan2Fault = 0x000400;
inline constexpr std::uint32_t kFan3Fault = 0x000800;

/* Fatal */
inline constexpr std::uint32_t kFan4Fault = 0x010000;
inline constexpr std::uint32_t kHardwareFault = 0x020000;

}  // namespace alarms


enum class ErrorCode : int {
  kSuccess = 0,
  kChannelIndexOutOfRange = -1,
  kInvalidIPAddress = -2,
  kConnectionTimedOut = -3,
  kConnectionFailed = -4,
  kSendFailed = -5,
  kSendTimedOut = -6,
  kReadFailed = -7,
  kReadTimedOut = -8,
  kNotConnected = -9,
  kAddressResolutionFailed = -10,
  kInvalidResponse = -11,
  kInvalidFaultType = -12,
  kInvalidSenseRange = -13,
  kInvalidArgument = -14,
  kInvalidDriverHandle = -15,
  kReceiveNotAllowed = -16,
  kAlreadyConnected = -17,
  kSocketError = -18,
  kFailedToBindSocket = -19,
  kOpeningSerialPortFailed = -20,
  kFailedToConfigurePort = -21,
  kFailedToJoinGroup = -22,
};

template <class T>
using Result = tl::expected<T, ErrorCode>;

const char* ErrorMessage(ErrorCode ec) noexcept;

}  // namespace bci::abs

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMONTYPES_H */
