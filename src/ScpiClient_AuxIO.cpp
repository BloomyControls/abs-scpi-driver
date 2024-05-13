#include <bci/abs/ScpiClient.h>
#include <fmt/core.h>

#include <string_view>

#include "ScpiUtil.h"
#include "Util.h"

namespace bci::abs {

static constexpr float kMaxAnalogOutVoltage = 10.0f;

using util::Err;
using ec = ErrorCode;

ErrorCode ScpiClient::SetAnalogOutput(unsigned int channel,
                                      float voltage) const {
  if (channel >= kAnalogOutputCount) {
    return ec::kChannelIndexOutOfRange;
  }

  voltage = std::clamp(voltage, -kMaxAnalogOutVoltage, kMaxAnalogOutVoltage);

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:AUX:OUT{} {:.3f}\r\n",
                   channel + 1, voltage);

  return Send(buf);
}

Result<float> ScpiClient::GetAnalogOutput(unsigned int channel) const {
  if (channel >= kAnalogOutputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:AUX:OUT{}?\r\n", channel + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
}

ErrorCode ScpiClient::SetDigitalOutput(unsigned int channel, bool level) const {
  if (channel >= kDigitalOutputCount) {
    return ec::kChannelIndexOutOfRange;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:DAUX:OUT{} {:d}\r\n",
                   channel + 1, level);

  return Send(buf);
}

Result<bool> ScpiClient::GetDigitalOutput(unsigned int channel) const {
  if (channel >= kDigitalOutputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:DAUX:OUT{}?\r\n", channel + 1);

  return SendAndRecv(buf).and_then(scpi::ParseBoolResponse);
}

Result<float> ScpiClient::MeasureAnalogInput(unsigned int channel) const {
  if (channel >= kAnalogInputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:AUX:IN{}?\r\n", channel + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
}

Result<bool> ScpiClient::MeasureDigitalInput(unsigned int channel) const {
  if (channel >= kDigitalInputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:DAUX:IN{}?\r\n", channel + 1);

  return SendAndRecv(buf).and_then(scpi::ParseBoolResponse);
}

}  // namespace bci::abs
