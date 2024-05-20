/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

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

/**
 * @brief Generic SCPI client for communicating with the Bloomy Controls ABS.
 *
 * This class implements all the SCPI commands and queries, as well as parsing
 * and returning the results. It works with any CommDriver.
 *
 * Example usage (error handling omitted):
 * @code{.cpp}
 * auto driver{std::make_unique<bci::abs::drivers::UdpDriver>()};
 * driver->Open("192.168.1.70");
 * ScpiClient client{driver};
 * client.SetCellVoltage(0, 1.35f);
 * if (auto v = client.MeasureCellVoltage(0)) {
 *   std::cout << "cell 1 voltage: " << *v << "\n";
 * }
 * @endcode
 */
class ScpiClient {
 public:
  /// Default CTOR.
  ScpiClient() = default;

  /**
   * @brief Initialize a ScpiClient with a driver handle.
   *
   * @param[in] driver pointer to a comm driver
   */
  explicit ScpiClient(std::shared_ptr<drivers::CommDriver> driver) noexcept;

  /**
   * @brief Move construct from another ScpiClient.
   *
   * @param[in] other ScpiClient to move from
   */
  ScpiClient(ScpiClient&& other) noexcept;

  ScpiClient(const ScpiClient&) = delete;

  /**
   * @brief Move assign from another ScpiClient.
   *
   * @param[in] rhs ScpiClient to move from
   *
   * @return Reference to self.
   */
  ScpiClient& operator=(ScpiClient&& rhs) noexcept;

  ScpiClient& operator=(const ScpiClient&) = delete;

  /// DTOR.
  ~ScpiClient() = default;

  /**
   * @return Pointer to the comm driver.
   */
  std::shared_ptr<drivers::CommDriver> GetDriver() noexcept;

  /**
   * @return Pointer to the comm driver.
   */
  std::shared_ptr<const drivers::CommDriver> GetDriver() const noexcept;

  /**
   * @brief Set or replace the comm driver for the client.
   *
   * @param[in] driver new driver to use
   */
  void SetDriver(std::shared_ptr<drivers::CommDriver> driver) noexcept;

  /**
   * @brief Change the targeted device ID. This is currently only meaningful for
   * RS-485.
   *
   * @param[in] id the serial ID in the range 0-255, or 256+ to target all
   * devices on a bus
   */
  void SetTargetDeviceID(unsigned int id);

  /**
   * @return Targeted device ID.
   */
  unsigned int GetTargetDeviceID() const;

  /* System */

  /**
   * @brief Query general information about the unit.
   *
   * @return Result containing a DeviceInfo structure or an error code.
   */
  Result<DeviceInfo> GetDeviceInfo() const;

  /**
   * @brief Query the device's serial ID.
   *
   * @return Result containing the device ID or an error code.
   */
  Result<std::uint8_t> GetDeviceId() const;

  /**
   * @brief Query the device's IP address and subnet mask.
   *
   * @return Result containing the IP address or an error code.
   */
  Result<EthernetConfig> GetIPAddress() const;

  /**
   * @brief Set the device's IP address and subnet mask.
   *
   * @note The addresses must be IPv4 dot-decimal notation, e.g. 192.168.1.70.
   *
   * @note You will need to reopen the connection if using Ethernet.
   *
   * @param[in] ip desired IP address for the unit
   * @param[in] netmask desired subnet mask for the unit
   *
   * @return An error code.
   */
  ErrorCode SetIPAddress(std::string_view ip, std::string_view netmask) const;

  /**
   * @brief Query the device's calibration date.
   *
   * @return Result containing the calibration date or an error code.
   */
  Result<std::string> GetCalibrationDate() const;

  /**
   * @brief Query the number of errors in the device's error queue.
   *
   * @return Result containing the error count or an error code.
   */
  Result<int> GetErrorCount() const;

  /**
   * @brief Pop the next error from the SCPI error queue.
   *
   * @return Result containing the error or an error code.
   */
  Result<ScpiError> GetNextError() const;

  /**
   * @brief Clear the device's error queue.
   *
   * @return An error code.
   */
  ErrorCode ClearErrors() const;

  /**
   * @brief Get the alarms raised on the unit.
   *
   * @note Use the bci::abs::alarms constants to mask/test the states of alarms.
   *
   * @return Result containing the alarm bitmask or an error code.
   */
  Result<std::uint32_t> GetAlarms() const;

  /**
   * @brief Assert the software interlock.
   *
   * Use ClearRecoverableAlarms() to clear the interlock.
   *
   * @return An error code.
   */
  ErrorCode AssertSoftwareInterlock() const;

  /**
   * @brief Clear any recoverable alarms currently raised on the unit.
   *
   * This can be used to clear the software interlock.
   *
   * @return An error code.
   */
  ErrorCode ClearRecoverableAlarms() const;

  /**
   * @brief Reboot the unit.
   *
   * @note You will need to reopen the connection if using TCP.
   *
   * @return An error code.
   */
  ErrorCode Reboot() const;

  /* Cells */

  /**
   * @brief Enable or disable a single cell.
   *
   * @param[in] cell the cell index (0 is the first cell)
   * @param[in] en whether to enable or disable the cell
   *
   * @return An error code.
   */
  ErrorCode EnableCell(unsigned int cell, bool en) const;

  /**
   * @brief Enable or disable multiple cells using a bitmask.
   *
   * The mask is 1 bit per cell, with the first cell being bit 0.
   *
   * @param[in] cells a mask of bits selecting the cells to target
   * @param[in] en whether to enable or disable the selected cells
   *
   * @return An error code.
   */
  ErrorCode EnableCellsMasked(unsigned int cells, bool en) const;

  /**
   * @brief Query the enable state of a cell.
   *
   * @param[in] cell target cell index (0 is the first cell)
   *
   * @return Result containing whether the cell is enabled or an error code.
   */
  Result<bool> GetCellEnabled(unsigned int cell) const;

  /**
   * @brief Query the enable states of all cells.
   *
   * @return Result containing an array of cell enable states or an error code.
   */
  Result<std::array<bool, kCellCount>> GetAllCellsEnabled() const;

  /**
   * @brief Query the enable states of all cells as a bitmask.
   *
   * @return Result containing a bitmask of enabled cells or an error code.
   */
  Result<unsigned int> GetAllCellsEnabledMasked() const;

  /**
   * @brief Set a single cell's target voltage.
   *
   * @param[in] cell target cell index
   * @param[in] voltage target voltage
   *
   * @return An error code.
   */
  ErrorCode SetCellVoltage(unsigned int cell, float voltage) const;

  /**
   * @brief Set all cells' voltages to the same value.
   *
   * @param[in] voltage target voltage
   *
   * @return An error code.
   */
  ErrorCode SetAllCellVoltage(float voltage) const;

  /**
   * @brief Set all cells' voltages.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[in] voltages pointer to an array of cell voltages
   * @param[in] count how many cells to set (how many entries are in the
   * voltages array)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellVoltage(const float* voltages, std::size_t count) const;

  /**
   * @brief Set all cells' voltages.
   *
   * @param[in] voltages target voltages, one per cell (there may not be more
   * entries than cells)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellVoltage(std::span<const float> voltages) const;

  /**
   * @brief Set all cells' voltages.
   *
   * @param[in] voltages target voltages, one per cell
   *
   * @return An error code.
   */
  ErrorCode SetAllCellVoltage(
      const std::array<float, kCellCount>& voltages) const;

  /**
   * @brief Query a single cell's voltage set point.
   *
   * @param[in] cell target cell index
   *
   * @return Result containing the cell's voltage set point or an error code.
   */
  Result<float> GetCellVoltageTarget(unsigned int cell) const;

  /**
   * @brief Query all cells' voltage set points.
   *
   * @return Result containing an array of voltages or an error code.
   */
  Result<std::array<float, kCellCount>> GetAllCellVoltageTarget() const;

  /**
   * @brief Query all cells' voltage set points.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[out] voltages pointer to an array to contain the returned voltages
   * @param[in] count number of elements in the voltages array (must not be more
   * than the total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellVoltageTarget(float* voltages, std::size_t count) const;

  /**
   * @brief Query all cells' voltage set points.
   *
   * @param[out] voltages array to store the returned voltages
   *
   * @return An error code.
   */
  ErrorCode GetAllCellVoltageTarget(
      std::array<float, kCellCount>& voltages) const;

  /**
   * @brief Query all cells' voltage set points.
   *
   * @param[out] voltages array to store the returned voltages (must not be
   * longer than the total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellVoltageTarget(std::span<float> voltages) const;

  /**
   * @brief Set a single cell's sourcing current limit.
   *
   * @param[in] cell target cell index
   * @param[in] limit desired sourcing current limit
   *
   * @return An error code.
   */
  ErrorCode SetCellSourcing(unsigned int cell, float limit) const;

  /**
   * @brief Set all cells' sourcing current limits to the same value.
   *
   * @param[in] limit desired sourcing current limit
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSourcing(float limit) const;

  /**
   * @brief Set all cells' sourcing current limits.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[in] limits array of sourcing current limits
   * @param[in] count number of cells to set (must not be greater than the total
   * cell count)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSourcing(const float* limits, std::size_t count) const;

  /**
   * @brief Set all cells' sourcing current limits.
   *
   * @param[in] limits array of sourcing current limits (must not be longer than
   * the total cell count)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSourcing(std::span<const float> limits) const;

  /**
   * @brief Set all cells' sourcing current limits.
   *
   * @param[in] limits array of sourcing current limits
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSourcing(
      const std::array<float, kCellCount>& limits) const;

  /**
   * @brief Query a single cell's sourcing current limit.
   *
   * @param[in] cell target cell index
   *
   * @return Result containing the cell's sourcing current limit or an error
   * code.
   */
  Result<float> GetCellSourcingLimit(unsigned int cell) const;

  /**
   * @brief Query all cells' sourcing current limits.
   *
   * @return Result containing an array of the cells' sourcing limits or an
   * error code.
   */
  Result<std::array<float, kCellCount>> GetAllCellSourcingLimit() const;

  /**
   * @brief Query all cells' sourcing current limits.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[out] limits array to store the sourcing current limits
   * @param[in] count size of the limits array (must not be greater than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSourcingLimit(float* limits, std::size_t count) const;

  /**
   * @brief Query all cells' sourcing current limits.
   *
   * @param[out] limits array to store the sourcing current limits
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSourcingLimit(
      std::array<float, kCellCount>& limits) const;

  /**
   * @brief Query all cells' sourcing current limits.
   *
   * @param[out] limits array to store the sourcing current limits (must not be
   * longer than the total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSourcingLimit(std::span<float> limits) const;

  /**
   * @brief Set a single cell's sinking current limit.
   *
   * @param[in] cell target cell index
   * @param[in] limit desired sinking current limit (may be positive or
   * negative)
   *
   * @return An error code.
   */
  ErrorCode SetCellSinking(unsigned int cell, float limit) const;

  /**
   * @brief Set all cells' sinking current limits to the same value.
   *
   * @param[in] limit desired sinking current limit (may be positive or
   * negative)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSinking(float limit) const;

  /**
   * @brief Set all cells' sinking current limits.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[in] limits array of sinking current limits (may be positive or
   * negative)
   * @param[in] count number of cells to set (must not be greater than the total
   * cell count)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSinking(const float* limits, std::size_t count) const;

  /**
   * @brief Set all cells' sinking current limits.
   *
   * @param[in] limits array of sinking current limits (must not be longer than
   * the total cell count)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSinking(std::span<const float> limits) const;

  /**
   * @brief Set all cells' sinking current limits.
   *
   * @param[in] limits array of sinking current limits
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSinking(
      const std::array<float, kCellCount>& limits) const;

  /**
   * @brief Query a single cell's sinking current limit.
   *
   * @param[in] cell target cell index
   *
   * @return Result containing the cell's sinking current limit or an error
   * code.
   */
  Result<float> GetCellSinkingLimit(unsigned int cell) const;

  /**
   * @brief Query all cells' sinking current limits.
   *
   * @return Result containing an array of the cells' sinking limits or an
   * error code.
   */
  Result<std::array<float, kCellCount>> GetAllCellSinkingLimit() const;

  /**
   * @brief Query all cells' sinking current limits.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[out] limits array to store the sinking current limits
   * @param[in] count size of the limits array (must not be greater than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSinkingLimit(float* limits, std::size_t count) const;

  /**
   * @brief Query all cells' sinking current limits.
   *
   * @param[out] limits array to store the sinking current limits
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSinkingLimit(std::array<float, kCellCount>& limits) const;

  /**
   * @brief Query all cells' sinking current limits.
   *
   * @param[out] limits array to store the sinking current limits (must not be
   * longer than the total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSinkingLimit(std::span<float> limits) const;

  /**
   * @brief Set a single cell's faulting state.
   *
   * @param[in] cell target cell index
   * @param[in] fault desired cell fault
   *
   * @return An error code.
   */
  ErrorCode SetCellFault(unsigned int cell, CellFault fault) const;

  /**
   * @brief Set all cells' faulting states to the same value.
   *
   * @param[in] fault desired cell fault
   *
   * @return An error code.
   */
  ErrorCode SetAllCellFault(CellFault fault) const;

  /**
   * @brief Set all cells' faulting states.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[in] faults an array of cell fault states
   * @param[in] count the length of the array (must not be greater than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellFault(const CellFault* faults, std::size_t count) const;

  /**
   * @brief Set all cells' faulting states.
   *
   * @param[in] faults an array of fault states (must not be longer than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellFault(std::span<const CellFault> faults) const;

  /**
   * @brief Set all cells' faulting states.
   *
   * @param[in] faults an array of fault states
   *
   * @return An error code.
   */
  ErrorCode SetAllCellFault(
      const std::array<CellFault, kCellCount>& faults) const;

  /**
   * @brief Query a single cell's faulting state.
   *
   * @param[in] cell target cell index
   *
   * @return Result containing the cell's fault state or an error code.
   */
  Result<CellFault> GetCellFault(unsigned int cell) const;

  /**
   * @brief Query all cells' faulting states.
   *
   * @return Result containing an array of cell fault states or an error code.
   */
  Result<std::array<CellFault, kCellCount>> GetAllCellFault() const;

  /**
   * @brief Query all cells' faulting states.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[out] faults an array of cell fault states
   * @param[in] count the length of the array (must not be greater than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellFault(CellFault* faults, std::size_t count) const;

  /**
   * @brief Query all cells' faulting states.
   *
   * @param[out] faults an array of cell fault states
   *
   * @return An error code.
   */
  ErrorCode GetAllCellFault(std::array<CellFault, kCellCount>& faults) const;

  /**
   * @brief Query all cells' faulting states.
   *
   * @param[out] faults an array of cell fault states (must not be longer than
   * the total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellFault(std::span<CellFault> faults) const;

  /**
   * @brief Set a single cell's current sense range.
   *
   * @param[in] cell target cell index
   * @param[in] range desired sense range
   *
   * @return An error code.
   */
  ErrorCode SetCellSenseRange(unsigned int cell, CellSenseRange range) const;

  /**
   * @brief Set all cells' current sense ranges to the same value.
   *
   * @param[in] range desired sense range
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSenseRange(CellSenseRange range) const;

  /**
   * @brief Set all cells' current sense ranges.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[in] ranges an array of sense ranges
   * @param[in] count the length of the array (must not be greater than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSenseRange(const CellSenseRange* ranges,
                                 std::size_t count) const;

  /**
   * @brief Set all cells' current sense ranges.
   *
   * @param[in] ranges an array of sense ranges (must not be longer than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSenseRange(std::span<const CellSenseRange> ranges) const;

  /**
   * @brief Set all cells' current sense ranges.
   *
   * @param[in] ranges an array of sense ranges
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSenseRange(
      const std::array<CellSenseRange, kCellCount>& ranges) const;

  /**
   * @brief Query a single cell's current sense range.
   *
   * @param[in] cell target cell index
   *
   * @return Result containing the cell's sense range or an error code.
   */
  Result<CellSenseRange> GetCellSenseRange(unsigned int cell) const;

  /**
   * @brief Query all cells' current sense ranges.
   *
   * @return Result containing an array of cell sense ranges or an error code.
   */
  Result<std::array<CellSenseRange, kCellCount>> GetAllCellSenseRange() const;

  /**
   * @brief Query all cells' current sense ranges.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[out] ranges array to store the sense ranges
   * @param[in] count length of the array (must not be greater than the total
   * cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSenseRange(CellSenseRange* ranges,
                                 std::size_t count) const;

  /**
   * @brief Query all cells' current sense ranges.
   *
   * @param[out] ranges an array of current sense ranges
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSenseRange(
      std::array<CellSenseRange, kCellCount>& ranges) const;

  /**
   * @brief Query all cells' current sense ranges.
   *
   * @param[out] ranges an array of sense ranges (must not be longer than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSenseRange(std::span<CellSenseRange> ranges) const;

  /**
   * @brief Set the cell precision mode.
   *
   * @param[in] mode desired cell precision mode
   *
   * @return An error code.
   */
  ErrorCode SetCellPrecisionMode(CellPrecisionMode mode) const;

  /**
   * @brief Query the cell precision mode.
   *
   * @return Result containing the precision mode or an error code.
   */
  Result<CellPrecisionMode> GetCellPrecisionMode() const;

  /**
   * @brief Measure a single cell's voltage.
   *
   * @param[in] cell target cell index
   *
   * @return Result containing the cell voltage or an error code.
   */
  Result<float> MeasureCellVoltage(unsigned int cell) const;

  /**
   * @brief Measure all cells' voltages.
   *
   * @return Result containing an array of cell voltages or an error code.
   */
  Result<std::array<float, kCellCount>> MeasureAllCellVoltage() const;

  /**
   * @brief Measure all cells' voltages.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[out] voltages array to store the measured voltages
   * @param[in] count length of the array (must not be greater than the total
   * cell count)
   *
   * @return An error code.
   */
  ErrorCode MeasureAllCellVoltage(float* voltages, std::size_t count) const;

  /**
   * @brief Measure all cells' voltages.
   *
   * @param[out] voltages an array of cell voltages
   *
   * @return An error code.
   */
  ErrorCode MeasureAllCellVoltage(
      std::array<float, kCellCount>& voltages) const;

  /**
   * @brief Measure all cells' voltages.
   *
   * @param[out] voltages an array of cell voltages (must not be longer than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode MeasureAllCellVoltage(std::span<float> voltages) const;

  /**
   * @brief Measure a single cell's current.
   *
   * @param[in] cell target cell index
   *
   * @return Result containing the cell current or an error code.
   */
  Result<float> MeasureCellCurrent(unsigned int cell) const;

  /**
   * @brief Measure all cells' currents.
   *
   * @return Result containing an array of cell currents or an error code.
   */
  Result<std::array<float, kCellCount>> MeasureAllCellCurrent() const;

  /**
   * @brief Measure all cells' currents.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[out] currents array to store the measured currents
   * @param[in] count length of the array (must not be greater than the total
   * cell count)
   *
   * @return An error code.
   */
  ErrorCode MeasureAllCellCurrent(float* currents, std::size_t count) const;

  /**
   * @brief Measure all cells' currents.
   *
   * @param[out] currents an array of cell currents
   *
   * @return An error code.
   */
  ErrorCode MeasureAllCellCurrent(
      std::array<float, kCellCount>& currents) const;

  /**
   * @brief Measure all cells' currents.
   *
   * @param[out] currents an array of cell currents (must not be longer than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode MeasureAllCellCurrent(std::span<float> currents) const;

  /**
   * @brief Query a single cell's operating mode.
   *
   * @param[in] cell target cell index
   *
   * @return Result containing the cell operating mode or an error code.
   */
  Result<CellMode> GetCellOperatingMode(unsigned int cell) const;

  /**
   * @brief Query all cells' operating modes.
   *
   * @return Result containing an array of cell operating modes or an error
   * code.
   */
  Result<std::array<CellMode, kCellCount>> GetAllCellOperatingMode() const;

  /**
   * @brief Query all cells' operating modes.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[out] modes array to store the operating modes
   * @param[in] count length of the array (must not be greater than the total
   * cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellOperatingMode(CellMode* modes, std::size_t count) const;

  /**
   * @brief Query all cells' operating modes.
   *
   * @param[out] modes an array of cell operating modes
   *
   * @return An error code.
   */
  ErrorCode GetAllCellOperatingMode(
      std::array<CellMode, kCellCount>& modes) const;

  /**
   * @brief Query all cells' operating modes.
   *
   * @param[out] modes an array of cell operating modes (must not be longer than
   * the total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellOperatingMode(std::span<CellMode> modes) const;

  /* Aux IO */

  /**
   * @brief Set a single analog output.
   *
   * @param[in] channel target channel index
   * @param[in] voltage desired voltage
   *
   * @return An error code.
   */
  ErrorCode SetAnalogOutput(unsigned int channel, float voltage) const;

  /**
   * @brief Set all analog outputs to the same value.
   *
   * @param[in] voltage desired voltage
   *
   * @return An error code.
   */
  ErrorCode SetAllAnalogOutput(float voltage) const;

  /**
   * @brief Set all analog outputs.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[in] voltages array of voltages
   * @param[in] count length of the array (must not be greater than the total
   * channel count)
   *
   * @return An error code.
   */
  ErrorCode SetAllAnalogOutput(const float* voltages, std::size_t count) const;

  /**
   * @brief Set all analog outputs.
   *
   * @param[in] voltages array of voltages (must not be longer than the total
   * channel count)
   *
   * @return An error code.
   */
  ErrorCode SetAllAnalogOutput(std::span<const float> voltages) const;

  /**
   * @brief Set all analog outputs.
   *
   * @param[in] voltages array of voltages
   *
   * @return An error code.
   */
  ErrorCode SetAllAnalogOutput(
      const std::array<float, kAnalogOutputCount>& voltages) const;

  /**
   * @brief Query a single analog output's voltage.
   *
   * @param[in] channel target channel index
   *
   * @return Result containing the voltage or an error code.
   */
  Result<float> GetAnalogOutput(unsigned int channel) const;

  /**
   * @brief Query all analog outputs' voltages.
   *
   * @return Result containing an array of voltages or an error code.
   */
  Result<std::array<float, kAnalogOutputCount>> GetAllAnalogOutput() const;

  /**
   * @brief Query all analog outputs' voltages.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[out] voltages array to store the voltages
   * @param[in] count the length of the array (must not be greater than the
   * total channel count)
   *
   * @return An error code.
   */
  ErrorCode GetAllAnalogOutput(float* voltages, std::size_t count) const;

  /**
   * @brief Query all analog outputs' voltages.
   *
   * @param[out] voltages array of voltages
   *
   * @return An error code.
   */
  ErrorCode GetAllAnalogOutput(
      std::array<float, kAnalogOutputCount>& voltages) const;

  /**
   * @brief Query all analog outputs' voltages.
   *
   * @param[out] voltages array of voltages (must not be longer than the total
   * channel count)
   *
   * @return An error code.
   */
  ErrorCode GetAllAnalogOutput(std::span<float> voltages) const;

  /**
   * @brief Set a single digital output.
   *
   * @param[in] channel target channel index
   * @param[in] level desired output state
   *
   * @return An error code.
   */
  ErrorCode SetDigitalOutput(unsigned int channel, bool level) const;

  /**
   * @brief Set all digital outputs to the same state.
   *
   * @param[in] level desired output state
   *
   * @return An error code.
   */
  ErrorCode SetAllDigitalOutput(bool level) const;

  /**
   * @brief Set all digital outputs.
   *
   * @param[in] channels a bitmask of channels to control, 1 bit per channel
   * @param[in] level the desired output state
   *
   * @return An error code.
   */
  ErrorCode SetAllDigitalOutputMasked(unsigned int channels, bool level) const;

  /**
   * @brief Set all digital outputs.
   *
   * @param[in] levels an array of output states
   *
   * @return An error code.
   */
  ErrorCode SetAllDigitalOutput(
      const std::array<bool, kDigitalOutputCount>& levels) const;

  /**
   * @brief Set all digital outputs.
   *
   * @param[in] levels an array of output states (must not be longer than the
   * total channel count)
   *
   * @return An error code.
   */
  ErrorCode SetAllDigitalOutput(std::span<const bool> levels) const;

  /**
   * @brief Query a digital output's state.
   *
   * @param[in] channel target channel index
   *
   * @return Result containing the output state or an error code.
   */
  Result<bool> GetDigitalOutput(unsigned int channel) const;

  /**
   * @brief Query all digital outputs' states.
   *
   * @return Result containing an array of output states or an error code.
   */
  Result<std::array<bool, kDigitalOutputCount>> GetAllDigitalOutput() const;

  /**
   * @brief Query all digital outputs' states.
   *
   * @return Result containing a bitmask of output states or an error code.
   */
  Result<unsigned int> GetAllDigitalOutputMasked() const;

  /**
   * @brief Measure a single analog input.
   *
   * @param[in] channel target channel index
   *
   * @return Result containing the analog input voltage or an error code.
   */
  Result<float> MeasureAnalogInput(unsigned int channel) const;

  /**
   * @brief Measure all analog inputs.
   *
   * @return Result containing an array of voltages or an error code.
   */
  Result<std::array<float, kAnalogInputCount>> MeasureAllAnalogInput() const;

  /**
   * @brief Measure all analog inputs.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[out] voltages array to store the measured voltages
   * @param[in] count length of the array (must not be greater than the total
   * channel count)
   *
   * @return An error code.
   */
  ErrorCode MeasureAllAnalogInput(float* voltages, std::size_t count) const;

  /**
   * @brief Measure all analog inputs.
   *
   * @param[out] voltages an array of analog input voltages
   *
   * @return An error code.
   */
  ErrorCode MeasureAllAnalogInput(
      std::array<float, kAnalogInputCount>& voltages) const;

  /**
   * @brief Measure all analog inputs.
   *
   * @param[out] voltages an array of voltages (must not be longer than the
   * total channel count)
   *
   * @return An error code.
   */
  ErrorCode MeasureAllAnalogInput(std::span<float> voltages) const;

  /**
   * @brief Measure a single digital input.
   *
   * @param[in] channel target channel count
   *
   * @return Result containing the input state or an error code.
   */
  Result<bool> MeasureDigitalInput(unsigned int channel) const;

  /**
   * @brief Measure all digital inputs.
   *
   * @return Result containing an array of input states or an error code.
   */
  Result<std::array<bool, kDigitalInputCount>> MeasureAllDigitalInput() const;

  /**
   * @brief Measure all digital inputs.
   *
   * @return Result containing a bitmask of input states or an error code.
   */
  Result<unsigned int> MeasureAllDigitalInputMasked() const;

 private:
  /// Driver handle.
  std::shared_ptr<drivers::CommDriver> driver_;

  /**
   * @brief Send a message to the ABS. Checks for driver validity.
   *
   * @param[in] buf data to send
   *
   * @return An error code.
   */
  ErrorCode Send(std::string_view buf) const;

  /**
   * @brief Send a message to the ABS and wait for a response. Checks for driver
   * validity.
   *
   * @param[in] buf data to send
   *
   * @return Result containing the response or an error code.
   */
  Result<std::string> SendAndRecv(std::string_view buf) const;
};

}  // namespace bci::abs

#endif /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SCPICLIENT_H */
