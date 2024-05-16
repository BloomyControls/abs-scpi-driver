#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ABS_CELL_FAULT_NONE 0
#define ABS_CELL_FAULT_OPEN 1
#define ABS_CELL_FAULT_SHORT 2
#define ABS_CELL_FAULT_POLARITY 3

#define ABS_CELL_SENSE_RANGE_AUTO 0
#define ABS_CELL_SENSE_RANGE_1A 1
#define ABS_CELL_SENSE_RANGE_5A 2

#define ABS_CELL_MODE_CV 0
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
