#include <bci/abs/ScpiClient.h>
#include <fmt/core.h>

#include <array>
#include <cstdint>
#include <memory>
#include <span>
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
  auto res = SendAndRecv("*IDN?\r\n");
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
  return SendAndRecv("CONF:COMM:SER:ID?\r\n")
      .and_then(scpi::ParseIntResponse<std::uint8_t>);
}

Result<int> ScpiClient::GetErrorCount() const {
  return SendAndRecv("SYST:ERR:COUN?\r\n")
      .and_then(scpi::ParseIntResponse<int>);
}

ErrorCode ScpiClient::Reboot() const { return Send("*RST\r\n"); }

ErrorCode ScpiClient::EnableCell(unsigned int cell, bool en) const {
  if (cell >= kCellCount) {
    return ec::kChannelIndexOutOfRange;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP{} {:d}\r\n", cell + 1, en);

  return Send(buf);
}

Result<bool> ScpiClient::GetCellEnabled(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP{}?\r\n", cell + 1);

  return SendAndRecv(buf).and_then(scpi::ParseBoolResponse);
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
    return Send(buf);
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

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellVoltage(float voltage) const {
  voltage = std::clamp(voltage, 0.0f, kMaxVoltage);

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:VOLT {:.4f},(@1:{})\r\n",
                   voltage, kCellCount);

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellVoltage(const float* voltages,
                                        std::size_t count) const {
  if ((count > 0 && !voltages) || count > kCellCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  std::string buf;
  // try to do only one allocation
  buf.reserve(count * 18 + 2);
  for (std::size_t i = 0; i < count; ++i) {
    buf += fmt::format("SOUR{}:VOLT {:.4f};", i + 1,
                       std::clamp(voltages[i], 0.0f, kMaxVoltage));
  }
  buf += "\r\n";

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellVoltage(std::span<const float> voltages) const {
  return SetAllCellVoltage(voltages.data(), voltages.size());
}

ErrorCode ScpiClient::SetAllCellVoltage(
    const std::array<float, 8>& voltages) const {
  return SetAllCellVoltage(voltages.data(), voltages.size());
}

Result<float> ScpiClient::GetCellVoltageTarget(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR{}:VOLT?\r\n", cell + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
}

ErrorCode ScpiClient::SetCellSourcing(unsigned int cell, float limit) const {
  if (cell >= kCellCount) {
    return ec::kChannelIndexOutOfRange;
  }

  limit = std::clamp(limit, 0.0f, kMaxSourcing);

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR{}:CURR:SRC {:.4f}\r\n", cell + 1,
                   limit);

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellSourcing(float limit) const {
  limit = std::clamp(limit, 0.0f, kMaxSourcing);

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:CURR:SRC {:.4f},(@1:{})\r\n",
                   limit, kCellCount);

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellSourcing(const float* limits,
                                         std::size_t count) const {
  if ((count > 0 && !limits) || count > kCellCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  std::string buf;
  // try to do only one allocation
  buf.reserve(count * 22 + 2);
  for (std::size_t i = 0; i < count; ++i) {
    buf += fmt::format("SOUR{}:CURR:SRC {:.4f};", i + 1,
                       std::clamp(limits[i], 0.0f, kMaxSourcing));
  }
  buf += "\r\n";

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellSourcing(std::span<const float> limits) const {
  return SetAllCellSourcing(limits.data(), limits.size());
}

ErrorCode ScpiClient::SetAllCellSourcing(
    const std::array<float, 8>& limits) const {
  return SetAllCellSourcing(limits.data(), limits.size());
}

Result<float> ScpiClient::GetCellSourcingLimit(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR{}:CURR:SRC?\r\n", cell + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
}

ErrorCode ScpiClient::SetCellSinking(unsigned int cell, float limit) const {
  if (cell >= kCellCount) {
    return ec::kChannelIndexOutOfRange;
  }

  limit = std::clamp(limit, -kMaxSinking, kMaxSinking);

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR{}:CURR:SNK {:.4f}\r\n", cell + 1,
                   limit);

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellSinking(float limit) const {
  limit = std::clamp(limit, -kMaxSinking, kMaxSinking);

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:CURR:SNK {:.4f},(@1:{})\r\n",
                   limit, kCellCount);

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellSinking(const float* limits,
                                        std::size_t count) const {
  if ((count > 0 && !limits) || count > kCellCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  std::string buf;
  // try to do only one allocation
  buf.reserve(count * 22 + 2);
  for (std::size_t i = 0; i < count; ++i) {
    buf += fmt::format("SOUR{}:CURR:SNK {:.4f};", i + 1,
                       std::clamp(limits[i], -kMaxSinking, kMaxSinking));
  }
  buf += "\r\n";

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellSinking(std::span<const float> limits) const {
  return SetAllCellSinking(limits.data(), limits.size());
}

ErrorCode ScpiClient::SetAllCellSinking(
    const std::array<float, 8>& limits) const {
  return SetAllCellSinking(limits.data(), limits.size());
}

Result<float> ScpiClient::GetCellSinkingLimit(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR{}:CURR:SNK?\r\n", cell + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
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

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellFault(CellFault fault) const {
  auto fstr = scpi::CellFaultMnemonic(fault);
  if (fstr.empty()) {
    return ec::kInvalidFaultType;
  }

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP:FAUL {},(@1:{})\r\n", fstr,
                   kCellCount);

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellFault(const CellFault* faults,
                                      std::size_t count) const {
  if ((count > 0 && !faults) || count > kCellCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  std::string buf;
  // try to do only one allocation
  buf.reserve(count * 17 + 2);
  for (std::size_t i = 0; i < count; ++i) {
    auto fstr = scpi::CellFaultMnemonic(faults[i]);
    if (fstr.empty()) {
      return ec::kInvalidFaultType;
    }
    buf += fmt::format("OUTP{}:FAUL {};", i + 1, fstr);
  }
  buf += "\r\n";

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellFault(std::span<const CellFault> faults) const {
  return SetAllCellFault(faults.data(), faults.size());
}

ErrorCode ScpiClient::SetAllCellFault(
    const std::array<CellFault, 8>& faults) const {
  return SetAllCellFault(faults.data(), faults.size());
}

Result<CellFault> ScpiClient::GetCellFault(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP{}:FAUL?\r\n", cell + 1);

  return SendAndRecv(buf).and_then(scpi::ParseCellFault);
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

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellSenseRange(CellSenseRange range) const {
  auto rstr = scpi::CellSenseRangeMnemonic(range);
  if (rstr.empty()) {
    return ec::kInvalidSenseRange;
  }

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SENS:RANG {},(@1:{})\r\n", rstr,
                   kCellCount);

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellSenseRange(const CellSenseRange* ranges,
                                           std::size_t count) const {
  if ((count > 0 && !ranges) || count > kCellCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  std::string buf;
  // try to do only one allocation
  buf.reserve(count * 16 + 2);
  for (std::size_t i = 0; i < count; ++i) {
    auto rstr = scpi::CellSenseRangeMnemonic(ranges[i]);
    if (rstr.empty()) {
      return ec::kInvalidSenseRange;
    }
    buf += fmt::format("SENS{}:RANG {};", i + 1, rstr);
  }
  buf += "\r\n";

  return Send(buf);
}

ErrorCode ScpiClient::SetAllCellSenseRange(
    std::span<const CellSenseRange> ranges) const {
  return SetAllCellSenseRange(ranges.data(), ranges.size());
}

ErrorCode ScpiClient::SetAllCellSenseRange(
    const std::array<CellSenseRange, 8>& ranges) const {
  return SetAllCellSenseRange(ranges.data(), ranges.size());
}

Result<CellSenseRange> ScpiClient::GetCellSenseRange(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SENS{}:RANG?\r\n", cell + 1);

  return SendAndRecv(buf).and_then(scpi::ParseCellSenseRange);
}

Result<float> ScpiClient::MeasureCellVoltage(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS{}:VOLT?\r\n", cell + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
}

Result<float> ScpiClient::MeasureCellCurrent(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS{}:CURR?\r\n", cell + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
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

ErrorCode ScpiClient::Send(std::string_view buf) const {
  if (!driver_) {
    return ec::kInvalidDriverHandle;
  }

  return driver_->Write(buf, kWriteTimeoutMs);
}

Result<std::string> ScpiClient::SendAndRecv(std::string_view buf) const {
  if (!driver_) {
    return Err(ec::kInvalidDriverHandle);
  }

  auto res = driver_->Write(buf, kWriteTimeoutMs);
  if (res != ErrorCode::kSuccess) {
    return Err(res);
  }
  return driver_->ReadLine(kReadTimeoutMs);
}

}  // namespace bci::abs
