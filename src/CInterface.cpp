#include <bci/abs/CInterface.h>
#include <bci/abs/ScpiClient.h>
#include <bci/abs/UdpDriver.h>

#include <cstddef>
#include <memory>

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

int AbsScpiClient_Init(AbsScpiClientHandle* handle_out) {
  if (!handle_out) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  ScpiClient*& client_ptr = *(ScpiClient**)handle_out;
  if (client_ptr) {
    // TODO
  }

  client_ptr = new ScpiClient();

  return static_cast<int>(ec::kSuccess);
}

void AbsScpiClient_Destroy(AbsScpiClientHandle* handle) {
  if (handle) {
    delete (ScpiClient*)*handle;
    *handle = nullptr;
  }
}

int AbsScpiClient_OpenUdp(AbsScpiClientHandle handle, const char* local_ip,
                       const char* target_ip) {
  if (!handle) {
    return static_cast<int>(ec::kInvalidArgument);
  }

  // TODO: what's the behavior if it's already got an open driver and this
  // fails?
  auto driver = std::make_shared<drivers::UdpDriver>();
  ec ret = driver->Open(local_ip, target_ip);
  if (ret == ec::kSuccess) {
    GetClient(handle).SetDriver(driver);
  }

  return static_cast<int>(ret);
}

int AbsScpiClient_GetDeviceId(AbsScpiClientHandle handle, uint8_t* id_out) {
  return WrapGet(&sc::GetDeviceId, handle, id_out);
}

int AbsScpiClient_EnableCell(AbsScpiClientHandle handle, unsigned int cell,
                          bool en) {
  return WrapSet(&sc::EnableCell, handle, cell, en);
}

int AbsScpiClient_EnableCellsMasked(AbsScpiClientHandle handle, unsigned int cells,
                                 bool en) {
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
                                 const float* voltages, unsigned int count) {
  return WrapSet(&sc::SetAllCellVoltage, handle, voltages, static_cast<std::size_t>(count));
}

int AbsScpiClient_GetCellVoltageTarget(AbsScpiClientHandle handle,
                                    unsigned int cell, float* voltage_out) {
  return WrapGet(&sc::GetCellVoltageTarget, handle, voltage_out, cell);
}

int AbsScpiClient_GetAllCellVoltageTarget(AbsScpiClientHandle handle,
                                       float* voltages_out, unsigned int count) {
  return WrapGet(&sc::GetAllCellVoltageTarget, handle, voltages_out, static_cast<std::size_t>(count));
}

int AbsScpiClient_SetCellSourcing(AbsScpiClientHandle handle, unsigned int cell, float limit) {
  return WrapSet(&sc::SetCellSourcing, handle, cell, limit);
}

int AbsScpiClient_SetAllCellSourcing(AbsScpiClientHandle handle, const float* limits, unsigned int count) {
  return WrapSet(&sc::SetAllCellSourcing, handle, limits, static_cast<std::size_t>(count));
}

int AbsScpiClient_GetCellSourcingLimit(AbsScpiClientHandle handle, unsigned int cell, float* limit_out) {
  return WrapGet(&sc::GetCellSourcingLimit, handle, limit_out, cell);
}

int AbsScpiClient_GetAllCellSourcingLimit(AbsScpiClientHandle handle, float* limits_out, unsigned int count) {
  return WrapGet(&sc::GetAllCellSourcingLimit, handle, limits_out, static_cast<std::size_t>(count));
}

int AbsScpiClient_SetCellSinking(AbsScpiClientHandle handle, unsigned int cell, float limit) {
  return WrapSet(&sc::SetCellSinking, handle, cell, limit);
}

int AbsScpiClient_SetAllCellSinking(AbsScpiClientHandle handle, const float* limits, unsigned int count) {
  return WrapSet(&sc::SetAllCellSinking, handle, limits, static_cast<std::size_t>(count));
}

int AbsScpiClient_GetCellSinkingLimit(AbsScpiClientHandle handle, unsigned int cell, float* limit_out) {
  return WrapGet(&sc::GetCellSinkingLimit, handle, limit_out, cell);
}

int AbsScpiClient_GetAllCellSinkingLimit(AbsScpiClientHandle handle, float* limits_out, unsigned int count) {
  return WrapGet(&sc::GetAllCellSinkingLimit, handle, limits_out, static_cast<std::size_t>(count));
}

static_assert(ABS_CELL_FAULT_NONE == static_cast<int>(CellFault::kNone));
static_assert(ABS_CELL_FAULT_OPEN == static_cast<int>(CellFault::kOpenCircuit));
static_assert(ABS_CELL_FAULT_SHORT == static_cast<int>(CellFault::kShortCircuit));
static_assert(ABS_CELL_FAULT_POLARITY == static_cast<int>(CellFault::kPolarity));
static_assert(sizeof(CellFault) == sizeof(int));

int AbsScpiClient_SetCellFault(AbsScpiClientHandle handle, unsigned int cell, int fault) {
  return WrapSet(&sc::SetCellFault, handle, cell, static_cast<CellFault>(fault));
}

int AbsScpiClient_SetAllCellFault(AbsScpiClientHandle handle, const int* faults, unsigned int count) {
  return WrapSet(&sc::SetAllCellFault, handle, reinterpret_cast<const CellFault*>(faults), static_cast<std::size_t>(count));
}

int AbsScpiClient_GetCellFault(AbsScpiClientHandle handle, unsigned int cell, int* fault_out) {
  return WrapGet(&sc::GetCellFault, handle, reinterpret_cast<CellFault*>(fault_out), cell);
}

int AbsScpiClient_GetAllCellFault(AbsScpiClientHandle handle, int* faults_out, unsigned int count) {
  return WrapGet(&sc::GetAllCellFault, handle, reinterpret_cast<CellFault*>(faults_out), static_cast<std::size_t>(count));
}

static_assert(ABS_CELL_SENSE_RANGE_AUTO == static_cast<int>(CellSenseRange::kAuto));
static_assert(ABS_CELL_SENSE_RANGE_1A == static_cast<int>(CellSenseRange::kLow));
static_assert(ABS_CELL_SENSE_RANGE_5A == static_cast<int>(CellSenseRange::kHigh));
static_assert(sizeof(CellSenseRange) == sizeof(int));

int AbsScpiClient_SetCellSenseRange(AbsScpiClientHandle handle, unsigned int cell, int range) {
  return WrapSet(&sc::SetCellSenseRange, handle, cell, static_cast<CellSenseRange>(range));
}

int AbsScpiClient_SetAllCellSenseRange(AbsScpiClientHandle handle, const int* ranges, unsigned int count) {
  return WrapSet(&sc::SetAllCellSenseRange, handle, reinterpret_cast<const CellSenseRange*>(ranges), static_cast<std::size_t>(count));
}

int AbsScpiClient_GetCellSenseRange(AbsScpiClientHandle handle, unsigned int cell, int* range_out) {
  return WrapGet(&sc::GetCellSenseRange, handle, reinterpret_cast<CellSenseRange*>(range_out), cell);
}

int AbsScpiClient_GetAllCellSenseRange(AbsScpiClientHandle handle, int* ranges_out, unsigned int count) {
  return WrapGet(&sc::GetAllCellSenseRange, handle, reinterpret_cast<CellSenseRange*>(ranges_out), static_cast<std::size_t>(count));
}
