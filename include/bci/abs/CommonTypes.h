/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

/**
 * @file
 * @brief Defines common types for use with the SCPI client.
 */
#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMONTYPES_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMONTYPES_H

#include <cstdint>
#include <string>
#include <utility>

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
/// Total global model input count.
inline constexpr unsigned int kGlobalModelInputCount = 8U;
/// Total local model input count.
inline constexpr unsigned int kLocalModelInputCount = 8U;
/// Total model output count.
inline constexpr unsigned int kModelOutputCount = 36U;

/// Cell fault states.
enum class CellFault : int {
  kNone,          ///< No fault
  kOpenCircuit,   ///< Open circuit
  kShortCircuit,  ///< Short circuit
  kPolarity,      ///< Polarity fault
};

/// Cell current sense ranges. Defaults to auto at power up.
enum class CellSenseRange : int {
  kAuto,  ///< Automatic based on current limit (default)
  kLow,   ///< Low range (1A)
  kHigh,  ///< High range (5A)
};

/// Cell operating modes.
enum class CellMode : int {
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

/// Information about a model.
struct ModelInfo {
  std::string name;     ///< Name of the model.
  std::string version;  ///< Version of the model.
};

/// Bits for interpreting the model status.
namespace model_status {

/// The model is running.
inline constexpr std::uint8_t kRunning = 0x01;

/// The model is loaded.
inline constexpr std::uint8_t kLoaded = 0x02;

/// The model has errored.
inline constexpr std::uint8_t kErrored = 0x04;

}  // namespace model_status

/// Error codes returned by driver functions.
/// @note These values must match the macros in CInterface.h!
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
  kInvalidResponse = -10,          ///< Invalid response from the unit
  kInvalidFaultType = -11,         ///< Invalid fault type
  kInvalidSenseRange = -12,        ///< Invalid sense range
  kInvalidArgument = -13,          ///< Invalid argument
  kInvalidDriverHandle = -14,      ///< Invalid driver handle
  kReceiveNotAllowed = -15,        ///< Receive not allowed by driver
  kAlreadyConnected = -16,         ///< Already connected
  kSocketError = -17,              ///< Unexpected socket error
  kFailedToBindSocket = -18,       ///< Failed to bind socket
  kOpeningSerialPortFailed = -19,  ///< Failed to open serial port
  kFailedToConfigurePort = -20,    ///< Failed to configure serial port
  kFailedToJoinGroup = -21,        ///< Failed to join multicast group
  kBufferTooSmall = -22,           ///< Buffer too small
  kAllocationFailed = -23,         ///< Allocation failed (C only)
  kUnexpectedException = -24,      ///< Unexpected exception (C only)
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

#endif /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_COMMONTYPES_H */
