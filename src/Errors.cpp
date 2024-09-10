/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include <bci/abs/CommonTypes.h>

namespace bci::abs {

const char* ErrorMessage(ErrorCode ec) noexcept {
  switch (ec) {
    case ErrorCode::kSuccess:
      return "Success";
    case ErrorCode::kChannelIndexOutOfRange:
      return "Channel index out of range";
    case ErrorCode::kInvalidIPAddress:
      return "Invalid IP address";
    case ErrorCode::kConnectionTimedOut:
      return "Connection timed out";
    case ErrorCode::kConnectionFailed:
      return "Connection failed";
    case ErrorCode::kSendFailed:
      return "Send failed";
    case ErrorCode::kSendTimedOut:
      return "Send timed out";
    case ErrorCode::kReadFailed:
      return "Read failed";
    case ErrorCode::kReadTimedOut:
      return "Read timed out";
    case ErrorCode::kNotConnected:
      return "Not connected";
    case ErrorCode::kInvalidResponse:
      return "Invalid response received from the unit";
    case ErrorCode::kInvalidFaultType:
      return "Invalid fault type";
    case ErrorCode::kInvalidSenseRange:
      return "Invalid sense range";
    case ErrorCode::kInvalidArgument:
      return "Invalid argument";
    case ErrorCode::kInvalidDriverHandle:
      return "Invalid driver handle";
    case ErrorCode::kReceiveNotAllowed:
      return "Receiving not supported by driver";
    case ErrorCode::kAlreadyConnected:
      return "Already connected";
    case ErrorCode::kSocketError:
      return "Unexpected socket error";
    case ErrorCode::kFailedToBindSocket:
      return "Failed to bind socket";
    case ErrorCode::kOpeningSerialPortFailed:
      return "Failed to open serial port";
    case ErrorCode::kFailedToConfigurePort:
      return "Failed to configure port";
    case ErrorCode::kFailedToJoinGroup:
      return "Failed to join multicast group";
    case ErrorCode::kBufferTooSmall:
      return "Insufficient buffer size";
    case ErrorCode::kAllocationFailed:
      return "Allocation failed";
    case ErrorCode::kUnexpectedException:
      return "Unexpected exception";
  }

  return "Unknown error";
}

}  // namespace bci::abs
