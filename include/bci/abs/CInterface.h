#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define ABS_CELL_FAULT_NONE 0
#define ABS_CELL_FAULT_OPEN 1
#define ABS_CELL_FAULT_SHORT 2
#define ABS_CELL_FAULT_POLARITY 3

#define ABS_CELL_SENSE_RANGE_AUTO 0
#define ABS_CELL_SENSE_RANGE_1A 1
#define ABS_CELL_SENSE_RANGE_5A 2

typedef void* AbsScpiClientHandle;

int AbsScpiClient_Init(AbsScpiClientHandle* handle_out);

void AbsScpiClient_Destroy(AbsScpiClientHandle* handle);

int ScpiClient_OpenUdp(AbsScpiClientHandle handle, const char* local_ip,
                       const char* target_ip);

int ScpiClient_GetDeviceId(AbsScpiClientHandle handle, uint8_t* id_out);

int ScpiClient_EnableCell(AbsScpiClientHandle handle, unsigned int cell,
                          bool en);

int ScpiClient_EnableCellsMasked(AbsScpiClientHandle handle, unsigned int cells,
                                 bool en);

int ScpiClient_GetCellEnabled(AbsScpiClientHandle handle, unsigned int cell,
                              bool* en_out);

int ScpiClient_GetCellsEnabledMasked(AbsScpiClientHandle handle,
                                     unsigned int* cells_out);

int ScpiClient_SetCellVoltage(AbsScpiClientHandle handle, unsigned int cell,
                              float voltage);

int ScpiClient_SetAllCellVoltage(AbsScpiClientHandle handle,
                                 const float* voltages, unsigned int count);

int ScpiClient_GetCellVoltageTarget(AbsScpiClientHandle handle,
                                    unsigned int cell, float* voltage_out);

int ScpiClient_GetAllCellVoltageTarget(AbsScpiClientHandle handle,
                                       float* voltages_out, unsigned int count);

int ScpiClient_SetCellSourcing(AbsScpiClientHandle handle, unsigned int cell, float limit);

int ScpiClient_SetAllCellSourcing(AbsScpiClientHandle handle, const float* limits, unsigned int count);

int ScpiClient_GetCellSourcingLimit(AbsScpiClientHandle handle, unsigned int cell, float* limit_out);

int ScpiClient_GetAllCellSourcingLimit(AbsScpiClientHandle handle, float* limits_out, unsigned int count);

int ScpiClient_SetCellSinking(AbsScpiClientHandle handle, unsigned int cell, float limit);

int ScpiClient_SetAllCellSinking(AbsScpiClientHandle handle, const float* limits, unsigned int count);

int ScpiClient_GetCellSinkingLimit(AbsScpiClientHandle handle, unsigned int cell, float* limit_out);

int ScpiClient_GetAllCellSinkingLimit(AbsScpiClientHandle handle, float* limits_out, unsigned int count);

int ScpiClient_SetCellFault(AbsScpiClientHandle handle, unsigned int cell, int fault);

int ScpiClient_SetAllCellFault(AbsScpiClientHandle handle, const int* faults, unsigned int count);

int ScpiClient_GetCellFault(AbsScpiClientHandle handle, unsigned int cell, int* fault_out);

int ScpiClient_GetAllCellFault(AbsScpiClientHandle handle, int* faults_out, unsigned int count);

int ScpiClient_SetCellSenseRange(AbsScpiClientHandle handle, unsigned int cell, int range);

int ScpiClient_SetAllCellSenseRange(AbsScpiClientHandle handle, const int* ranges, unsigned int count);

int ScpiClient_GetCellSenseRange(AbsScpiClientHandle handle, unsigned int cell, int* range_out);

int ScpiClient_GetAllCellSenseRange(AbsScpiClientHandle handle, int* ranges_out, unsigned int count);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H */
