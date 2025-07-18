/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

/**
 * @file
 * @brief C-style interface to the library.
 */
#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup CInterface C Interface
 *
 * This interface provides a wrapper around the C++ library for use in C and
 * other languages.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup CErrors Error Codes
 * Error codes returned by the SCPI driver functions.
 * @{
 */
/// Success (no error)
#define ABS_SCPI_ERR_SUCCESS (0)
/// Channel index out of range
#define ABS_SCPI_ERR_CHANNEL_INDEX_OUT_OF_RANGE (-1)
/// Invalid IP address
#define ABS_SCPI_ERR_INVALID_IP_ADDRESS (-2)
/// Connection timed out
#define ABS_SCPI_ERR_CONNECTION_TIMED_OUT (-3)
/// Connection failed
#define ABS_SCPI_ERR_CONNECTION_FAILED (-4)
/// Failed to send message
#define ABS_SCPI_ERR_SEND_FAILED (-5)
/// Send timed out
#define ABS_SCPI_ERR_SEND_TIMED_OUT (-6)
/// Failed to read message
#define ABS_SCPI_ERR_READ_FAILED (-7)
/// Read timed out
#define ABS_SCPI_ERR_READ_TIMED_OUT (-8)
/// Not connected
#define ABS_SCPI_ERR_NOT_CONNECTED (-9)
/// Invalid response from the unit
#define ABS_SCPI_ERR_INVALID_RESPONSE (-10)
/// Invalid fault type
#define ABS_SCPI_ERR_INVALID_FAULT_TYPE (-11)
/// Invalid sense range
#define ABS_SCPI_ERR_INVALID_SENSE_RANGE (-12)
/// Invalid argument
#define ABS_SCPI_ERR_INVALID_ARGUMENT (-13)
/// Invalid driver handle
#define ABS_SCPI_ERR_INVALID_DRIVER_HANDLE (-14)
/// Receive not allowed by driver
#define ABS_SCPI_ERR_RECEIVE_NOT_ALLOWED (-15)
/// Already connected
#define ABS_SCPI_ERR_ALREADY_CONNECTED (-16)
/// Unexpected socket error
#define ABS_SCPI_ERR_SOCKET_ERROR (-17)
/// Failed to bind socket
#define ABS_SCPI_ERR_FAILED_TO_BIND_SOCKET (-18)
/// Failed to open serial port
#define ABS_SCPI_ERR_OPENING_SERIAL_PORT_FAILED (-19)
/// Failed to configure serial port
#define ABS_SCPI_ERR_FAILED_TO_CONFIGURE_PORT (-20)
/// Failed to join multicast group
#define ABS_SCPI_ERR_FAILED_TO_JOIN_GROUP (-21)
/// Buffer too small
#define ABS_SCPI_ERR_BUFFER_TOO_SMALL (-22)
/// Allocation failed
#define ABS_SCPI_ERR_ALLOCATION_FAILED (-23)
/// Unexpected exception
#define ABS_SCPI_ERR_UNEXPECTED_EXCEPTION (-24)
/** @} */

/**
 * @addtogroup CCells
 * @{
 */
/**
 * @defgroup CellFaults Cell Faults
 * Cell faulting states constants.
 * @{
 */
/// No fault
#define ABS_CELL_FAULT_NONE 0
/// Open circuit fault
#define ABS_CELL_FAULT_OPEN 1
/// Short circuit fault
#define ABS_CELL_FAULT_SHORT 2
/// Polarity fault
#define ABS_CELL_FAULT_POLARITY 3
/** @} */

/**
 * @defgroup SenseRanges Cell Current Sense Ranges
 * Cell sense range constants.
 * @{
 */
/// Automatic based on current limits (default)
#define ABS_CELL_SENSE_RANGE_AUTO 0
/// Low range (1A)
#define ABS_CELL_SENSE_RANGE_1A 1
/// High range (5A)
#define ABS_CELL_SENSE_RANGE_5A 2
/** @} */

/**
 * @defgroup CellModes Cell Operating Modes
 * Cell operating mode constants.
 * @{
 */
/// Constant voltage (normal)
#define ABS_CELL_MODE_CV 0
/// Current limited
#define ABS_CELL_MODE_ILIM 1
/** @} */
/** @} */

/// ABS SCPI client handle.
typedef void* AbsScpiClientHandle;

/**
 * @addtogroup CSystem
 * @{
 */
/// ABS device information structure. All strings are guaranteed to be
/// null-terminated.
typedef struct AbsDeviceInfo {
  char part_number[128];  ///< Part number.
  char serial[128];       ///< Serial number.
  char version[128];      ///< Version number.
} AbsDeviceInfo;

/// ABS Ethernet address configuration. All strings are guaranteed to be
/// null-terminated.
typedef struct AbsEthernetConfig {
  char ip[32];       ///< IP address.
  char netmask[32];  ///< Subnet mask.
} AbsEthernetConfig;
/** @} */

/**
 * @addtogroup CModeling
 * @{
 */
/// Model is running.
#define ABS_MODEL_STATUS_RUNNING (0x01)
/// Model is loaded.
#define ABS_MODEL_STATUS_LOADED (0x02)
/// Model has errored.
#define ABS_MODEL_STATUS_ERRORED (0x04)

/// Information about a model. Strings are null-terminated.
typedef struct AbsModelInfo {
  char name[256];     ///< Model name.
  char version[256];  ///< Model version.
} AbsModelInfo;
/** @} */

/**
 * @addtogroup CDisc
 * @{
 */
/// ABS Ethernet discovery result. All strings are guaranteed to be
/// null-terminated.
typedef struct AbsEthernetDiscoveryResult {
  char ip[32];       ///< IP address.
  char serial[128];  ///< Serial number.
} AbsEthernetDiscoveryResult;

/// ABS serial discovery result. All strings are guaranteed to be
/// null-terminated.
typedef struct AbsSerialDiscoveryResult {
  uint8_t id;        ///< IP address.
  char serial[128];  ///< Serial number.
} AbsSerialDiscoveryResult;
/** @} */

/**
 * @brief Get the version of the SCPI library as an unsigned base 10 integer.
 * For example, version 1.2.3 would return 10203.
 *
 * This is intended to be used to check for the existence of certain
 * functionality based on library version, particularly in wrappers (such as
 * Python).
 *
 * @since v1.1.0
 *
 * @return The library version.
 */
unsigned int AbsScpiClient_Version();

/**
 * @brief Get an error message to describe an error code returned by the driver.
 *
 * @return Null-terminated error message string.
 */
const char* AbsScpiClient_ErrorMessage(int error);

/**
 * @brief Initialize a SCPI client. Must be destroyed by the caller!
 *
 * @param[out] handle_out pointer to a handle to initialize (handle should be
 * zeroed)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_Init(AbsScpiClientHandle* handle_out);

/**
 * @brief Destroy a SCPI client.
 *
 * @param[in,out] handle pointer to a handle to destroy
 */
void AbsScpiClient_Destroy(AbsScpiClientHandle* handle);

/**
 * @brief Open a UDP connection to the ABS.
 *
 * @param[in] handle SCPI client
 * @param[in] target_ip ABS's IP address
 * @param[in] interface_ip address of the local interface to bind the socket to
 * (may be NULL or empty to use any local address)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_OpenUdp(AbsScpiClientHandle handle, const char* target_ip,
                          const char* interface_ip);

/**
 * @brief Open a TCP connection to the ABS.
 *
 * @note As a consequence of TCP's design, it is inherently less deterministic
 * than UDP. Commanding the ABS over TCP can be slower and less deterministic
 * than UDP, so for time-sensitive applications, it's recommended to avoid TCP
 * in favor of UDP.
 *
 * @param[in] handle SCPI client
 * @param[in] target_ip ABS's IP address
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_OpenTcp(AbsScpiClientHandle handle, const char* target_ip);

/**
 * @brief Open a serial connection to one or many ABSes.
 *
 * @param[in] handle SCPI client
 * @param[in] com_port the serial port to open, such as COM1 or /dev/ttyS0
 * @param[in] device_id target device ID, 0-31, or 32+ to broadcast to all
 * units on the bus
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_OpenSerial(AbsScpiClientHandle handle, const char* com_port,
                             unsigned int device_id);

/**
 * @brief Open a UDP multicast socket for broadcasting to many ABSes.
 *
 * @param[in] handle SCPI client
 * @param[in] interface_ip IP address of the local NIC to bind to
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_OpenUdpMulticast(AbsScpiClientHandle handle,
                                   const char* interface_ip);

/**
 * @brief Close the client connection.
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_Close(AbsScpiClientHandle handle);

/**
 * @brief Set the target device ID for communication. Only applies to RS-485
 * connections.
 *
 * @param[in] handle SCPI client
 * @param[in] device_id target device ID, 0-31, or 32+ to broadcast to all
 * units on the bus
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetTargetDeviceId(AbsScpiClientHandle handle,
                                    unsigned int device_id);

/**
 * @brief Get the target device ID for communication. Only relevant for RS-485
 * connections.
 *
 * @param[in] handle SCPI client
 * @param[out] device_id_out target device ID
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetTargetDeviceId(AbsScpiClientHandle handle,
                                    unsigned int* device_id_out);

/**
 * @defgroup CSystem System Control
 * Functions for controlling the system, such as querying device info and
 * updating the IP address.
 * @{
 */

/**
 * @brief Query basic information about the unit.
 *
 * @param[in] handle SCPI client
 * @param[out] info_out pointer to a device information structure to populate
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetDeviceInfo(AbsScpiClientHandle handle,
                                AbsDeviceInfo* info_out);

/**
 * @brief Query the ABS's device ID.
 *
 * @param[in] handle SCPI client
 * @param[out] id_out pointer to the resulting ID
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetDeviceId(AbsScpiClientHandle handle, uint8_t* id_out);

/**
 * @brief Query the device's IP address and subnet mask.
 *
 * @param[in] handle SCPI client
 * @param[out] addr_out pointer to an Ethernet configuration structure to
 * populate
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetIPAddress(AbsScpiClientHandle handle,
                               AbsEthernetConfig* addr_out);

/**
 * @brief Set the device's IP address and subnet mask.
 *
 * @note For UDP and TCP connections, after changing the IP, you should call the
 * corresponding Open*() function again to reconnect. Note that it may take
 * a moment for the unit to complete the change.
 *
 * @param[in] handle SCPI client
 * @param[in] addr desired Ethernet configuration
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetIPAddress(AbsScpiClientHandle handle,
                               const AbsEthernetConfig* addr);

/**
 * @brief Get the device's calibration date.
 *
 * @note The returned string will always be null-terminated. If this function
 * fails, the buffer will not be modified.
 *
 * @param[in] handle SCPI client
 * @param[out] buf pointer to the buffer into which a null-terminated string
 * will be written
 * @param[in] len the length of the buffer
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetCalibrationDate(AbsScpiClientHandle handle, char buf[],
                                     unsigned int len);

/**
 * @brief Query the number of error's in the device's error queue.
 *
 * @param[in] handle SCPI client
 * @param[out] count_out pointer to an integer to store the error count
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetErrorCount(AbsScpiClientHandle handle, int* count_out);

/**
 * @brief Query the next error from the device's error queue.
 *
 * @note The returned message will always be null-terminated. If this function
 * fails, the values of the outputs are undefined.
 *
 * @param[in] handle SCPI client
 * @param[out] err_code_out pointer to an integer to store the error code
 * @param[out] msg_buf pointer to a buffer to store the null-terminated error
 * message
 * @param[in] msg_buf_len length of the buffer
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetNextError(AbsScpiClientHandle handle,
                               int16_t* err_code_out, char msg_buf[],
                               unsigned int msg_buf_len);

/**
 * @brief Clear the device's error queue.
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_ClearErrors(AbsScpiClientHandle handle);

/**
 * @brief Query the alarms raised on the device.
 *
 * @param[in] handle SCPI client
 * @param[out] alarms_out pointer to an integer to store the bitmask of alarms
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAlarms(AbsScpiClientHandle handle, uint32_t* alarms_out);

/**
 * @brief Query the system interlock state. When in interlock, the unit will be
 * put into its PoR state and cannot be controlled until the interlock is
 * lifted.
 *
 * @param[in] handle SCPI client
 * @param[out] interlock_out pointer to the interlock state
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetInterlockState(AbsScpiClientHandle handle,
                                    bool* interlock_out);

/**
 * @brief Assert the software interlock (a recoverable alarm).
 *
 * The interlock may be cleared using the ClearRecoverableAlarms() function.
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_AssertSoftwareInterlock(AbsScpiClientHandle handle);

/**
 * @brief Clear recoverable alarms. This also clears a software interlock.
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_ClearRecoverableAlarms(AbsScpiClientHandle handle);

/**
 * @brief Reboot the device, resetting it to its POR state.
 *
 * Rebooting takes about 8-10 seconds. If using TCP, you'll need to re-open the
 * connection after the device has rebooted.
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_Reboot(AbsScpiClientHandle handle);

/** @} */

/**
 * @defgroup CCells Cell Control
 * Functions for controlling and measuring the cells.
 * @{
 */

/**
 * @brief Enable a single cell.
 *
 * @param[in] handle SCPI client
 * @param[in] cell the cell index 0-7
 * @param[in] en whether to enable the cell
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_EnableCell(AbsScpiClientHandle handle, unsigned int cell,
                             bool en);

/**
 * @brief Enable or disable multiple cells at once.
 *
 * @param[in] handle SCPI client
 * @param[in] cells a bitmask of cells to control, where bit 0 corresponds to
 * cell 1
 * @param[in] en the desired enable state of the masked cells
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_EnableCellsMasked(AbsScpiClientHandle handle,
                                    unsigned int cells, bool en);

/**
 * @brief Query whether a cell is enabled.
 *
 * @param[in] handle SCPI client
 * @param[in] cell cell index 0-7
 * @param[out] en_out pointer to the returned enable state
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetCellEnabled(AbsScpiClientHandle handle, unsigned int cell,
                                 bool* en_out);

/**
 * @brief Query the enable state of all cells.
 *
 * @param[in] handle SCPI client
 * @param[out] cells_out pointer to the returned bitmask containing a 1 bit for
 * each enabled cell and a 0 for each disabled cell
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetCellsEnabledMasked(AbsScpiClientHandle handle,
                                        unsigned int* cells_out);

/**
 * @brief Set a single cell's target voltage.
 *
 * @param[in] handle SCPI client
 * @param[in] cell target cell index, 0-7
 * @param[in] voltage cell voltage, 0-5
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetCellVoltage(AbsScpiClientHandle handle, unsigned int cell,
                                 float voltage);

/**
 * @brief Set many cells' target voltages.
 *
 * @param[in] handle SCPI client
 * @param[in] voltages array of voltages
 * @param[in] count length of the voltages array (must not be greater than the
 * total cell count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetAllCellVoltages(AbsScpiClientHandle handle,
                                     const float voltages[],
                                     unsigned int count);

/**
 * @brief Set multiple cells to the same voltage.
 *
 * @param[in] handle SCPI client
 * @param[in] cells bitmask of cells to target (one bit per cell)
 * @param[in] voltage target voltage
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetMultipleCellVoltages(AbsScpiClientHandle handle,
                                          unsigned int cells, float voltage);

/**
 * @brief Query a cell's target voltage.
 *
 * @param[in] handle SCPI client
 * @param[in] cell target cell index, 0-7
 * @param[out] voltage_out pointer to the returned cell voltage target
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetCellVoltageTarget(AbsScpiClientHandle handle,
                                       unsigned int cell, float* voltage_out);

/**
 * @brief Query many cells' target voltages.
 *
 * @param[in] handle SCPI client
 * @param[out] voltages_out an array to store the returned voltages
 * @param[in] count length of the voltages array (must not be greater than the
 * total cell count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllCellVoltageTargets(AbsScpiClientHandle handle,
                                           float voltages_out[],
                                           unsigned int count);

/**
 * @brief Set a single cell's current sourcing limit.
 *
 * @param[in] handle SCPI client
 * @param[in] cell cell index, 0-7
 * @param[in] limit desired sourcing limit, 0-5
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetCellSourcing(AbsScpiClientHandle handle, unsigned int cell,
                                  float limit);

/**
 * @brief Set many cells' sourcing limits.
 *
 * @param[in] handle SCPI client
 * @param[in] limits sourcing limits, one per cell
 * @param[in] count number of cells to target (must not be greater than the
 * total cell count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetAllCellSourcing(AbsScpiClientHandle handle,
                                     const float limits[], unsigned int count);

/**
 * @brief Set multiple cells to the same sourcing limit.
 *
 * @param[in] handle SCPI client
 * @param[in] cells bitmask of cells to target (one bit per cell)
 * @param[in] limit target sourcing limit, 0-5
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetMultipleCellSourcing(AbsScpiClientHandle handle,
                                          unsigned int cells, float limit);

/**
 * @brief Query a single cell's sourcing limit.
 *
 * @param[in] handle SCPI client
 * @param[in] cell target cell index, 0-7
 * @param[out] limit_out pointer to the returned sourcing limit
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetCellSourcingLimit(AbsScpiClientHandle handle,
                                       unsigned int cell, float* limit_out);

/**
 * @brief Query many cells' sourcing limits.
 *
 * @param[in] handle SCPI client
 * @param[out] limits_out array to store the returned limits
 * @param[in] count length of the array (must not be greater than the total cell
 * count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllCellSourcingLimits(AbsScpiClientHandle handle,
                                           float limits_out[],
                                           unsigned int count);

/**
 * @brief Set a single cell's current sinking limit.
 *
 * @param[in] handle SCPI client
 * @param[in] cell cell index, 0-7
 * @param[in] limit desired sinking limit, 0-5 (may be positive or negative)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetCellSinking(AbsScpiClientHandle handle, unsigned int cell,
                                 float limit);

/**
 * @brief Set many cells' sinking limits.
 *
 * @param[in] handle SCPI client
 * @param[in] limits sinking limits, one per cell
 * @param[in] count number of cells to target (must not be greater than the
 * total cell count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetAllCellSinking(AbsScpiClientHandle handle,
                                    const float limits[], unsigned int count);

/**
 * @brief Set multiple cells to the same sinking limit.
 *
 * @param[in] handle SCPI client
 * @param[in] cells bitmask of cells to target (one bit per cell)
 * @param[in] limit target sinking limit, 0-5 (positive or negative)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetMultipleCellSinking(AbsScpiClientHandle handle,
                                         unsigned int cells, float limit);

/**
 * @brief Query a single cell's sinking limit.
 *
 * @param[in] handle SCPI client
 * @param[in] cell target cell index, 0-7
 * @param[out] limit_out pointer to the returned sinking limit
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetCellSinkingLimit(AbsScpiClientHandle handle,
                                      unsigned int cell, float* limit_out);

/**
 * @brief Query many cells' sinking limits.
 *
 * @param[in] handle SCPI client
 * @param[out] limits_out array to store the returned limits
 * @param[in] count length of the array (must not be greater than the total cell
 * count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllCellSinkingLimits(AbsScpiClientHandle handle,
                                          float limits_out[],
                                          unsigned int count);

/**
 * @brief Set a single cell's fault state.
 *
 * @param[in] handle SCPI client
 * @param[in] cell cell index, 0-7
 * @param[in] fault desired cell fault
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetCellFault(AbsScpiClientHandle handle, unsigned int cell,
                               int fault);

/**
 * @brief Set many cells' fault states
 *
 * @param[in] handle SCPI client
 * @param[in] faults fault states, one per cell
 * @param[in] count number of cells to target (must not be greater than the
 * total cell count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetAllCellFaults(AbsScpiClientHandle handle,
                                   const int faults[], unsigned int count);

/**
 * @brief Set multiple cells to the same fault state.
 *
 * @param[in] handle SCPI client
 * @param[in] cells bitmask of cells to target, one bit per cell
 * @param[in] fault target cell fault state
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetMultipleCellFaults(AbsScpiClientHandle handle,
                                        unsigned int cells, int fault);

/**
 * @brief Query a single cell's fault state.
 *
 * @param[in] handle SCPI client
 * @param[in] cell target cell index, 0-7
 * @param[out] fault_out pointer to the returned fault state
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetCellFault(AbsScpiClientHandle handle, unsigned int cell,
                               int* fault_out);

/**
 * @brief Query many cells' fault states.
 *
 * @param[in] handle SCPI client
 * @param[out] faults_out array to store the returned fault states
 * @param[in] count length of the array (must not be greater than the total cell
 * count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllCellFaults(AbsScpiClientHandle handle, int faults_out[],
                                   unsigned int count);

/**
 * @brief Set a single cell's current sense range.
 *
 * @note This is unnecessary for most applications. By default, the cell will
 * choose a sense range based on its current sourcing and sinking limits.
 *
 * @param[in] handle SCPI client
 * @param[in] cell cell index, 0-7
 * @param[in] range desired cell current sense range
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetCellSenseRange(AbsScpiClientHandle handle,
                                    unsigned int cell, int range);

/**
 * @brief Set many cells' current sense ranges.
 *
 * @note This is unnecessary for most applications. By default, the cell will
 * choose a sense range based on its current sourcing and sinking limits.
 *
 * @param[in] handle SCPI client
 * @param[in] ranges sense ranges, one per cell
 * @param[in] count number of cells to target (must not be greater than the
 * total cell count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetAllCellSenseRanges(AbsScpiClientHandle handle,
                                        const int ranges[], unsigned int count);

/**
 * @brief Query a single cell's current sense range.
 *
 * @param[in] handle SCPI client
 * @param[in] cell target cell index, 0-7
 * @param[out] range_out pointer to the returned sense range
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetCellSenseRange(AbsScpiClientHandle handle,
                                    unsigned int cell, int* range_out);

/**
 * @brief Query many cells' current sense ranges.
 *
 * @param[in] handle SCPI client
 * @param[out] ranges_out array to store the returned sense ranges
 * @param[in] count length of the array (must not be greater than the total cell
 * count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllCellSenseRanges(AbsScpiClientHandle handle,
                                        int ranges_out[], unsigned int count);

/**
 * @brief Enable or disable the cell 50/60Hz noise filter.
 *
 * This mode filters 50/60Hz noise and increases cell measurement accuracy, but
 * reduces the cell control rate to 10Hz.
 *
 * @param[in] handle SCPI client
 * @param[in] en desired cell noise filter state
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_EnableCellNoiseFilter(AbsScpiClientHandle handle, bool en);

/**
 * @brief Query the enable state of the cell 50/60Hz noise filter.
 *
 * @param[in] handle SCPI client
 * @param[out] en_out a pointer to the returned filter state
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetCellNoiseFilterEnabled(AbsScpiClientHandle handle,
                                            bool* en_out);

/**
 * @brief Measure a cell's voltage.
 *
 * @param[in] handle SCPI client
 * @param[in] cell target cell index, 0-7
 * @param[out] voltage_out pointer to the returned voltage
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureCellVoltage(AbsScpiClientHandle handle,
                                     unsigned int cell, float* voltage_out);

/**
 * @brief Measure many cells' voltages.
 *
 * @param[in] handle SCPI client
 * @param[out] voltages_out array to store the returned voltages, one per cell
 * @param[in] count the number of cells to measure
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureAllCellVoltages(AbsScpiClientHandle handle,
                                         float voltages_out[],
                                         unsigned int count);

/**
 * @brief Measure a cell's current.
 *
 * @param[in] handle SCPI client
 * @param[in] cell target cell index, 0-7
 * @param[out] current_out pointer to the returned current
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureCellCurrent(AbsScpiClientHandle handle,
                                     unsigned int cell, float* current_out);

/**
 * @brief Measure many cells' currents.
 *
 * @param[in] handle SCPI client
 * @param[out] currents_out array to store the returned currents, one per cell
 * @param[in] count the number of cells to measure
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureAllCellCurrents(AbsScpiClientHandle handle,
                                         float currents_out[],
                                         unsigned int count);

/**
 * @brief Retrieve the rolling average of the last 10 voltage measurements for
 * a single cell.
 *
 * At the default sample rate, this is a 10ms window. With filtering on, the
 * length of this window will change.
 *
 * @since v1.1.0
 *
 * @par Requires
 * Firmware v1.2.0
 *
 * @param[in] handle SCPI client
 * @param[in] cell target cell index, 0-7
 * @param[out] voltage_out pointer to the returned average voltage
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureAverageCellVoltage(AbsScpiClientHandle handle,
                                            unsigned int cell,
                                            float* voltage_out);

/**
 * @brief Retrieve the rolling average of the last 10 voltage measurements for
 * many cells.
 *
 * At the default sample rate, this is a 10ms window. With filtering on, the
 * length of this window will change.
 *
 * @since v1.1.0
 *
 * @par Requires
 * Firmware v1.2.0
 *
 * @param[in] handle SCPI client
 * @param[out] voltages_out array to store the returned average voltages, one
 * per cell
 * @param[in] count the number of cells to measure
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureAllAverageCellVoltages(AbsScpiClientHandle handle,
                                                float voltages_out[],
                                                unsigned int count);

/**
 * @brief Retrieve the rolling average of the last 10 current measurements for
 * a single cell.
 *
 * At the default sample rate, this is a 10ms window. With filtering on, the
 * length of this window will change.
 *
 * @since v1.1.0
 *
 * @par Requires
 * Firmware v1.2.0
 *
 * @param[in] handle SCPI client
 * @param[in] cell target cell index, 0-7
 * @param[out] current_out pointer to the returned average current
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureAverageCellCurrent(AbsScpiClientHandle handle,
                                            unsigned int cell,
                                            float* current_out);

/**
 * @brief Retrieve the rolling average of the last 10 current measurements for
 * many cells.
 *
 * At the default sample rate, this is a 10ms window. With filtering on, the
 * length of this window will change.
 *
 * @since v1.1.0
 *
 * @par Requires
 * Firmware v1.2.0
 *
 * @param[in] handle SCPI client
 * @param[out] currents_out array to store the returned average currents, one
 * per cell
 * @param[in] count the number of cells to measure
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureAllAverageCellCurrents(AbsScpiClientHandle handle,
                                                float currents_out[],
                                                unsigned int count);

/**
 * @brief Query a cell's operating mode (constant voltage or current-limited).
 *
 * @param[in] handle SCPI client
 * @param[in] cell target cell index, 0-7
 * @param[out] mode_out pointer to returned operating mode
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetCellOperatingMode(AbsScpiClientHandle handle,
                                       unsigned int cell, int* mode_out);

/**
 * @brief Query many cells' operating modes (constant voltage or
 * current-limited).
 *
 * @param[in] handle SCPI client
 * @param[out] modes_out array to store the returned modes, one per cell
 * @param[in] count number of cells to query (must not be greater than the total
 * number of cells)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllCellOperatingModes(AbsScpiClientHandle handle,
                                           int modes_out[], unsigned int count);

/** @} */

/**
 * @defgroup CAux Aux IO Control
 * Functions for controlling auxiliary analog and digital IO.
 * @{
 */

/**
 * @brief Set a single analog output's voltage.
 *
 * @param[in] handle SCPI client
 * @param[in] channel target channel index, 0-7
 * @param[in] voltage desired voltage
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetAnalogOutput(AbsScpiClientHandle handle,
                                  unsigned int channel, float voltage);

/**
 * @brief Set many analog outputs.
 *
 * @param[in] handle SCPI client
 * @param[in] voltages array of voltages, one per channel
 * @param[in] count number of channels to target (must not be greater than the
 * total channel count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetAllAnalogOutputs(AbsScpiClientHandle handle,
                                      const float voltages[],
                                      unsigned int count);

/**
 * @brief Set multiple analog outputs to the same value.
 *
 * @param[in] handle SCPI client
 * @param[in] channels bitmask of channels to target, one bit per channel
 * @param[in] voltage target output voltage
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetMultipleAnalogOutputs(AbsScpiClientHandle handle,
                                           unsigned int channels,
                                           float voltage);

/**
 * @brief Query on analog output's set point.
 *
 * @param[in] handle SCPI client
 * @param[in] channel target channel index, 0-7
 * @param[out] voltage_out pointer to store the returned voltage
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAnalogOutput(AbsScpiClientHandle handle,
                                  unsigned int channel, float* voltage_out);

/**
 * @brief Query many analog outputs' set points.
 *
 * @param[in] handle SCPI client
 * @param[out] voltages_out array to store the returned voltages, one per
 * channel
 * @param[in] count length of the array (must not be greater than the total
 * channel count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllAnalogOutputs(AbsScpiClientHandle handle,
                                      float voltages_out[], unsigned int count);

/**
 * @brief Set a single digital output.
 *
 * @param[in] handle SCPI client
 * @param[in] channel target channel index, 0-3
 * @param[in] level desired output level
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetDigitalOutput(AbsScpiClientHandle handle,
                                   unsigned int channel, bool level);

/**
 * @brief Set all digital outputs.
 *
 * @param[in] handle SCPI client
 * @param[in] levels_mask mask of desired output states, one bit per channel
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetAllDigitalOutputs(AbsScpiClientHandle handle,
                                       unsigned int levels_mask);

/**
 * @brief Query the state of a digital output.
 *
 * @param[in] handle SCPI client
 * @param[in] channel target channel index, 0-3
 * @param[out] level_out pointer to the returned output level
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetDigitalOutput(AbsScpiClientHandle handle,
                                   unsigned int channel, bool* level_out);

/**
 * @brief Query the states of all digital outputs.
 *
 * @param[in] handle SCPI client
 * @param[out] levels_out pointer to store the returned levels, one bit per
 * channel
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllDigitalOutputs(AbsScpiClientHandle handle,
                                       unsigned int* levels_out);

/**
 * @brief Measure a single analog input.
 *
 * @param[in] handle SCPI client
 * @param[in] channel target channel index, 0-7
 * @param[out] voltage_out pointer to the returned voltage
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureAnalogInput(AbsScpiClientHandle handle,
                                     unsigned int channel, float* voltage_out);

/**
 * @brief Measure many analog inputs.
 *
 * @param[in] handle SCPI client
 * @param[out] voltages_out array to store the returned voltages
 * @param[in] count length of the array (must not be greater than the total
 * channel count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureAllAnalogInputs(AbsScpiClientHandle handle,
                                         float voltages_out[],
                                         unsigned int count);

/**
 * @brief Measure a single digital input.
 *
 * @param[in] handle SCPI client
 * @param[in] channel target channel index, 0-3
 * @param[out] level_out pointer to the returned input state
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureDigitalInput(AbsScpiClientHandle handle,
                                      unsigned int channel, bool* level_out);

/**
 * @brief Measure all digital inputs.
 *
 * @param[in] handle SCPI client
 * @param[out] levels_out pointer to the returned levels, one bit per channel
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureAllDigitalInputs(AbsScpiClientHandle handle,
                                          unsigned int* levels_out);

/** @} */

/**
 * @defgroup CModeling Model Control
 * Model control functionality.
 * @{
 */

/**
 * @brief Query the model status.
 *
 * The ABS_MODEL_STATUS_* macros can be used to interpret the status.
 *
 * @param[in] handle SCPI client
 * @param[out] status_out pointer to the returned status bits
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetModelStatus(AbsScpiClientHandle handle,
                                 uint8_t* status_out);

/**
 * @brief Load the model configuration on the device.
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_LoadModel(AbsScpiClientHandle handle);

/**
 * @brief Start modeling.
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_StartModel(AbsScpiClientHandle handle);

/**
 * @brief Stop modeling.
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_StopModel(AbsScpiClientHandle handle);

/**
 * @brief Unload the model configuration.
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_UnloadModel(AbsScpiClientHandle handle);

/**
 * @brief Query information about the loaded model.
 *
 * @param[in] handle SCPI client
 * @param[out] model_info_out pointer to the returned model info
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetModelInfo(AbsScpiClientHandle handle,
                               AbsModelInfo* model_info_out);

/**
 * @brief Query the ID of the currently loaded model. This ID is user-defined
 * and is not used by the unit. It is intended for use by tools.
 *
 * @param[in] handle SCPI client
 * @param[out] id_buf buffer to store the null-terminated ID
 * @param[in] buf_len length of @a id_buf
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetModelId(AbsScpiClientHandle handle, char id_buf[],
                             unsigned int buf_len);

/**
 * @brief Set a single global model input. Particularly useful with multicast to
 * address multiple units at once.
 *
 * @param[in] handle SCPI client
 * @param[in] index input index, 0-7
 * @param[in] value desired input value
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetGlobalModelInput(AbsScpiClientHandle handle,
                                      unsigned int index, float value);

/**
 * @brief Set multiple global model inputs. Particularly useful with multicast
 * to address multiple units at once.
 *
 * @param[in] handle SCPI client
 * @param[in] values array of values, one per input
 * @param[in] count number of values to send (must not be greater than the total
 * input count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetAllGlobalModelInputs(AbsScpiClientHandle handle,
                                          const float values[],
                                          unsigned int count);

/**
 * @brief Query the value of a single global model input.
 *
 * @param[in] handle SCPI client
 * @param[in] index index of the input, 0-7
 * @param[out] value_out pointer to the returned value
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetGlobalModelInput(AbsScpiClientHandle handle,
                                      unsigned int index, float* value_out);

/**
 * @brief Query the values of multiple global model inputs.
 *
 * @param[in] handle SCPI client
 * @param[out] values_out array of returned values, one per input
 * @param[in] count length of the array (must not be longer than the total input
 * count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllGlobalModelInputs(AbsScpiClientHandle handle,
                                          float values_out[],
                                          unsigned int count);

/**
 * @brief Set a single local model input.
 *
 * @param[in] handle SCPI client
 * @param[in] index input index, 0-7
 * @param[in] value desired input value
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetLocalModelInput(AbsScpiClientHandle handle,
                                     unsigned int index, float value);

/**
 * @brief Set multiple local model inputs.
 *
 * @param[in] handle SCPI client
 * @param[in] values array of values, one per input
 * @param[in] count number of values to send (must not be greater than the total
 * input count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetAllLocalModelInputs(AbsScpiClientHandle handle,
                                         const float values[],
                                         unsigned int count);

/**
 * @brief Query the value of a single local model input.
 *
 * @param[in] handle SCPI client
 * @param[in] index index of the input, 0-7
 * @param[out] value_out pointer to the returned value
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetLocalModelInput(AbsScpiClientHandle handle,
                                     unsigned int index, float* value_out);

/**
 * @brief Query the values of multiple local model inputs.
 *
 * @param[in] handle SCPI client
 * @param[out] values_out array of returned values, one per input
 * @param[in] count length of the array (must not be longer than the total input
 * count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllLocalModelInputs(AbsScpiClientHandle handle,
                                         float values_out[],
                                         unsigned int count);

/**
 * @brief Query a single model output.
 *
 * @param[in] handle SCPI client
 * @param[in] index output index, 0-35
 * @param[out] value_out pointer to the returned model output
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetModelOutput(AbsScpiClientHandle handle, unsigned int index,
                                 float* value_out);

/**
 * @brief Query multiple model outputs.
 *
 * @param[in] handle SCPI client
 * @param[out] values_out array of returned outputs
 * @param[in] count length of the array (must not be greater than the total
 * model output count)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllModelOutputs(AbsScpiClientHandle handle,
                                     float values_out[], unsigned int count);

/** @} */

/**
 * @defgroup CDisc Device Discovery
 * Functions for discovering ABSes on the network.
 * @{
 */

/**
 * @brief Use UDP multicast to discover ABSes on the network.
 *
 * This function does not require that a ScpiClient be open or connected.
 *
 * @note If the array is not big enough, this function will fill in as many
 * results as possible and return a buffer too small error. The output is still
 * valid in this case.
 *
 * @param[in] interface_ip address of the local interface to bind to
 * @param[out] results_out array of Ethernet discovery results
 * @param[in,out] count pointer to the length of the array; will be set to the
 * actual number of units discovered
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MulticastDiscovery(const char* interface_ip,
                                     AbsEthernetDiscoveryResult results_out[],
                                     unsigned int* count);

/**
 * @brief Use RS-485 to discover ABSes on the bus.
 *
 * This function does not require that a ScpiClient be open or connected. In
 * fact, a ScpiClient may NOT be connected to the same serial port as is used
 * during the call to this function.
 *
 * To discover serial devices, this function simply iterates through serial IDs
 * and waits for a response from each device.
 *
 * @note If the array is not big enough, this function will fill in as many
 * results as possible and return a buffer too small error. The output is still
 * valid in this case.
 *
 * @param[in] port serial port to use, such as COM1 or /dev/ttyS0
 * @param[in] first_id first serial ID to check, 0-31
 * @param[in] last_id last serial ID to check (inclusive), 0-31 (must not be
 * less than @a first_id)
 * @param[out] results_out array of serial discovery results
 * @param[in,out] count pointer to the length of the array; will be set to the
 * actual number of units discovered
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SerialDiscovery(const char* port, uint8_t first_id,
                                  uint8_t last_id,
                                  AbsSerialDiscoveryResult results_out[],
                                  unsigned int* count);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H */
