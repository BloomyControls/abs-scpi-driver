#include <bci/abs/ScpiClient.h>
#include <fmt/core.h>

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "ScpiUtil.h"
#include "Util.h"

namespace bci::abs {

static constexpr unsigned int kCellsMask = ((1U << kCellCount) - 1);

static constexpr unsigned int kWriteTimeoutMs = 250;
static constexpr unsigned int kReadTimeoutMs = 500;

static constexpr float kMaxVoltage = 5.0f;
static constexpr float kMaxSourcing = 5.0f;
static constexpr float kMaxSinking = 5.0f;
static constexpr float kMaxAnalogOutVoltage = 10.0f;

using util::Err;
using ec = ErrorCode;

ScpiClient::ScpiClient(std::shared_ptr<drivers::CommDriver> driver)
    : driver_(std::move(driver)) {}

ScpiClient::~ScpiClient() {}

Result<DeviceInfo> ScpiClient::GetDeviceInfo() const {
  auto res = WriteAndRead("*IDN?\r\n");
  if (!res) {
    return Err(res.error());
  }

  std::array<std::string_view, 4> idn;
  if (scpi::SplitRespMnemonics(*res, idn) != ErrorCode::kSuccess) {
    return Err(ErrorCode::kInvalidResponse);
  }

  return DeviceInfo{std::string(idn[1]), std::string(idn[2]),
                    std::string(idn[3])};
}

Result<std::uint8_t> ScpiClient::GetDeviceId() const {
  return WriteAndRead("CONF:COMM:SER:ID?\r\n")
      .and_then(scpi::ParseIntResponse<std::uint8_t>);
}

Result<int> ScpiClient::GetErrorCount() const {
  return WriteAndRead("SYST:ERR:COUN?\r\n")
      .and_then(scpi::ParseIntResponse<int>);
}

ErrorCode ScpiClient::Reboot() const {
  return driver_->Write("*RST\r\n", kWriteTimeoutMs);
}

ErrorCode ScpiClient::EnableCell(unsigned int cell, bool en) const {
  if (cell >= kCellCount) {
    return ec::kChannelIndexOutOfRange;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP{} {:d}\r\n", cell + 1, en);

  return driver_->Write(buf, kWriteTimeoutMs);
}

Result<bool> ScpiClient::GetCellEnabled(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP{}?\r\n", cell + 1);

  return WriteAndRead(buf).and_then(scpi::ParseBoolResponse);
}

ErrorCode ScpiClient::EnableCellsMasked(unsigned int cells, bool en) const {
  cells &= kCellsMask;
  if (cells != 0) {
    // TODO: rewrite without allocation
    std::string buf = fmt::format("OUTP {:d},(@", en);
    for (unsigned int i = 0; i < kCellCount && cells != 0; ++i, cells >>= 1) {
      if (cells & 1) {
        buf += fmt::format("{}{}", (i > 0) ? "," : "", i + 1);
      }
    }
    buf += ")\r\n";
    return driver_->Write(buf, kWriteTimeoutMs);
  }
  return ec::kSuccess;
}

ErrorCode ScpiClient::SetCellVoltage(unsigned int cell, float voltage) const {
  if (cell >= kCellCount) {
    return ec::kChannelIndexOutOfRange;
  }

  voltage = std::clamp(voltage, 0.0f, kMaxVoltage);

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR{}:VOLT {:.4f}\r\n", cell + 1,
                   voltage);

  return driver_->Write(buf, kWriteTimeoutMs);
}

Result<float> ScpiClient::GetCellVoltageTarget(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR{}:VOLT?\r\n", cell + 1);

  return WriteAndRead(buf).and_then(scpi::ParseFloatResponse);
}

ErrorCode ScpiClient::SetCellSourcing(unsigned int cell, float limit) const {
  if (cell >= kCellCount) {
    return ec::kChannelIndexOutOfRange;
  }

  limit = std::clamp(limit, 0.0f, kMaxSourcing);

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR{}:CURR:SRC {:.4f}\r\n", cell + 1,
                   limit);

  return driver_->Write(buf, kWriteTimeoutMs);
}

Result<float> ScpiClient::GetCellSourcingLimit(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR{}:CURR:SRC?\r\n", cell + 1);

  return WriteAndRead(buf).and_then(scpi::ParseFloatResponse);
}

ErrorCode ScpiClient::SetCellSinking(unsigned int cell, float limit) const {
  if (cell >= kCellCount) {
    return ec::kChannelIndexOutOfRange;
  }

  limit = std::clamp(limit, -kMaxSinking, kMaxSinking);

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR{}:CURR:SNK {:.4f}\r\n", cell + 1,
                   limit);

  return driver_->Write(buf, kWriteTimeoutMs);
}

Result<float> ScpiClient::GetCellSinkingLimit(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR{}:CURR:SNK?\r\n", cell + 1);

  return WriteAndRead(buf).and_then(scpi::ParseFloatResponse);
}

ErrorCode ScpiClient::SetCellFault(unsigned int cell, CellFault fault) const {
  if (cell >= kCellCount) {
    return ec::kChannelIndexOutOfRange;
  }

  auto fstr = scpi::CellFaultMnemonic(fault);
  if (fstr.empty()) {
    return ec::kInvalidFaultType;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP{}:FAUL {}\r\n", cell + 1, fstr);

  return driver_->Write(buf, kWriteTimeoutMs);
}

Result<CellFault> ScpiClient::GetCellFault(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP{}:FAUL?\r\n", cell + 1);

  return WriteAndRead(buf).and_then(scpi::ParseCellFault);
}

ErrorCode ScpiClient::SetCellSenseRange(unsigned int cell,
                                        CellSenseRange range) const {
  if (cell >= kCellCount) {
    return ec::kChannelIndexOutOfRange;
  }

  auto rstr = scpi::CellSenseRangeMnemonic(range);
  if (rstr.empty()) {
    return ec::kInvalidSenseRange;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SENS{}:RANG {}\r\n", cell + 1, rstr);

  return driver_->Write(buf, kWriteTimeoutMs);
}

Result<CellSenseRange> ScpiClient::GetCellSenseRange(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SENS{}:RANG?\r\n", cell + 1);

  return WriteAndRead(buf).and_then(scpi::ParseCellSenseRange);
}

Result<float> ScpiClient::MeasureCellVoltage(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS{}:VOLT?\r\n", cell + 1);

  return WriteAndRead(buf).and_then(scpi::ParseFloatResponse);
}

Result<float> ScpiClient::MeasureCellCurrent(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS{}:CURR?\r\n", cell + 1);

  return WriteAndRead(buf).and_then(scpi::ParseFloatResponse);
}

ErrorCode ScpiClient::SetAnalogOutput(unsigned int channel,
                                      float voltage) const {
  if (channel >= kAnalogOutputCount) {
    return ec::kChannelIndexOutOfRange;
  }

  voltage = std::clamp(voltage, -kMaxAnalogOutVoltage, kMaxAnalogOutVoltage);

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:AUX:OUT{} {:.3f}\r\n",
                   channel + 1, voltage);

  return driver_->Write(buf, kWriteTimeoutMs);
}

Result<float> ScpiClient::GetAnalogOutput(unsigned int channel) const {
  if (channel >= kAnalogOutputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:AUX:OUT{}?\r\n", channel + 1);

  return WriteAndRead(buf).and_then(scpi::ParseFloatResponse);
}

ErrorCode ScpiClient::SetDigitalOutput(unsigned int channel, bool level) const {
  if (channel >= kDigitalOutputCount) {
    return ec::kChannelIndexOutOfRange;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:DAUX:OUT{} {:d}\r\n",
                   channel + 1, level);

  return driver_->Write(buf, kWriteTimeoutMs);
}

Result<bool> ScpiClient::GetDigitalOutput(unsigned int channel) const {
  if (channel >= kDigitalOutputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:DAUX:OUT{}?\r\n", channel + 1);

  return WriteAndRead(buf).and_then(scpi::ParseBoolResponse);
}

Result<float> ScpiClient::MeasureAnalogInput(unsigned int channel) const {
  if (channel >= kAnalogInputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:AUX:IN{}?\r\n", channel + 1);

  return WriteAndRead(buf).and_then(scpi::ParseFloatResponse);
}

Result<bool> ScpiClient::MeasureDigitalInput(unsigned int channel) const {
  if (channel >= kDigitalInputCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:DAUX:IN{}?\r\n", channel + 1);

  return WriteAndRead(buf).and_then(scpi::ParseBoolResponse);
}

Result<std::string> ScpiClient::WriteAndRead(std::string_view buf) const {
  auto res = driver_->Write(buf, kWriteTimeoutMs);
  if (res != ErrorCode::kSuccess) {
    return Err(res);
  }
  return driver_->ReadLine(kReadTimeoutMs);
}

}  // namespace bci::abs
