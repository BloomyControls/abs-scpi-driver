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

  ErrorCode EnableCell(unsigned int cell, bool en) const;

  Result<float> MeasureCellVoltage(unsigned int cell) const;

 private:
  std::shared_ptr<drivers::CommDriver> driver_;

  Result<std::string> WriteAndRead(std::string_view buf) const;
};

}  // namespace bci::abs

#endif  /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SCPICLIENT_H */
