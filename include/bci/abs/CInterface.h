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

int AbsScpiClient_GetIPAddress(AbsScpiClientHandle handle,
                               AbsEthernetConfig* addr_out);

int AbsScpiClient_SetIPAddress(AbsScpiClientHandle handle,
                               const AbsEthernetConfig* addr);

// buffer is not modified on error
int AbsScpiClient_GetCalibrationDate(AbsScpiClientHandle handle, char* buf,
                                     unsigned int len);

int AbsScpiClient_GetErrorCount(AbsScpiClientHandle handle, int* count_out);

int AbsScpiClient_GetNextError(AbsScpiClientHandle handle,
                               int16_t* err_code_out, char* msg_buf,
                               unsigned int msg_buf_len);

int AbsScpiClient_ClearErrors(AbsScpiClientHandle handle);

int AbsScpiClient_GetAlarms(AbsScpiClientHandle handle, uint32_t* alarms_out);

int AbsScpiClient_AssertSoftwareInterlock(AbsScpiClientHandle handle);

int AbsScpiClient_ClearRecoverableAlarms(AbsScpiClientHandle handle);

int AbsScpiClient_Reboot(AbsScpiClientHandle handle);

int AbsScpiClient_EnableCell(AbsScpiClientHandle handle, unsigned int cell,
                             bool en);

int AbsScpiClient_EnableCellsMasked(AbsScpiClientHandle handle,
                                    unsigned int cells, bool en);

int AbsScpiClient_GetCellEnabled(AbsScpiClientHandle handle, unsigned int cell,
                                 bool* en_out);

int AbsScpiClient_GetCellsEnabledMasked(AbsScpiClientHandle handle,
                                        unsigned int* cells_out);

int AbsScpiClient_SetCellVoltage(AbsScpiClientHandle handle, unsigned int cell,
                                 float voltage);

int AbsScpiClient_SetAllCellVoltage(AbsScpiClientHandle handle,
                                    const float* voltages, unsigned int count);

int AbsScpiClient_GetCellVoltageTarget(AbsScpiClientHandle handle,
                                       unsigned int cell, float* voltage_out);

int AbsScpiClient_GetAllCellVoltageTarget(AbsScpiClientHandle handle,
                                          float* voltages_out,
                                          unsigned int count);

int AbsScpiClient_SetCellSourcing(AbsScpiClientHandle handle, unsigned int cell,
                                  float limit);

int AbsScpiClient_SetAllCellSourcing(AbsScpiClientHandle handle,
                                     const float* limits, unsigned int count);

int AbsScpiClient_GetCellSourcingLimit(AbsScpiClientHandle handle,
                                       unsigned int cell, float* limit_out);

int AbsScpiClient_GetAllCellSourcingLimit(AbsScpiClientHandle handle,
                                          float* limits_out,
                                          unsigned int count);

int AbsScpiClient_SetCellSinking(AbsScpiClientHandle handle, unsigned int cell,
                                 float limit);

int AbsScpiClient_SetAllCellSinking(AbsScpiClientHandle handle,
                                    const float* limits, unsigned int count);

int AbsScpiClient_GetCellSinkingLimit(AbsScpiClientHandle handle,
                                      unsigned int cell, float* limit_out);

int AbsScpiClient_GetAllCellSinkingLimit(AbsScpiClientHandle handle,
                                         float* limits_out, unsigned int count);

int AbsScpiClient_SetCellFault(AbsScpiClientHandle handle, unsigned int cell,
                               int fault);

int AbsScpiClient_SetAllCellFault(AbsScpiClientHandle handle, const int* faults,
                                  unsigned int count);

int AbsScpiClient_GetCellFault(AbsScpiClientHandle handle, unsigned int cell,
                               int* fault_out);

int AbsScpiClient_GetAllCellFault(AbsScpiClientHandle handle, int* faults_out,
                                  unsigned int count);

int AbsScpiClient_SetCellSenseRange(AbsScpiClientHandle handle,
                                    unsigned int cell, int range);

int AbsScpiClient_SetAllCellSenseRange(AbsScpiClientHandle handle,
                                       const int* ranges, unsigned int count);

int AbsScpiClient_GetCellSenseRange(AbsScpiClientHandle handle,
                                    unsigned int cell, int* range_out);

int AbsScpiClient_GetAllCellSenseRange(AbsScpiClientHandle handle,
                                       int* ranges_out, unsigned int count);

int AbsScpiClient_SetCellPrecisionMode(AbsScpiClientHandle handle, int mode);

int AbsScpiClient_GetCellPrecisionMode(AbsScpiClientHandle handle,
                                       int* mode_out);

int AbsScpiClient_MeasureCellVoltage(AbsScpiClientHandle handle,
                                     unsigned int cell, float* voltage_out);

int AbsScpiClient_MeasureAllCellVoltage(AbsScpiClientHandle handle,
                                        float* voltages_out,
                                        unsigned int count);

int AbsScpiClient_MeasureCellCurrent(AbsScpiClientHandle handle,
                                     unsigned int cell, float* current_out);

int AbsScpiClient_MeasureAllCellCurrent(AbsScpiClientHandle handle,
                                        float* currents_out,
                                        unsigned int count);

int AbsScpiClient_GetCellOperatingMode(AbsScpiClientHandle handle,
                                       unsigned int cell, int* mode_out);

int AbsScpiClient_GetAllCellOperatingMode(AbsScpiClientHandle handle,
                                          int* modes_out, unsigned int count);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H */
