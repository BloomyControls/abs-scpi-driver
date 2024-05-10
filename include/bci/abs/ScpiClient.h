#ifndef ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SCPICLIENT_H
#define ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SCPICLIENT_H

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include "CommDriver.h"
#include "CommonTypes.h"

namespace bci::abs {

class ScpiClient {
 public:
  ScpiClient(std::shared_ptr<drivers::CommDriver> driver);

  virtual ~ScpiClient();

  /* System */

  Result<DeviceInfo> GetDeviceInfo() const;

  Result<std::uint8_t> GetDeviceId() const;

  Result<int> GetErrorCount() const;

  ErrorCode Reboot() const;

  /* Cells */

  ErrorCode EnableCell(unsigned int cell, bool en) const;

  Result<bool> GetCellEnabled(unsigned int cell) const;

  ErrorCode EnableCellsMasked(unsigned int cells, bool en) const;

  ErrorCode SetCellVoltage(unsigned int cell, float voltage) const;

  Result<float> GetCellVoltageTarget(unsigned int cell) const;

  ErrorCode SetCellSourcing(unsigned int cell, float limit) const;

  Result<float> GetCellSourcingLimit(unsigned int cell) const;

  ErrorCode SetCellSinking(unsigned int cell, float limit) const;

  Result<float> GetCellSinkingLimit(unsigned int cell) const;

  ErrorCode SetCellFault(unsigned int cell, CellFault fault) const;

  Result<CellFault> GetCellFault(unsigned int cell) const;

  ErrorCode SetCellSenseRange(unsigned int cell, CellSenseRange range) const;

  Result<CellSenseRange> GetCellSenseRange(unsigned int cell) const;

  Result<float> MeasureCellVoltage(unsigned int cell) const;

  Result<float> MeasureCellCurrent(unsigned int cell) const;

  /* Aux IO */

  ErrorCode SetAnalogOutput(unsigned int channel, float voltage) const;

  Result<float> GetAnalogOutput(unsigned int channel) const;

  ErrorCode SetDigitalOutput(unsigned int channel, bool level) const;

  Result<bool> GetDigitalOutput(unsigned int channel) const;

  Result<float> MeasureAnalogInput(unsigned int channel) const;

  Result<bool> MeasureDigitalInput(unsigned int channel) const;

 private:
  std::shared_ptr<drivers::CommDriver> driver_;

  Result<std::string> WriteAndRead(std::string_view buf) const;
};

}  // namespace bci::abs

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SCPICLIENT_H */
