#include <bci/abs/CInterface.h>
#include <bci/abs/ScpiClient.h>
#include <bci/abs/SerialDriver.h>
#include <bci/abs/TcpDriver.h>
#include <bci/abs/UdpDriver.h>
#include <bci/abs/UdpMulticastDriver.h>
#include <string.h>  // for strnlen()

#include <array>
#include <cstddef>
#include <memory>
#include <span>
#include <type_traits>

using namespace bci::abs;
using ec = bci::abs::ErrorCode;
using sc = bci::abs::ScpiClient;

static ScpiClient& GetClient(AbsScpiClientHandle handle) {
  return *(ScpiClient*)handle;
}

template <class... Args>
static int WrapSet(ErrorCode (ScpiClient::*func)(Args...) const,
                   AbsScpiClientHandle handle, Args... args) {
  if (!handle || !func) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  return static_cast<int>((GetClient(handle).*func)(args...));
}

template <class T, class... Args>
static int WrapGet(Result<T> (ScpiClient::*func)(Args...) const,
                   AbsScpiClientHandle handle, T* res, Args... args) {
  if (!handle || !func || !res) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  ec err = ec::kSuccess;
  if (auto result = (GetClient(handle).*func)(args...)) {
    *res = *result;
  } else {
    err = result.error();
  }

  return static_cast<int>(err);
}

template <class... Args>
static int WrapGet(ErrorCode (ScpiClient::*func)(Args...) const,
                   AbsScpiClientHandle handle, Args... args) {
  if (!handle || !func) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  return static_cast<int>((GetClient(handle).*func)(args...));
}

// Get a view over a char array that may or may not be null-terminated.
template <std::size_t kLen>
inline constexpr std::string_view CharsView(const char (&str)[kLen]) {
  return std::string_view(str, ::strnlen(str, kLen));
}

const char* AbsScpiClient_ErrorMessage(int error) {
  return bci::abs::ErrorMessage(static_cast<bci::abs::ErrorCode>(error));
}

int AbsScpiClient_Init(AbsScpiClientHandle* handle_out) {
  if (!handle_out) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  ScpiClient*& client_ptr = *(ScpiClient**)handle_out;
  if (!client_ptr) {
    client_ptr = new ScpiClient();
  }

  return static_cast<int>(ec::kSuccess);
}

void AbsScpiClient_Destroy(AbsScpiClientHandle* handle) {
  if (handle) {
    delete (ScpiClient*)*handle;
    *handle = nullptr;
  }
}

int AbsScpiClient_OpenUdp(AbsScpiClientHandle handle, const char* target_ip,
                          const char* interface_ip) {
  if (!handle || !target_ip) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  GetClient(handle).SetDriver(nullptr);

  auto driver = std::make_shared<drivers::UdpDriver>();
  ec ret;
  if (interface_ip) {
    ret = driver->Open(interface_ip, target_ip);
  } else {
    ret = driver->Open(target_ip);
  }
  if (ret == ec::kSuccess) {
    GetClient(handle).SetDriver(driver);
  }

  return static_cast<int>(ret);
}

int AbsScpiClient_OpenTcp(AbsScpiClientHandle handle, const char* target_ip) {
  if (!handle || !target_ip) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  GetClient(handle).SetDriver(nullptr);

  auto driver = std::make_shared<drivers::TcpDriver>();
  ec ret = driver->Connect(target_ip, 500);
  if (ret == ec::kSuccess) {
    GetClient(handle).SetDriver(driver);
  }

  return static_cast<int>(ret);
}

int AbsScpiClient_OpenSerial(AbsScpiClientHandle handle, const char* com_port,
                             unsigned int device_id) {
  if (!handle || !com_port) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  GetClient(handle).SetDriver(nullptr);

  auto driver = std::make_shared<drivers::SerialDriver>();
  ec ret = driver->Open(com_port);
  if (ret == ec::kSuccess) {
    driver->SetDeviceID(device_id);
    GetClient(handle).SetDriver(driver);
  }

  return static_cast<int>(ret);
}

int AbsScpiClient_OpenUdpMulticast(AbsScpiClientHandle handle,
                                   const char* interface_ip) {
  if (!handle || !interface_ip) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  GetClient(handle).SetDriver(nullptr);

  auto driver = std::make_shared<drivers::UdpMcastDriver>();
  ec ret = driver->Open(interface_ip);
  if (ret == ec::kSuccess) {
    GetClient(handle).SetDriver(driver);
  }

  return static_cast<int>(ret);
}

int AbsScpiClient_GetDeviceInfo(AbsScpiClientHandle handle,
                                AbsDeviceInfo* info_out) {
  if (!info_out) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  bci::abs::DeviceInfo dev_info{};

  int ret = WrapGet(&sc::GetDeviceInfo, handle, &dev_info);
  if (ret != static_cast<int>(ec::kSuccess)) {
    return ret;
  }

  *info_out = {};

  auto&& [pn, sn, ver] = dev_info;
  auto&& [pn_out, sn_out, ver_out] = *info_out;

  pn.copy(pn_out, sizeof(pn_out) - 1);
  sn.copy(sn_out, sizeof(sn_out) - 1);
  ver.copy(ver_out, sizeof(ver_out) - 1);

  return static_cast<int>(ec::kSuccess);
}

int AbsScpiClient_GetDeviceId(AbsScpiClientHandle handle, uint8_t* id_out) {
  return WrapGet(&sc::GetDeviceId, handle, id_out);
}

int AbsScpiClient_GetIPAddress(AbsScpiClientHandle handle,
                               AbsEthernetConfig* addr_out) {
  if (!addr_out) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  bci::abs::EthernetConfig addr{};

  int ret = WrapGet(&sc::GetIPAddress, handle, &addr);
  if (ret != static_cast<int>(ec::kSuccess)) {
    return ret;
  }

  *addr_out = {};

  auto&& [ip, mask] = addr;
  auto&& [ip_out, mask_out] = *addr_out;

  ip.copy(ip_out, sizeof(ip_out) - 1);
  mask.copy(mask_out, sizeof(mask_out) - 1);

  return static_cast<int>(ec::kSuccess);
}

int AbsScpiClient_SetIPAddress(AbsScpiClientHandle handle,
                               const AbsEthernetConfig* addr) {
  if (!addr) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  return WrapSet(&sc::SetIPAddress, handle, CharsView(addr->ip),
                 CharsView(addr->netmask));
}

int AbsScpiClient_GetCalibrationDate(AbsScpiClientHandle handle, char* buf,
                                     unsigned int len) {
  if (!buf || len == 0) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  std::string date{};
  int ret = WrapGet(&sc::GetCalibrationDate, handle, &date);
  if (ret != static_cast<int>(ec::kSuccess)) {
    return ret;
  }

  // must have enough space for the date + null terminator
  if (len < date.size() + 1) {
    return static_cast<int>(ec::kBufferTooSmall);
  }

  date.copy(buf, date.size());
  buf[date.size()] = '\0';

  return static_cast<int>(ec::kSuccess);
}

int AbsScpiClient_GetErrorCount(AbsScpiClientHandle handle, int* count_out) {
  return WrapGet(&sc::GetErrorCount, handle, count_out);
}

int AbsScpiClient_GetNextError(AbsScpiClientHandle handle,
                               int16_t* err_code_out, char* msg_buf,
                               unsigned int msg_buf_len) {
  if (!err_code_out || !msg_buf || msg_buf_len == 0) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  bci::abs::ScpiError err{};
  int ret = WrapGet(&sc::GetNextError, handle, &err);
  if (ret != static_cast<int>(ec::kSuccess)) {
    return ret;
  }

  *err_code_out = err.err_code;

  if (msg_buf_len < err.err_msg.size() + 1) {
    return static_cast<int>(ec::kBufferTooSmall);
  }

  err.err_msg.copy(msg_buf, err.err_msg.size());
  msg_buf[err.err_msg.size()] = '\0';

  return static_cast<int>(ec::kSuccess);
}

int AbsScpiClient_ClearErrors(AbsScpiClientHandle handle) {
  return WrapSet(&sc::ClearErrors, handle);
}

int AbsScpiClient_GetAlarms(AbsScpiClientHandle handle, uint32_t* alarms_out) {
  return WrapGet(&sc::GetAlarms, handle, alarms_out);
}

int AbsScpiClient_AssertSoftwareInterlock(AbsScpiClientHandle handle) {
  return WrapSet(&sc::AssertSoftwareInterlock, handle);
}

int AbsScpiClient_ClearRecoverableAlarms(AbsScpiClientHandle handle) {
  return WrapSet(&sc::ClearRecoverableAlarms, handle);
}

int AbsScpiClient_Reboot(AbsScpiClientHandle handle) {
  return WrapSet(&sc::Reboot, handle);
}

int AbsScpiClient_EnableCell(AbsScpiClientHandle handle, unsigned int cell,
                             bool en) {
  return WrapSet(&sc::EnableCell, handle, cell, en);
}

int AbsScpiClient_EnableCellsMasked(AbsScpiClientHandle handle,
                                    unsigned int cells, bool en) {
  return WrapSet(&sc::EnableCellsMasked, handle, cells, en);
}

int AbsScpiClient_GetCellEnabled(AbsScpiClientHandle handle, unsigned int cell,
                                 bool* en_out) {
  return WrapGet(&sc::GetCellEnabled, handle, en_out, cell);
}

int AbsScpiClient_GetCellsEnabledMasked(AbsScpiClientHandle handle,
                                        unsigned int* cells_out) {
  return WrapGet(&sc::GetAllCellsEnabledMasked, handle, cells_out);
}

int AbsScpiClient_SetCellVoltage(AbsScpiClientHandle handle, unsigned int cell,
                                 float voltage) {
  return WrapSet(&sc::SetCellVoltage, handle, cell, voltage);
}

int AbsScpiClient_SetAllCellVoltage(AbsScpiClientHandle handle,
                                    const float voltages[],
                                    unsigned int count) {
  return WrapSet(&sc::SetAllCellVoltage, handle, voltages,
                 static_cast<std::size_t>(count));
}

int AbsScpiClient_GetCellVoltageTarget(AbsScpiClientHandle handle,
                                       unsigned int cell, float* voltage_out) {
  return WrapGet(&sc::GetCellVoltageTarget, handle, voltage_out, cell);
}

int AbsScpiClient_GetAllCellVoltageTarget(AbsScpiClientHandle handle,
                                          float voltages_out[],
                                          unsigned int count) {
  return WrapGet(&sc::GetAllCellVoltageTarget, handle, voltages_out,
                 static_cast<std::size_t>(count));
}

int AbsScpiClient_SetCellSourcing(AbsScpiClientHandle handle, unsigned int cell,
                                  float limit) {
  return WrapSet(&sc::SetCellSourcing, handle, cell, limit);
}

int AbsScpiClient_SetAllCellSourcing(AbsScpiClientHandle handle,
                                     const float limits[], unsigned int count) {
  return WrapSet(&sc::SetAllCellSourcing, handle, limits,
                 static_cast<std::size_t>(count));
}

int AbsScpiClient_GetCellSourcingLimit(AbsScpiClientHandle handle,
                                       unsigned int cell, float* limit_out) {
  return WrapGet(&sc::GetCellSourcingLimit, handle, limit_out, cell);
}

int AbsScpiClient_GetAllCellSourcingLimit(AbsScpiClientHandle handle,
                                          float limits_out[],
                                          unsigned int count) {
  return WrapGet(&sc::GetAllCellSourcingLimit, handle, limits_out,
                 static_cast<std::size_t>(count));
}

int AbsScpiClient_SetCellSinking(AbsScpiClientHandle handle, unsigned int cell,
                                 float limit) {
  return WrapSet(&sc::SetCellSinking, handle, cell, limit);
}

int AbsScpiClient_SetAllCellSinking(AbsScpiClientHandle handle,
                                    const float limits[], unsigned int count) {
  return WrapSet(&sc::SetAllCellSinking, handle, limits,
                 static_cast<std::size_t>(count));
}

int AbsScpiClient_GetCellSinkingLimit(AbsScpiClientHandle handle,
                                      unsigned int cell, float* limit_out) {
  return WrapGet(&sc::GetCellSinkingLimit, handle, limit_out, cell);
}

int AbsScpiClient_GetAllCellSinkingLimit(AbsScpiClientHandle handle,
                                         float limits_out[],
                                         unsigned int count) {
  return WrapGet(&sc::GetAllCellSinkingLimit, handle, limits_out,
                 static_cast<std::size_t>(count));
}

static_assert(ABS_CELL_FAULT_NONE == static_cast<int>(CellFault::kNone));
static_assert(ABS_CELL_FAULT_OPEN == static_cast<int>(CellFault::kOpenCircuit));
static_assert(ABS_CELL_FAULT_SHORT ==
              static_cast<int>(CellFault::kShortCircuit));
static_assert(ABS_CELL_FAULT_POLARITY ==
              static_cast<int>(CellFault::kPolarity));
static_assert(std::is_same_v<std::underlying_type_t<CellFault>, int>);

int AbsScpiClient_SetCellFault(AbsScpiClientHandle handle, unsigned int cell,
                               int fault) {
  return WrapSet(&sc::SetCellFault, handle, cell,
                 static_cast<CellFault>(fault));
}

int AbsScpiClient_SetAllCellFault(AbsScpiClientHandle handle,
                                  const int faults[], unsigned int count) {
  return WrapSet(&sc::SetAllCellFault, handle,
                 reinterpret_cast<const CellFault*>(faults),
                 static_cast<std::size_t>(count));
}

int AbsScpiClient_GetCellFault(AbsScpiClientHandle handle, unsigned int cell,
                               int* fault_out) {
  return WrapGet(&sc::GetCellFault, handle,
                 reinterpret_cast<CellFault*>(fault_out), cell);
}

int AbsScpiClient_GetAllCellFault(AbsScpiClientHandle handle, int faults_out[],
                                  unsigned int count) {
  return WrapGet(&sc::GetAllCellFault, handle,
                 reinterpret_cast<CellFault*>(faults_out),
                 static_cast<std::size_t>(count));
}

static_assert(ABS_CELL_SENSE_RANGE_AUTO ==
              static_cast<int>(CellSenseRange::kAuto));
static_assert(ABS_CELL_SENSE_RANGE_1A ==
              static_cast<int>(CellSenseRange::kLow));
static_assert(ABS_CELL_SENSE_RANGE_5A ==
              static_cast<int>(CellSenseRange::kHigh));
static_assert(std::is_same_v<std::underlying_type_t<CellSenseRange>, int>);

int AbsScpiClient_SetCellSenseRange(AbsScpiClientHandle handle,
                                    unsigned int cell, int range) {
  return WrapSet(&sc::SetCellSenseRange, handle, cell,
                 static_cast<CellSenseRange>(range));
}

int AbsScpiClient_SetAllCellSenseRange(AbsScpiClientHandle handle,
                                       const int ranges[], unsigned int count) {
  return WrapSet(&sc::SetAllCellSenseRange, handle,
                 reinterpret_cast<const CellSenseRange*>(ranges),
                 static_cast<std::size_t>(count));
}

int AbsScpiClient_GetCellSenseRange(AbsScpiClientHandle handle,
                                    unsigned int cell, int* range_out) {
  return WrapGet(&sc::GetCellSenseRange, handle,
                 reinterpret_cast<CellSenseRange*>(range_out), cell);
}

int AbsScpiClient_GetAllCellSenseRange(AbsScpiClientHandle handle,
                                       int ranges_out[], unsigned int count) {
  return WrapGet(&sc::GetAllCellSenseRange, handle,
                 reinterpret_cast<CellSenseRange*>(ranges_out),
                 static_cast<std::size_t>(count));
}

static_assert(ABS_CELL_PREC_NORMAL ==
              static_cast<int>(CellPrecisionMode::kNormal));
static_assert(ABS_CELL_PREC_HIGH ==
              static_cast<int>(CellPrecisionMode::kHighPrecision));
static_assert(ABS_CELL_PREC_FILTER ==
              static_cast<int>(CellPrecisionMode::kNoiseRejection));
static_assert(std::is_same_v<std::underlying_type_t<CellPrecisionMode>, int>);

int AbsScpiClient_SetCellPrecisionMode(AbsScpiClientHandle handle, int mode) {
  return WrapSet(&sc::SetCellPrecisionMode, handle,
                 static_cast<CellPrecisionMode>(mode));
}

int AbsScpiClient_GetCellPrecisionMode(AbsScpiClientHandle handle,
                                       int* mode_out) {
  return WrapGet(&sc::GetCellPrecisionMode, handle,
                 reinterpret_cast<CellPrecisionMode*>(mode_out));
}

int AbsScpiClient_MeasureCellVoltage(AbsScpiClientHandle handle,
                                     unsigned int cell, float* voltage_out) {
  return WrapGet(&sc::MeasureCellVoltage, handle, voltage_out, cell);
}

int AbsScpiClient_MeasureAllCellVoltage(AbsScpiClientHandle handle,
                                        float voltages_out[],
                                        unsigned int count) {
  return WrapGet(&sc::MeasureAllCellVoltage, handle, voltages_out,
                 static_cast<std::size_t>(count));
}

int AbsScpiClient_MeasureCellCurrent(AbsScpiClientHandle handle,
                                     unsigned int cell, float* current_out) {
  return WrapGet(&sc::MeasureCellCurrent, handle, current_out, cell);
}

int AbsScpiClient_MeasureAllCellCurrent(AbsScpiClientHandle handle,
                                        float currents_out[],
                                        unsigned int count) {
  return WrapGet(&sc::MeasureAllCellCurrent, handle, currents_out,
                 static_cast<std::size_t>(count));
}

static_assert(ABS_CELL_MODE_CV == static_cast<int>(CellMode::kConstantVoltage));
static_assert(ABS_CELL_MODE_ILIM ==
              static_cast<int>(CellMode::kCurrentLimited));
static_assert(std::is_same_v<std::underlying_type_t<CellMode>, int>);

int AbsScpiClient_GetCellOperatingMode(AbsScpiClientHandle handle,
                                       unsigned int cell, int* mode_out) {
  return WrapGet(&sc::GetCellOperatingMode, handle,
                 reinterpret_cast<CellMode*>(mode_out), cell);
}

int AbsScpiClient_GetAllCellOperatingMode(AbsScpiClientHandle handle,
                                          int modes_out[], unsigned int count) {
  return WrapGet(&sc::GetAllCellOperatingMode, handle,
                 reinterpret_cast<CellMode*>(modes_out),
                 static_cast<std::size_t>(count));
}

int AbsScpiClient_SetAnalogOutput(AbsScpiClientHandle handle,
                                  unsigned int channel, float voltage) {
  return WrapSet(&sc::SetAnalogOutput, handle, channel, voltage);
}

int AbsScpiClient_SetAllAnalogOutput(AbsScpiClientHandle handle,
                                     const float voltages[],
                                     unsigned int count) {
  return WrapSet(&sc::SetAllAnalogOutput, handle, voltages,
                 static_cast<std::size_t>(count));
}

int AbsScpiClient_GetAnalogOutput(AbsScpiClientHandle handle,
                                  unsigned int channel, float* voltage_out) {
  return WrapGet(&sc::GetAnalogOutput, handle, voltage_out, channel);
}

int AbsScpiClient_GetAllAnalogOutput(AbsScpiClientHandle handle,
                                     float voltages_out[], unsigned int count) {
  return WrapGet(&sc::GetAllAnalogOutput, handle, voltages_out,
                 static_cast<std::size_t>(count));
}

int AbsScpiClient_SetDigitalOutput(AbsScpiClientHandle handle,
                                   unsigned int channel, bool level) {
  return WrapSet(&sc::SetDigitalOutput, handle, channel, level);
}

int AbsScpiClient_SetAllDigitalOutput(AbsScpiClientHandle handle,
                                      unsigned int levels_mask) {
  std::array<bool, kDigitalOutputCount> levels{};
  for (auto i = 0U; i < kDigitalOutputCount; ++i) {
    levels[i] = !!(levels_mask & (1U << i));
  }
  return WrapSet(&sc::SetAllDigitalOutput, handle,
                 std::span<const bool>(levels));
}

int AbsScpiClient_GetDigitalOutput(AbsScpiClientHandle handle,
                                   unsigned int channel, bool* level_out) {
  return WrapGet(&sc::GetDigitalOutput, handle, level_out, channel);
}

int AbsScpiClient_GetAllDigitalOutput(AbsScpiClientHandle handle,
                                      unsigned int* levels_out) {
  return WrapGet(&sc::GetAllDigitalOutputMasked, handle, levels_out);
}

int AbsScpiClient_MeasureAnalogInput(AbsScpiClientHandle handle,
                                     unsigned int channel, float* voltage_out) {
  return WrapGet(&sc::MeasureAnalogInput, handle, voltage_out, channel);
}

int AbsScpiClient_MeasureAllAnalogInput(AbsScpiClientHandle handle,
                                        float voltages_out[],
                                        unsigned int count) {
  return WrapGet(&sc::MeasureAllAnalogInput, handle, voltages_out,
                 static_cast<std::size_t>(count));
}

int AbsScpiClient_MeasureDigitalInput(AbsScpiClientHandle handle,
                                      unsigned int channel, bool* level_out) {
  return WrapGet(&sc::MeasureDigitalInput, handle, level_out, channel);
}

int AbsScpiClient_MeasureAllDigitalInput(AbsScpiClientHandle handle,
                                         unsigned int* levels_out) {
  return WrapGet(&sc::MeasureAllDigitalInputMasked, handle, levels_out);
}
