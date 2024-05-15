#include <bci/abs/CInterface.h>
#include <bci/abs/ScpiClient.h>
#include <bci/abs/UdpDriver.h>

#include <memory>

using namespace bci::abs;
using ec = bci::abs::ErrorCode;

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

int ScpiClient_OpenUdp(AbsScpiClientHandle handle, const char* local_ip,
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

int ScpiClient_SetCellVoltage(AbsScpiClientHandle handle, unsigned int cell,
                              float voltage) {
  return WrapSet(&ScpiClient::SetCellVoltage, handle, cell, voltage);
}

int ScpiClient_GetCellVoltageTarget(AbsScpiClientHandle handle,
                                    unsigned int cell, float* voltage_out) {
  return WrapGet(&ScpiClient::GetCellVoltageTarget, handle, voltage_out, cell);
}
