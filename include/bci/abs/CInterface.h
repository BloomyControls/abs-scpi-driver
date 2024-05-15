#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef void* AbsScpiClientHandle;

int AbsScpiClient_Init(AbsScpiClientHandle* handle_out);

void AbsScpiClient_Destroy(AbsScpiClientHandle* handle);

int ScpiClient_OpenUdp(AbsScpiClientHandle handle, const char* local_ip,
                       const char* target_ip);

int ScpiClient_SetCellVoltage(AbsScpiClientHandle handle, unsigned int cell,
                              float voltage);

int ScpiClient_GetCellVoltageTarget(AbsScpiClientHandle handle,
                                    unsigned int cell, float* voltage_out);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_CINTERFACE_H */
