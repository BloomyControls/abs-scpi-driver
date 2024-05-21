/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* SCPI Driver Error Codes */

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
/// Address resolution failed
#define ABS_SCPI_ERR_ADDRESS_RESOLUTION_FAILED (-10)
/// Invalid response from the unit
#define ABS_SCPI_ERR_INVALID_RESPONSE (-11)
/// Invalid fault type
#define ABS_SCPI_ERR_INVALID_FAULT_TYPE (-12)
/// Invalid sense range
#define ABS_SCPI_ERR_INVALID_SENSE_RANGE (-13)
/// Invalid argument
#define ABS_SCPI_ERR_INVALID_ARGUMENT (-14)
/// Invalid driver handle
#define ABS_SCPI_ERR_INVALID_DRIVER_HANDLE (-15)
/// Receive not allowed by driver
#define ABS_SCPI_ERR_RECEIVE_NOT_ALLOWED (-16)
/// Already connected
#define ABS_SCPI_ERR_ALREADY_CONNECTED (-17)
/// Unexpected socket error
#define ABS_SCPI_ERR_SOCKET_ERROR (-18)
/// Failed to bind socket
#define ABS_SCPI_ERR_FAILED_TO_BIND_SOCKET (-19)
/// Failed to open serial port
#define ABS_SCPI_ERR_OPENING_SERIAL_PORT_FAILED (-20)
/// Failed to configure serial port
#define ABS_SCPI_ERR_FAILED_TO_CONFIGURE_PORT (-21)
/// Failed to join multicast group
#define ABS_SCPI_ERR_FAILED_TO_JOIN_GROUP (-22)
/// Buffer too small
#define ABS_SCPI_ERR_BUFFER_TOO_SMALL (-23)
/// Invalid precision mode
#define ABS_SCPI_ERR_INVALID_PRECISION_MODE (-24)
/// Allocation failed
#define ABS_SCPI_ERR_ALLOCATION_FAILED (-25)
/// Unexpected exception
#define ABS_SCPI_ERR_UNEXPECTED_EXCEPTION (-26)

/* Cell Indices and Bit Masks */

#define ABS_CELL_1 0
#define ABS_CELL_2 1
#define ABS_CELL_3 2
#define ABS_CELL_4 3
#define ABS_CELL_5 4
#define ABS_CELL_6 5
#define ABS_CELL_7 6
#define ABS_CELL_8 7

#define ABS_CELL_1_MASK (1U << 0)
#define ABS_CELL_2_MASK (1U << 1)
#define ABS_CELL_3_MASK (1U << 2)
#define ABS_CELL_4_MASK (1U << 3)
#define ABS_CELL_5_MASK (1U << 4)
#define ABS_CELL_6_MASK (1U << 5)
#define ABS_CELL_7_MASK (1U << 6)
#define ABS_CELL_8_MASK (1U << 7)

/* Cell Fault Modes */

/// No fault
#define ABS_CELL_FAULT_NONE 0
/// Open circuit fault
#define ABS_CELL_FAULT_OPEN 1
/// Short circuit fault
#define ABS_CELL_FAULT_SHORT 2
/// Polarity fault
#define ABS_CELL_FAULT_POLARITY 3

/* Cell Current Sense Ranges */

/// Automatic based on current limits (default)
#define ABS_CELL_SENSE_RANGE_AUTO 0
/// Low range (1A)
#define ABS_CELL_SENSE_RANGE_1A 1
/// High range (5A)
#define ABS_CELL_SENSE_RANGE_5A 2

/* Cell Precision Modes */

/// Normal precision mode (high speed)
#define ABS_CELL_PREC_NORMAL 0
/// High precision mode (low speed)
#define ABS_CELL_PREC_HIGH 1
/// Noise-rejection mode (low speed)
#define ABS_CELL_PREC_FILTER 2

/* Cell Operating Modes */

/// Constant voltage (normal)
#define ABS_CELL_MODE_CV 0
/// Current limited
#define ABS_CELL_MODE_ILIM 1

/// ABS SCPI client handle.
typedef void* AbsScpiClientHandle;

/// ABS device information structure. All strings are guaranteed to be
/// null-terminated.
typedef struct AbsDeviceInfo {
  char part_number[128];  ///< Part number.
  char serial[128];       ///< Serial number.
  char version[128];      ///< Version number.
} AbsDeviceInfo;

/// ABS Ethernet address configuration.
typedef struct AbsEthernetConfig {
  char ip[32];       ///< IP address.
  char netmask[32];  ///< Subnet mask.
} AbsEthernetConfig;

/// ABS Ethernet discovery result.
typedef struct AbsEthernetDiscoveryResult {
  char ip[32];       ///< IP address.
  char serial[128];  ///< Serial number.
} AbsEthernetDiscoveryResult;

/// ABS serial discovery result.
typedef struct AbsSerialDiscoveryResult {
  uint8_t id;        ///< IP address.
  char serial[128];  ///< Serial number.
} AbsSerialDiscoveryResult;

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
 * (may be NULL to use any local address)
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_OpenUdp(AbsScpiClientHandle handle, const char* target_ip,
                          const char* interface_ip);

/**
 * @brief Open a TCP connection to the ABS.
 *
 * @note As a consequence of TCP's design, it is inherently less deterministic
 * than UDP. Commanding the ABS over TCP can be much slower and less
 * deterministic than UDP, so for time-sensitive applications, it's recommended
 * to avoid TCP in favor of UDP.
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
 * @param[in] device_id target device ID, 0-255, or 256+ to broadcast to all
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
 * @brief Set the target device ID for communication. Only applies to RS-485
 * connections.
 *
 * @param[in] handle SCPI client
 * @param[in] device_id target device ID, 0-255, or 256+ to broadcast to all
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
 * @brief Set the cell precision mode.
 *
 * The device defaults to normal precision (high speed) mode.
 *
 * @param[in] handle SCPI client
 * @param[in] mode desired cell precision mode
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetCellPrecisionMode(AbsScpiClientHandle handle, int mode);

/**
 * @brief Query the cell precision mode.
 *
 * @param[in] handle SCPI client
 * @param[out] mode_out a pointer to the returned precision mode
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetCellPrecisionMode(AbsScpiClientHandle handle,
                                       int* mode_out);

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
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetCellOperatingMode(AbsScpiClientHandle handle,
                                       unsigned int cell, int* mode_out);

/**
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllCellOperatingModes(AbsScpiClientHandle handle,
                                           int modes_out[], unsigned int count);

/**
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetAnalogOutput(AbsScpiClientHandle handle,
                                  unsigned int channel, float voltage);

/**
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetAllAnalogOutputs(AbsScpiClientHandle handle,
                                      const float voltages[],
                                      unsigned int count);

/**
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAnalogOutput(AbsScpiClientHandle handle,
                                  unsigned int channel, float* voltage_out);

/**
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllAnalogOutputs(AbsScpiClientHandle handle,
                                      float voltages_out[], unsigned int count);

/**
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetDigitalOutput(AbsScpiClientHandle handle,
                                   unsigned int channel, bool level);

/**
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_SetAllDigitalOutputs(AbsScpiClientHandle handle,
                                       unsigned int levels_mask);

/**
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetDigitalOutput(AbsScpiClientHandle handle,
                                   unsigned int channel, bool* level_out);

/**
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_GetAllDigitalOutputs(AbsScpiClientHandle handle,
                                       unsigned int* levels_out);

/**
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureAnalogInput(AbsScpiClientHandle handle,
                                     unsigned int channel, float* voltage_out);

/**
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureAllAnalogInputs(AbsScpiClientHandle handle,
                                         float voltages_out[],
                                         unsigned int count);

/**
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureDigitalInput(AbsScpiClientHandle handle,
                                      unsigned int channel, bool* level_out);

/**
 * @brief 
 *
 * @param[in] handle SCPI client
 *
 * @return 0 on success or a negative error code.
 */
int AbsScpiClient_MeasureAllDigitalInputs(AbsScpiClientHandle handle,
                                          unsigned int* levels_out);

// count is both an input and an output!
// if results_out is not big enough, fills as much as possible and returns
// a buffer too small error
int AbsScpiClient_MulticastDiscovery(const char* interface_ip,
                                     AbsEthernetDiscoveryResult results_out[],
                                     unsigned int* count);

// count is both an input and an output!
// if results_out is not big enough, fills as much as possible and returns
// a buffer too small error
int AbsScpiClient_SerialDiscovery(const char* port, uint8_t first_id,
                                  uint8_t last_id,
                                  AbsSerialDiscoveryResult results_out[],
                                  unsigned int* count);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H */
