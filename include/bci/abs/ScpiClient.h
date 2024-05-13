#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SCPICLIENT_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SCPICLIENT_H

#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>

#include "CommDriver.h"
#include "CommonTypes.h"

namespace bci::abs {

class ScpiClient {
 public:
  ScpiClient(std::shared_ptr<drivers::CommDriver> driver);

  ScpiClient(std::nullptr_t) = delete;

  ~ScpiClient();

  /* System */

  Result<DeviceInfo> GetDeviceInfo() const;

  Result<std::uint8_t> GetDeviceId() const;

  Result<std::string> GetCalibrationDate() const;

  Result<int> GetErrorCount() const;

  Result<ScpiError> GetNextError() const;

  ErrorCode ClearErrors() const;

  ErrorCode Reboot() const;

  /* Cells */

  ErrorCode EnableCell(unsigned int cell, bool en) const;

  Result<bool> GetCellEnabled(unsigned int cell) const;

  ErrorCode EnableCellsMasked(unsigned int cells, bool en) const;

  ErrorCode SetCellVoltage(unsigned int cell, float voltage) const;

  ErrorCode SetAllCellVoltage(float voltage) const;

  ErrorCode SetAllCellVoltage(const float* voltages, std::size_t count) const;

  ErrorCode SetAllCellVoltage(std::span<const float> voltages) const;

  ErrorCode SetAllCellVoltage(
      const std::array<float, kCellCount>& voltages) const;

  Result<float> GetCellVoltageTarget(unsigned int cell) const;

  Result<std::array<float, kCellCount>> GetAllCellVoltageTarget() const;

  ErrorCode GetAllCellVoltageTarget(float* voltages, std::size_t count) const;

  ErrorCode GetAllCellVoltageTarget(
      std::array<float, kCellCount>& voltages) const;

  ErrorCode GetAllCellVoltageTarget(std::span<float> voltages) const;

  ErrorCode SetCellSourcing(unsigned int cell, float limit) const;

  ErrorCode SetAllCellSourcing(float limit) const;

  ErrorCode SetAllCellSourcing(const float* limits, std::size_t count) const;

  ErrorCode SetAllCellSourcing(std::span<const float> limits) const;

  ErrorCode SetAllCellSourcing(
      const std::array<float, kCellCount>& limits) const;

  Result<float> GetCellSourcingLimit(unsigned int cell) const;

  Result<std::array<float, kCellCount>> GetAllCellSourcingLimit() const;

  ErrorCode GetAllCellSourcingLimit(float* limits, std::size_t count) const;

  ErrorCode GetAllCellSourcingLimit(
      std::array<float, kCellCount>& limits) const;

  ErrorCode GetAllCellSourcingLimit(std::span<float> limits) const;

  ErrorCode SetCellSinking(unsigned int cell, float limit) const;

  ErrorCode SetAllCellSinking(float limit) const;

  ErrorCode SetAllCellSinking(const float* limits, std::size_t count) const;

  ErrorCode SetAllCellSinking(std::span<const float> limits) const;

  ErrorCode SetAllCellSinking(
      const std::array<float, kCellCount>& limits) const;

  Result<float> GetCellSinkingLimit(unsigned int cell) const;

  Result<std::array<float, kCellCount>> GetAllCellSinkingLimit() const;

  ErrorCode GetAllCellSinkingLimit(float* limits, std::size_t count) const;

  ErrorCode GetAllCellSinkingLimit(std::array<float, kCellCount>& limits) const;

  ErrorCode GetAllCellSinkingLimit(std::span<float> limits) const;

  ErrorCode SetCellFault(unsigned int cell, CellFault fault) const;

  ErrorCode SetAllCellFault(CellFault fault) const;

  ErrorCode SetAllCellFault(const CellFault* faults, std::size_t count) const;

  ErrorCode SetAllCellFault(std::span<const CellFault> faults) const;

  ErrorCode SetAllCellFault(
      const std::array<CellFault, kCellCount>& faults) const;

  Result<CellFault> GetCellFault(unsigned int cell) const;

  Result<std::array<CellFault, kCellCount>> GetAllCellFault() const;

  ErrorCode GetAllCellFault(CellFault* faults, std::size_t count) const;

  ErrorCode GetAllCellFault(std::array<CellFault, kCellCount>& faults) const;

  ErrorCode GetAllCellFault(std::span<CellFault> faults) const;

  ErrorCode SetCellSenseRange(unsigned int cell, CellSenseRange range) const;

  ErrorCode SetAllCellSenseRange(CellSenseRange range) const;

  ErrorCode SetAllCellSenseRange(const CellSenseRange* ranges,
                                 std::size_t count) const;

  ErrorCode SetAllCellSenseRange(std::span<const CellSenseRange> ranges) const;

  ErrorCode SetAllCellSenseRange(
      const std::array<CellSenseRange, kCellCount>& ranges) const;

  Result<CellSenseRange> GetCellSenseRange(unsigned int cell) const;

  Result<std::array<CellSenseRange, kCellCount>> GetAllCellSenseRange() const;

  ErrorCode GetAllCellSenseRange(CellSenseRange* ranges,
                                 std::size_t count) const;

  ErrorCode GetAllCellSenseRange(
      std::array<CellSenseRange, kCellCount>& ranges) const;

  ErrorCode GetAllCellSenseRange(std::span<CellSenseRange> ranges) const;

  Result<float> MeasureCellVoltage(unsigned int cell) const;

  Result<std::array<float, kCellCount>> MeasureAllCellVoltage() const;

  ErrorCode MeasureAllCellVoltage(float* voltages, std::size_t count) const;

  ErrorCode MeasureAllCellVoltage(
      std::array<float, kCellCount>& voltages) const;

  ErrorCode MeasureAllCellVoltage(std::span<float> voltages) const;

  Result<float> MeasureCellCurrent(unsigned int cell) const;

  Result<std::array<float, kCellCount>> MeasureAllCellCurrent() const;

  ErrorCode MeasureAllCellCurrent(float* currents, std::size_t count) const;

  ErrorCode MeasureAllCellCurrent(
      std::array<float, kCellCount>& currents) const;

  ErrorCode MeasureAllCellCurrent(std::span<float> currents) const;

  Result<CellMode> GetCellOperatingMode(unsigned int cell) const;

  Result<std::array<CellMode, kCellCount>> GetAllCellOperatingMode() const;

  ErrorCode GetAllCellOperatingMode(CellMode* modes, std::size_t count) const;

  ErrorCode GetAllCellOperatingMode(
      std::array<CellMode, kCellCount>& modes) const;

  ErrorCode GetAllCellOperatingMode(std::span<CellMode> modes) const;

  /* Aux IO */

  ErrorCode SetAnalogOutput(unsigned int channel, float voltage) const;

  ErrorCode SetAllAnalogOutput(float voltage) const;

  ErrorCode SetAllAnalogOutput(const float* voltages, std::size_t count) const;

  ErrorCode SetAllAnalogOutput(std::span<const float> voltages) const;

  ErrorCode SetAllAnalogOutput(
      const std::array<float, kAnalogOutputCount>& voltages) const;

  Result<float> GetAnalogOutput(unsigned int channel) const;

  Result<std::array<float, kAnalogOutputCount>> GetAllAnalogOutput() const;

  ErrorCode GetAllAnalogOutput(float* voltages, std::size_t count) const;

  ErrorCode GetAllAnalogOutput(
      std::array<float, kAnalogOutputCount>& voltages) const;

  ErrorCode GetAllAnalogOutput(std::span<float> voltages) const;

  ErrorCode SetDigitalOutput(unsigned int channel, bool level) const;

  ErrorCode SetAllDigitalOutput(bool level) const;

  ErrorCode SetAllDigitalOutputMasked(unsigned int channels, bool level) const;

  ErrorCode SetAllDigitalOutput(
      const std::array<bool, kDigitalOutputCount>& levels) const;

  ErrorCode SetAllDigitalOutput(std::span<const bool> levels) const;

  Result<bool> GetDigitalOutput(unsigned int channel) const;

  Result<std::array<bool, kDigitalOutputCount>> GetAllDigitalOutput() const;

  Result<unsigned int> GetAllDigitalOutputMasked() const;

  Result<float> MeasureAnalogInput(unsigned int channel) const;

  Result<std::array<float, kAnalogInputCount>> MeasureAllAnalogInput() const;

  ErrorCode MeasureAllAnalogInput(float* voltages, std::size_t count) const;

  ErrorCode MeasureAllAnalogInput(
      std::array<float, kAnalogInputCount>& voltages) const;

  ErrorCode MeasureAllAnalogInput(std::span<float> voltages) const;

  Result<bool> MeasureDigitalInput(unsigned int channel) const;

  Result<std::array<bool, kDigitalInputCount>> MeasureAllDigitalInput() const;

  Result<unsigned int> MeasureAllDigitalInputMasked() const;

 private:
  std::shared_ptr<drivers::CommDriver> driver_;

  ErrorCode Send(std::string_view buf) const;

  Result<std::string> SendAndRecv(std::string_view buf) const;
};

}  // namespace bci::abs

#endif /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SCPICLIENT_H */
