#include <bci/abs/ScpiClient.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <array>
#include <span>
#include <string>
#include <string_view>

#include "ScpiUtil.h"
#include "Util.h"

namespace bci::abs {

static constexpr unsigned int kCellsMask = ((1U << kCellCount) - 1);

static constexpr float kMaxVoltage = 5.0f;
static constexpr float kMaxSourcing = 5.0f;
static constexpr float kMaxSinking = 5.0f;

using util::Err;
using ec = ErrorCode;

ErrorCode ScpiClient::EnableCell(unsigned int cell, bool en) const {
  if (cell >= kCellCount) {
    return ec::kChannelIndexOutOfRange;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP{} {:d}\r\n", cell + 1, en);

  return Send(buf);
}

ErrorCode ScpiClient::EnableCellsMasked(unsigned int cells, bool en) const {
  cells &= kCellsMask;
  if (cells != 0) {
    // we can avoid allocations by using an array of indices
    std::array<unsigned int, kCellCount> which_cells{};
    std::size_t count = 0;
    for (unsigned int i = 0; i < kCellCount && cells != 0; ++i, cells >>= 1) {
      if (cells & 1) {
        which_cells[count++] = i + 1;
      }
    }
    std::span chan_list{which_cells.data(), count};
    char buf[64]{};
    fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP {:d},(@{})\r\n", en,
                     fmt::join(chan_list, ","));
    return Send(buf);
  }
  return ec::kSuccess;
}

Result<bool> ScpiClient::GetCellEnabled(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP{}?\r\n", cell + 1);

  return SendAndRecv(buf).and_then(scpi::ParseBoolResponse);
}

Result<std::array<bool, kCellCount>> ScpiClient::GetAllCellsEnabled() const {
  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP? (@1:{})\r\n", kCellCount);
  return SendAndRecv(buf).and_then(scpi::ParseRespBoolArray<kCellCount>);
}

Result<unsigned int> ScpiClient::GetAllCellsEnabledMasked() const {
  auto resp = GetAllCellsEnabled();
  if (!resp) {
    return Err(resp.error());
  }

  unsigned int mask{};
  for (std::size_t i = 0; i < resp->size(); ++i) {
    if (resp->at(i)) {
      mask |= (1U << i);
    }
  }

  return mask;
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
    const std::array<float, kCellCount>& voltages) const {
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

Result<std::array<float, kCellCount>> ScpiClient::GetAllCellVoltageTarget()
    const {
  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:VOLT? (@1:{})\r\n", kCellCount);
  return SendAndRecv(buf).and_then(scpi::ParseRespFloatArray<kCellCount>);
}

ErrorCode ScpiClient::GetAllCellVoltageTarget(float* voltages,
                                              std::size_t count) const {
  if ((!voltages && count > 0) || count > kCellCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:VOLT? (@1:{})\r\n", count);

  auto resp = SendAndRecv(buf);
  if (!resp) {
    return resp.error();
  }

  return scpi::SplitRespFloats(*resp, std::span{voltages, count});
}

ErrorCode ScpiClient::GetAllCellVoltageTarget(
    std::array<float, kCellCount>& voltages) const {
  return GetAllCellVoltageTarget(voltages.data(), voltages.size());
}

ErrorCode ScpiClient::GetAllCellVoltageTarget(std::span<float> voltages) const {
  return GetAllCellVoltageTarget(voltages.data(), voltages.size());
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
    const std::array<float, kCellCount>& limits) const {
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

Result<std::array<float, kCellCount>> ScpiClient::GetAllCellSourcingLimit()
    const {
  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:CURR:SRC? (@1:{})\r\n",
                   kCellCount);
  return SendAndRecv(buf).and_then(scpi::ParseRespFloatArray<kCellCount>);
}

ErrorCode ScpiClient::GetAllCellSourcingLimit(float* limits,
                                              std::size_t count) const {
  if ((!limits && count > 0) || count > kCellCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:CURR:SRC? (@1:{})\r\n", count);

  auto resp = SendAndRecv(buf);
  if (!resp) {
    return resp.error();
  }

  return scpi::SplitRespFloats(*resp, std::span{limits, count});
}

ErrorCode ScpiClient::GetAllCellSourcingLimit(
    std::array<float, kCellCount>& limits) const {
  return GetAllCellSourcingLimit(limits.data(), limits.size());
}

ErrorCode ScpiClient::GetAllCellSourcingLimit(std::span<float> limits) const {
  return GetAllCellSourcingLimit(limits.data(), limits.size());
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
    const std::array<float, kCellCount>& limits) const {
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

Result<std::array<float, kCellCount>> ScpiClient::GetAllCellSinkingLimit()
    const {
  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:CURR:SNK? (@1:{})\r\n",
                   kCellCount);
  return SendAndRecv(buf).and_then(scpi::ParseRespFloatArray<kCellCount>);
}

ErrorCode ScpiClient::GetAllCellSinkingLimit(float* limits,
                                             std::size_t count) const {
  if ((!limits && count > 0) || count > kCellCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SOUR:CURR:SNK? (@1:{})\r\n", count);

  auto resp = SendAndRecv(buf);
  if (!resp) {
    return resp.error();
  }

  return scpi::SplitRespFloats(*resp, std::span{limits, count});
}

ErrorCode ScpiClient::GetAllCellSinkingLimit(
    std::array<float, kCellCount>& limits) const {
  return GetAllCellSinkingLimit(limits.data(), limits.size());
}

ErrorCode ScpiClient::GetAllCellSinkingLimit(std::span<float> limits) const {
  return GetAllCellSinkingLimit(limits.data(), limits.size());
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
    const std::array<CellFault, kCellCount>& faults) const {
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

Result<std::array<CellFault, kCellCount>> ScpiClient::GetAllCellFault() const {
  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP:FAUL? (@1:{})\r\n", kCellCount);
  return SendAndRecv(buf).and_then(scpi::ParseCellFaultArray<kCellCount>);
}

ErrorCode ScpiClient::GetAllCellFault(CellFault* faults,
                                      std::size_t count) const {
  if ((!faults && count > 0) || count > kCellCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP:FAUL? (@1:{})\r\n", count);

  auto resp = SendAndRecv(buf);
  if (!resp) {
    return resp.error();
  }

  return scpi::ParseRespMnemonics(*resp, std::span{faults, count},
                                  scpi::ParseCellFault);
}

ErrorCode ScpiClient::GetAllCellFault(
    std::array<CellFault, kCellCount>& faults) const {
  return GetAllCellFault(faults.data(), faults.size());
}

ErrorCode ScpiClient::GetAllCellFault(std::span<CellFault> faults) const {
  return GetAllCellFault(faults.data(), faults.size());
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
    const std::array<CellSenseRange, kCellCount>& ranges) const {
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

Result<std::array<CellSenseRange, kCellCount>>
ScpiClient::GetAllCellSenseRange() const {
  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SENS:RANG? (@1:{})\r\n", kCellCount);
  return SendAndRecv(buf).and_then(scpi::ParseCellSenseRangeArray<kCellCount>);
}

ErrorCode ScpiClient::GetAllCellSenseRange(CellSenseRange* ranges,
                                           std::size_t count) const {
  if ((!ranges && count > 0) || count > kCellCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "SENS:RANG? (@1:{})\r\n", count);

  auto resp = SendAndRecv(buf);
  if (!resp) {
    return resp.error();
  }

  return scpi::ParseRespMnemonics(*resp, std::span{ranges, count},
                                  scpi::ParseCellSenseRange);
}

ErrorCode ScpiClient::GetAllCellSenseRange(
    std::array<CellSenseRange, kCellCount>& ranges) const {
  return GetAllCellSenseRange(ranges.data(), ranges.size());
}

ErrorCode ScpiClient::GetAllCellSenseRange(
    std::span<CellSenseRange> ranges) const {
  return GetAllCellSenseRange(ranges.data(), ranges.size());
}

ErrorCode ScpiClient::SetCellPrecisionMode(CellPrecisionMode mode) const {
  auto mstr = scpi::CellPrecisionModeMnemonic(mode);
  if (mstr.empty()) {
    return ec::kInvalidPrecisionMode;
  }

  char buf[64]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "CONF:MEAS:MODE {}\r\n", mstr);

  return Send(buf);
}

Result<CellPrecisionMode> ScpiClient::GetCellPrecisionMode() const {
  return SendAndRecv("CONF:MEAS:MODE?\r\n")
      .and_then(scpi::ParseCellPrecisionMode);
}

Result<float> ScpiClient::MeasureCellVoltage(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS{}:VOLT?\r\n", cell + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
}

Result<std::array<float, kCellCount>> ScpiClient::MeasureAllCellVoltage()
    const {
  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:VOLT? (@1:{})\r\n", kCellCount);
  return SendAndRecv(buf).and_then(scpi::ParseRespFloatArray<kCellCount>);
}

ErrorCode ScpiClient::MeasureAllCellVoltage(float* voltages,
                                            std::size_t count) const {
  if ((!voltages && count > 0) || count > kCellCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:VOLT? (@1:{})\r\n", count);

  auto resp = SendAndRecv(buf);
  if (!resp) {
    return resp.error();
  }

  return scpi::SplitRespFloats(*resp, std::span{voltages, count});
}

ErrorCode ScpiClient::MeasureAllCellVoltage(
    std::array<float, kCellCount>& voltages) const {
  return MeasureAllCellVoltage(voltages.data(), voltages.size());
}

ErrorCode ScpiClient::MeasureAllCellVoltage(std::span<float> voltages) const {
  return MeasureAllCellVoltage(voltages.data(), voltages.size());
}

Result<float> ScpiClient::MeasureCellCurrent(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS{}:CURR?\r\n", cell + 1);

  return SendAndRecv(buf).and_then(scpi::ParseFloatResponse);
}

Result<std::array<float, kCellCount>> ScpiClient::MeasureAllCellCurrent()
    const {
  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:CURR? (@1:{})\r\n", kCellCount);
  return SendAndRecv(buf).and_then(scpi::ParseRespFloatArray<kCellCount>);
}

ErrorCode ScpiClient::MeasureAllCellCurrent(float* currents,
                                            std::size_t count) const {
  if ((!currents && count > 0) || count > kCellCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "MEAS:CURR? (@1:{})\r\n", count);

  auto resp = SendAndRecv(buf);
  if (!resp) {
    return resp.error();
  }

  return scpi::SplitRespFloats(*resp, std::span{currents, count});
}

ErrorCode ScpiClient::MeasureAllCellCurrent(
    std::array<float, kCellCount>& currents) const {
  return MeasureAllCellCurrent(currents.data(), currents.size());
}

ErrorCode ScpiClient::MeasureAllCellCurrent(std::span<float> currents) const {
  return MeasureAllCellCurrent(currents.data(), currents.size());
}

Result<CellMode> ScpiClient::GetCellOperatingMode(unsigned int cell) const {
  if (cell >= kCellCount) {
    return Err(ec::kChannelIndexOutOfRange);
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP{}:MODE?\r\n", cell + 1);

  return SendAndRecv(buf).and_then(scpi::ParseCellOperatingMode);
}

Result<std::array<CellMode, kCellCount>> ScpiClient::GetAllCellOperatingMode()
    const {
  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP:MODE? (@1:{})\r\n", kCellCount);
  return SendAndRecv(buf).and_then(
      scpi::ParseCellOperatingModeArray<kCellCount>);
}

ErrorCode ScpiClient::GetAllCellOperatingMode(CellMode* modes,
                                              std::size_t count) const {
  if ((!modes && count > 0) || count > kCellCount) {
    return ec::kInvalidArgument;
  }

  if (count == 0) {
    return ec::kSuccess;
  }

  char buf[32]{};
  fmt::format_to_n(buf, sizeof(buf) - 1, "OUTP:MODE? (@1:{})\r\n", count);

  auto resp = SendAndRecv(buf);
  if (!resp) {
    return resp.error();
  }

  return scpi::ParseRespMnemonics(*resp, std::span{modes, count},
                                  scpi::ParseCellOperatingMode);
}

ErrorCode ScpiClient::GetAllCellOperatingMode(
    std::array<CellMode, kCellCount>& modes) const {
  return GetAllCellOperatingMode(modes.data(), modes.size());
}

ErrorCode ScpiClient::GetAllCellOperatingMode(std::span<CellMode> modes) const {
  return GetAllCellOperatingMode(modes.data(), modes.size());
}

}  // namespace bci::abs
