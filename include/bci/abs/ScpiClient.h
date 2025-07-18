/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

/**
 * @file
 * Primary SCPI client declaration.
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

// These comments make sure doxygen generates docs properly.
/**
 * @namespace bci
 * @brief Bloomy Controls root namespace.
 */
/**
 * @namespace bci::abs
 * @brief Contains ABS-related code.
 */

namespace bci::abs {

/**
 * @brief Generic SCPI client for communicating with the Bloomy Controls ABS.
 *
 * This class implements all the SCPI commands and queries, as well as parsing
 * and returning the results. It works with any CommDriver.
 *
 * Example usage (error handling omitted):
 * @code{.cpp}
 * auto driver{std::make_shared<bci::abs::drivers::UdpDriver>()};
 * driver->Open("192.168.1.100");
 * ScpiClient client{driver};
 * client.SetCellVoltage(0, 1.35f);
 * if (auto v = client.MeasureCellVoltage(0)) {
 *   std::cout << "cell 1 voltage: " << *v << "\n";
 * }
 * @endcode
 */
class ScpiClient {
 public:
  /**
   * @brief Get the library version as a decimal integer. For example, version
   * 1.3.2 would return 10302.
   *
   * @since v1.1.0
   *
   * @return Library version.
   */
  static unsigned int Version() noexcept;

  /// Default CTOR.
  ScpiClient() noexcept;

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
   * @brief Set the read timeout for the client. In most cases, this is
   * unnecessary. The default is 150ms.
   *
   * @param[in] timeout_ms new read timeout in milliseconds
   *
   * @return The previous timeout value.
   */
  unsigned int SetReadTimeout(unsigned int timeout_ms) noexcept;

  /**
   * @brief Change the targeted device ID. This is currently only meaningful for
   * RS-485.
   *
   * @param[in] id the serial ID in the range 0-31, or 32+ to target all
   * devices on a bus
   *
   * @return An error code.
   */
  ErrorCode SetTargetDeviceID(unsigned int id);

  /**
   * @return Result containing the targeted device ID or an error code.
   */
  Result<unsigned int> GetTargetDeviceID() const;

  /**
   * @name System Control
   */
  ///@{

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
   * @note The addresses must be IPv4 dot-decimal notation, e.g. 192.168.1.100.
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
   * @brief Get the system interlock state. When in interlock, the unit will be
   * put into its PoR state and cannot be controlled until the interlock is
   * lifted.
   *
   * @return Result containing the interlock state or an error code.
   */
  Result<bool> GetInterlockState() const;

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

  ///@}

  /**
   * @name Cell Control
   */
  ///@{

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
  ErrorCode SetAllCellVoltages(float voltage) const;

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
  ErrorCode SetAllCellVoltages(const float* voltages, std::size_t count) const;

  /**
   * @brief Set all cells' voltages.
   *
   * @param[in] voltages target voltages, one per cell (there may not be more
   * entries than cells)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellVoltages(std::span<const float> voltages) const;

  /**
   * @brief Set all cells' voltages.
   *
   * @param[in] voltages target voltages, one per cell
   *
   * @return An error code.
   */
  ErrorCode SetAllCellVoltages(
      const std::array<float, kCellCount>& voltages) const;

  /**
   * @brief Set multiple cells' voltages to the same value.
   *
   * @param[in] cells bitmask of cells to target, 1 bit per cell
   * @param[in] voltage target voltage
   *
   * @return An error code.
   */
  ErrorCode SetMultipleCellVoltages(unsigned int cells, float voltage) const;

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
  Result<std::array<float, kCellCount>> GetAllCellVoltageTargets() const;

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
  ErrorCode GetAllCellVoltageTargets(float* voltages, std::size_t count) const;

  /**
   * @brief Query all cells' voltage set points.
   *
   * @param[out] voltages array to store the returned voltages
   *
   * @return An error code.
   */
  ErrorCode GetAllCellVoltageTargets(
      std::array<float, kCellCount>& voltages) const;

  /**
   * @brief Query all cells' voltage set points.
   *
   * @param[out] voltages array to store the returned voltages (must not be
   * longer than the total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellVoltageTargets(std::span<float> voltages) const;

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
   * @brief Set multiple cells' sourcing current limits to the same value.
   *
   * @param[in] cells bitmask of cells to target, 1 bit per cell
   * @param[in] limit target sourcing limit
   *
   * @return An error code.
   */
  ErrorCode SetMultipleCellSourcing(unsigned int cells, float limit) const;

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
  Result<std::array<float, kCellCount>> GetAllCellSourcingLimits() const;

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
  ErrorCode GetAllCellSourcingLimits(float* limits, std::size_t count) const;

  /**
   * @brief Query all cells' sourcing current limits.
   *
   * @param[out] limits array to store the sourcing current limits
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSourcingLimits(
      std::array<float, kCellCount>& limits) const;

  /**
   * @brief Query all cells' sourcing current limits.
   *
   * @param[out] limits array to store the sourcing current limits (must not be
   * longer than the total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSourcingLimits(std::span<float> limits) const;

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
   * @brief Set multiple cells' sinking current limits to the same value.
   *
   * @param[in] cells bitmask of cells to target, 1 bit per cell
   * @param[in] limit target sinking limit
   *
   * @return An error code.
   */
  ErrorCode SetMultipleCellSinking(unsigned int cells, float limit) const;

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
  Result<std::array<float, kCellCount>> GetAllCellSinkingLimits() const;

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
  ErrorCode GetAllCellSinkingLimits(float* limits, std::size_t count) const;

  /**
   * @brief Query all cells' sinking current limits.
   *
   * @param[out] limits array to store the sinking current limits
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSinkingLimits(
      std::array<float, kCellCount>& limits) const;

  /**
   * @brief Query all cells' sinking current limits.
   *
   * @param[out] limits array to store the sinking current limits (must not be
   * longer than the total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSinkingLimits(std::span<float> limits) const;

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
  ErrorCode SetAllCellFaults(CellFault fault) const;

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
  ErrorCode SetAllCellFaults(const CellFault* faults, std::size_t count) const;

  /**
   * @brief Set all cells' faulting states.
   *
   * @param[in] faults an array of fault states (must not be longer than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellFaults(std::span<const CellFault> faults) const;

  /**
   * @brief Set all cells' faulting states.
   *
   * @param[in] faults an array of fault states
   *
   * @return An error code.
   */
  ErrorCode SetAllCellFaults(
      const std::array<CellFault, kCellCount>& faults) const;

  /**
   * @brief Set multiple cells to the same fault state.
   *
   * @param[in] cells bitmask of cells to target, one bit per cell
   * @param[in] fault target fault state
   *
   * @return An error code.
   */
  ErrorCode SetMultipleCellFaults(unsigned int cells, CellFault fault) const;

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
  Result<std::array<CellFault, kCellCount>> GetAllCellFaults() const;

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
  ErrorCode GetAllCellFaults(CellFault* faults, std::size_t count) const;

  /**
   * @brief Query all cells' faulting states.
   *
   * @param[out] faults an array of cell fault states
   *
   * @return An error code.
   */
  ErrorCode GetAllCellFaults(std::array<CellFault, kCellCount>& faults) const;

  /**
   * @brief Query all cells' faulting states.
   *
   * @param[out] faults an array of cell fault states (must not be longer than
   * the total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellFaults(std::span<CellFault> faults) const;

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
  ErrorCode SetAllCellSenseRanges(CellSenseRange range) const;

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
  ErrorCode SetAllCellSenseRanges(const CellSenseRange* ranges,
                                  std::size_t count) const;

  /**
   * @brief Set all cells' current sense ranges.
   *
   * @param[in] ranges an array of sense ranges (must not be longer than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSenseRanges(std::span<const CellSenseRange> ranges) const;

  /**
   * @brief Set all cells' current sense ranges.
   *
   * @param[in] ranges an array of sense ranges
   *
   * @return An error code.
   */
  ErrorCode SetAllCellSenseRanges(
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
  Result<std::array<CellSenseRange, kCellCount>> GetAllCellSenseRanges() const;

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
  ErrorCode GetAllCellSenseRanges(CellSenseRange* ranges,
                                  std::size_t count) const;

  /**
   * @brief Query all cells' current sense ranges.
   *
   * @param[out] ranges an array of current sense ranges
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSenseRanges(
      std::array<CellSenseRange, kCellCount>& ranges) const;

  /**
   * @brief Query all cells' current sense ranges.
   *
   * @param[out] ranges an array of sense ranges (must not be longer than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellSenseRanges(std::span<CellSenseRange> ranges) const;

  /**
   * @brief Enable or disable the cell 50/60Hz noise filter.
   *
   * This mode filters 50/60Hz noise and increases cell measurement accuracy,
   * but reduces the cell control rate to 10Hz.
   *
   * @param[in] en desired cell noise filter state
   *
   * @return An error code.
   */
  ErrorCode EnableCellNoiseFilter(bool en) const;

  /**
   * @brief Query the state of the cell noise filter.
   *
   * @return Result containing the enable state of the noise filter or an error
   * code.
   */
  Result<bool> GetCellNoiseFilterEnabled() const;

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
  Result<std::array<float, kCellCount>> MeasureAllCellVoltages() const;

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
  ErrorCode MeasureAllCellVoltages(float* voltages, std::size_t count) const;

  /**
   * @brief Measure all cells' voltages.
   *
   * @param[out] voltages an array of cell voltages
   *
   * @return An error code.
   */
  ErrorCode MeasureAllCellVoltages(
      std::array<float, kCellCount>& voltages) const;

  /**
   * @brief Measure all cells' voltages.
   *
   * @param[out] voltages an array of cell voltages (must not be longer than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode MeasureAllCellVoltages(std::span<float> voltages) const;

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
  Result<std::array<float, kCellCount>> MeasureAllCellCurrents() const;

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
  ErrorCode MeasureAllCellCurrents(float* currents, std::size_t count) const;

  /**
   * @brief Measure all cells' currents.
   *
   * @param[out] currents an array of cell currents
   *
   * @return An error code.
   */
  ErrorCode MeasureAllCellCurrents(
      std::array<float, kCellCount>& currents) const;

  /**
   * @brief Measure all cells' currents.
   *
   * @param[out] currents an array of cell currents (must not be longer than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode MeasureAllCellCurrents(std::span<float> currents) const;

  /**
   * @brief Retrieve the rolling average of the last 10 voltage measurements for
   * a single cell.
   *
   * At the default sample rate, this is a 10ms window. With filtering on, the
   * length of this window will change.
   *
   * @since v1.1.0
   *
   * @par Requires
   * Firmware v1.2.0
   *
   * @param[in] cell target cell index
   *
   * @return Result containing the average cell voltage or an error code.
   */
  Result<float> MeasureAverageCellVoltage(unsigned int cell) const;

  /**
   * @brief Retrieve the rolling average of the last 10 voltage measurements for
   * all cells.
   *
   * At the default sample rate, this is a 10ms window. With filtering on, the
   * length of this window will change.
   *
   * @since v1.1.0
   *
   * @par Requires
   * Firmware v1.2.0
   *
   * @return Result containing an array of average cell voltages or an error
   * code.
   */
  Result<std::array<float, kCellCount>> MeasureAllAverageCellVoltages() const;

  /**
   * @brief Retrieve the rolling average of the last 10 voltage measurements for
   * all cells.
   *
   * At the default sample rate, this is a 10ms window. With filtering on, the
   * length of this window will change.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @since v1.1.0
   *
   * @par Requires
   * Firmware v1.2.0
   *
   * @param[out] voltages array to store the average voltages
   * @param[in] count length of the array (must not be greater than the total
   * cell count)
   *
   * @return An error code.
   */
  ErrorCode MeasureAllAverageCellVoltages(float* voltages,
                                          std::size_t count) const;

  /**
   * @brief Retrieve the rolling average of the last 10 voltage measurements for
   * all cells.
   *
   * At the default sample rate, this is a 10ms window. With filtering on, the
   * length of this window will change.
   *
   * @since v1.1.0
   *
   * @par Requires
   * Firmware v1.2.0
   *
   * @param[out] voltages an array of average cell voltages
   *
   * @return An error code.
   */
  ErrorCode MeasureAllAverageCellVoltages(
      std::array<float, kCellCount>& voltages) const;

  /**
   * @brief Retrieve the rolling average of the last 10 voltage measurements for
   * all cells.
   *
   * At the default sample rate, this is a 10ms window. With filtering on, the
   * length of this window will change.
   *
   * @since v1.1.0
   *
   * @par Requires
   * Firmware v1.2.0
   *
   * @param[out] voltages an array of cell voltages (must not be longer than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode MeasureAllAverageCellVoltages(std::span<float> voltages) const;

  /**
   * @brief Retrieve the rolling average of the last 10 current measurements for
   * a single cell.
   *
   * At the default sample rate, this is a 10ms window. With filtering on, the
   * length of this window will change.
   *
   * @since v1.1.0
   *
   * @par Requires
   * Firmware v1.2.0
   *
   * @param[in] cell target cell index
   *
   * @return Result containing the average cell current or an error code.
   */
  Result<float> MeasureAverageCellCurrent(unsigned int cell) const;

  /**
   * @brief Retrieve the rolling average of the last 10 current measurements for
   * all cells.
   *
   * At the default sample rate, this is a 10ms window. With filtering on, the
   * length of this window will change.
   *
   * @since v1.1.0
   *
   * @par Requires
   * Firmware v1.2.0
   *
   * @return Result containing an array of average cell currents or an error
   * code.
   */
  Result<std::array<float, kCellCount>> MeasureAllAverageCellCurrents() const;

  /**
   * @brief Retrieve the rolling average of the last 10 current measurements for
   * all cells.
   *
   * At the default sample rate, this is a 10ms window. With filtering on, the
   * length of this window will change.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @since v1.1.0
   *
   * @par Requires
   * Firmware v1.2.0
   *
   * @param[out] currents array to store the average currents
   * @param[in] count length of the array (must not be greater than the total
   * cell count)
   *
   * @return An error code.
   */
  ErrorCode MeasureAllAverageCellCurrents(float* currents,
                                          std::size_t count) const;

  /**
   * @brief Retrieve the rolling average of the last 10 current measurements for
   * all cells.
   *
   * At the default sample rate, this is a 10ms window. With filtering on, the
   * length of this window will change.
   *
   * @since v1.1.0
   *
   * @par Requires
   * Firmware v1.2.0
   *
   * @param[out] currents an array of average cell currents
   *
   * @return An error code.
   */
  ErrorCode MeasureAllAverageCellCurrents(
      std::array<float, kCellCount>& currents) const;

  /**
   * @brief Retrieve the rolling average of the last 10 current measurements for
   * all cells.
   *
   * At the default sample rate, this is a 10ms window. With filtering on, the
   * length of this window will change.
   *
   * @since v1.1.0
   *
   * @par Requires
   * Firmware v1.2.0
   *
   * @param[out] currents an array of cell currents (must not be longer than the
   * total cell count)
   *
   * @return An error code.
   */
  ErrorCode MeasureAllAverageCellCurrents(std::span<float> currents) const;

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
  Result<std::array<CellMode, kCellCount>> GetAllCellOperatingModes() const;

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
  ErrorCode GetAllCellOperatingModes(CellMode* modes, std::size_t count) const;

  /**
   * @brief Query all cells' operating modes.
   *
   * @param[out] modes an array of cell operating modes
   *
   * @return An error code.
   */
  ErrorCode GetAllCellOperatingModes(
      std::array<CellMode, kCellCount>& modes) const;

  /**
   * @brief Query all cells' operating modes.
   *
   * @param[out] modes an array of cell operating modes (must not be longer than
   * the total cell count)
   *
   * @return An error code.
   */
  ErrorCode GetAllCellOperatingModes(std::span<CellMode> modes) const;

  ///@}

  /**
   * @name Aux IO Control
   */
  ///@{

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
  ErrorCode SetAllAnalogOutputs(float voltage) const;

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
  ErrorCode SetAllAnalogOutputs(const float* voltages, std::size_t count) const;

  /**
   * @brief Set all analog outputs.
   *
   * @param[in] voltages array of voltages (must not be longer than the total
   * channel count)
   *
   * @return An error code.
   */
  ErrorCode SetAllAnalogOutputs(std::span<const float> voltages) const;

  /**
   * @brief Set all analog outputs.
   *
   * @param[in] voltages array of voltages
   *
   * @return An error code.
   */
  ErrorCode SetAllAnalogOutputs(
      const std::array<float, kAnalogOutputCount>& voltages) const;

  /**
   * @brief Set multiple analog outputs to the same value.
   *
   * @param[in] channels bitmask of channels to target, 1 bit per channel
   * @param[in] voltage target voltage
   *
   * @return An error code.
   */
  ErrorCode SetMultipleAnalogOutputs(unsigned int channels,
                                     float voltage) const;

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
  Result<std::array<float, kAnalogOutputCount>> GetAllAnalogOutputs() const;

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
  ErrorCode GetAllAnalogOutputs(float* voltages, std::size_t count) const;

  /**
   * @brief Query all analog outputs' voltages.
   *
   * @param[out] voltages array of voltages
   *
   * @return An error code.
   */
  ErrorCode GetAllAnalogOutputs(
      std::array<float, kAnalogOutputCount>& voltages) const;

  /**
   * @brief Query all analog outputs' voltages.
   *
   * @param[out] voltages array of voltages (must not be longer than the total
   * channel count)
   *
   * @return An error code.
   */
  ErrorCode GetAllAnalogOutputs(std::span<float> voltages) const;

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
  ErrorCode SetAllDigitalOutputs(bool level) const;

  /**
   * @brief Set all digital outputs.
   *
   * @param[in] channels a bitmask of channels to control, 1 bit per channel
   * @param[in] level the desired output state
   *
   * @return An error code.
   */
  ErrorCode SetAllDigitalOutputsMasked(unsigned int channels, bool level) const;

  /**
   * @brief Set all digital outputs.
   *
   * @param[in] levels an array of output states
   *
   * @return An error code.
   */
  ErrorCode SetAllDigitalOutputs(
      const std::array<bool, kDigitalOutputCount>& levels) const;

  /**
   * @brief Set all digital outputs.
   *
   * @param[in] levels an array of output states (must not be longer than the
   * total channel count)
   *
   * @return An error code.
   */
  ErrorCode SetAllDigitalOutputs(std::span<const bool> levels) const;

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
  Result<std::array<bool, kDigitalOutputCount>> GetAllDigitalOutputs() const;

  /**
   * @brief Query all digital outputs' states.
   *
   * @return Result containing a bitmask of output states or an error code.
   */
  Result<unsigned int> GetAllDigitalOutputsMasked() const;

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
  Result<std::array<float, kAnalogInputCount>> MeasureAllAnalogInputs() const;

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
  ErrorCode MeasureAllAnalogInputs(float* voltages, std::size_t count) const;

  /**
   * @brief Measure all analog inputs.
   *
   * @param[out] voltages an array of analog input voltages
   *
   * @return An error code.
   */
  ErrorCode MeasureAllAnalogInputs(
      std::array<float, kAnalogInputCount>& voltages) const;

  /**
   * @brief Measure all analog inputs.
   *
   * @param[out] voltages an array of voltages (must not be longer than the
   * total channel count)
   *
   * @return An error code.
   */
  ErrorCode MeasureAllAnalogInputs(std::span<float> voltages) const;

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
  Result<std::array<bool, kDigitalInputCount>> MeasureAllDigitalInputs() const;

  /**
   * @brief Measure all digital inputs.
   *
   * @return Result containing a bitmask of input states or an error code.
   */
  Result<unsigned int> MeasureAllDigitalInputsMasked() const;

  ///@}

  /**
   * @name Model Control
   */
  ///@{

  /**
   * @brief Query the model status.
   *
   * @return Result containing the model status bitmask or an error code.
   */
  Result<std::uint8_t> GetModelStatus() const;

  /**
   * @brief Load the model configuration on the device.
   *
   * @return An error code.
   */
  ErrorCode LoadModel() const;

  /**
   * @brief Start modeling.
   *
   * @return An error code.
   */
  ErrorCode StartModel() const;

  /**
   * @brief Stop modeling.
   *
   * @return An error code.
   */
  ErrorCode StopModel() const;

  /**
   * @brief Unload the model configuration on the device.
   *
   * @return An error code.
   */
  ErrorCode UnloadModel() const;

  /**
   * @brief Query information about the currently loaded model.
   *
   * @return Result containing information about the model or an error code.
   */
  Result<ModelInfo> GetModelInfo() const;

  /**
   * @brief Query the ID of the currently loaded model. This ID is arbitrary and
   * set in the config. It is primarily for tool use.
   *
   * @return Result containing the model ID or an error code.
   */
  Result<std::string> GetModelId() const;

  /**
   * @brief Set a global model input.
   *
   * @param[in] index input index, 0-7
   * @param[in] value input value
   *
   * @return An error code.
   */
  ErrorCode SetGlobalModelInput(unsigned int index, float value) const;

  /**
   * @brief Set all global model inputs to the same value.
   *
   * @param[in] value the input value
   *
   * @return An error code.
   */
  ErrorCode SetAllGlobalModelInputs(float value) const;

  /**
   * @brief Set many global model inputs.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[in] values array of values, one per input
   * @param[in] count length of the array (must not be greater than the total
   * number of inputs)
   *
   * @return An error code.
   */
  ErrorCode SetAllGlobalModelInputs(const float* values,
                                    std::size_t count) const;

  /**
   * @brief Set many global model inputs.
   *
   * @param[in] values array of values, one per input (must not be longer than
   * the total number of inputs)
   *
   * @return An error code.
   */
  ErrorCode SetAllGlobalModelInputs(std::span<const float> values) const;

  /**
   * @brief Set all global model inputs.
   *
   * @param[in] values array of values, one per input
   *
   * @return An error code.
   */
  ErrorCode SetAllGlobalModelInputs(
      const std::array<float, kGlobalModelInputCount>& values) const;

  /**
   * @brief Query a single global model input.
   *
   * @param[in] index index of the input, 0-7
   *
   * @return Result containing the value of the input or an error code.
   */
  Result<float> GetGlobalModelInput(unsigned int index) const;

  /**
   * @brief Query all global model inputs.
   *
   * @return Result containing the array of values or an error code.
   */
  Result<std::array<float, kGlobalModelInputCount>> GetAllGlobalModelInputs()
      const;

  /**
   * @brief Query many global model inputs.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[out] values array to store the values, one per input
   * @param[in] count length of the array (must not be more than the total
   * number of inputs)
   *
   * @return An error code.
   */
  ErrorCode GetAllGlobalModelInputs(float* values, std::size_t count) const;

  /**
   * @brief Query all global model inputs.
   *
   * @param[out] values array of returned values, one per input
   *
   * @return An error code.
   */
  ErrorCode GetAllGlobalModelInputs(
      std::array<float, kGlobalModelInputCount>& values) const;

  /**
   * @brief Query many global model inputs.
   *
   * @param[out] values array of returned values, one per input (must not be
   * longer than the total number of inputs)
   *
   * @return An error code.
   */
  ErrorCode GetAllGlobalModelInputs(std::span<float> values) const;

  /**
   * @brief Set a local model input.
   *
   * @param[in] index input index, 0-7
   * @param[in] value input value
   *
   * @return An error code.
   */
  ErrorCode SetLocalModelInput(unsigned int index, float value) const;

  /**
   * @brief Set all local model inputs to the same value.
   *
   * @param[in] value the input value
   *
   * @return An error code.
   */
  ErrorCode SetAllLocalModelInputs(float value) const;

  /**
   * @brief Set many local model inputs.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[in] values array of values, one per input
   * @param[in] count length of the array (must not be greater than the total
   * number of inputs)
   *
   * @return An error code.
   */
  ErrorCode SetAllLocalModelInputs(const float* values,
                                   std::size_t count) const;

  /**
   * @brief Set many local model inputs.
   *
   * @param[in] values array of values, one per input (must not be longer than
   * the total number of inputs)
   *
   * @return An error code.
   */
  ErrorCode SetAllLocalModelInputs(std::span<const float> values) const;

  /**
   * @brief Set all local model inputs.
   *
   * @param[in] values array of values, one per input
   *
   * @return An error code.
   */
  ErrorCode SetAllLocalModelInputs(
      const std::array<float, kLocalModelInputCount>& values) const;

  /**
   * @brief Query a single local model input.
   *
   * @param[in] index index of the input, 0-7
   *
   * @return Result containing the value of the input or an error code.
   */
  Result<float> GetLocalModelInput(unsigned int index) const;

  /**
   * @brief Query all local model inputs.
   *
   * @return Result containing the array of values or an error code.
   */
  Result<std::array<float, kLocalModelInputCount>> GetAllLocalModelInputs()
      const;

  /**
   * @brief Query many local model inputs.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[out] values array to store the values, one per input
   * @param[in] count length of the array (must not be more than the total
   * number of inputs)
   *
   * @return An error code.
   */
  ErrorCode GetAllLocalModelInputs(float* values, std::size_t count) const;

  /**
   * @brief Query all local model inputs.
   *
   * @param[out] values array of returned values, one per input
   *
   * @return An error code.
   */
  ErrorCode GetAllLocalModelInputs(
      std::array<float, kLocalModelInputCount>& values) const;

  /**
   * @brief Query many local model inputs.
   *
   * @param[out] values array of returned values, one per input (must not be
   * longer than the total number of inputs)
   *
   * @return An error code.
   */
  ErrorCode GetAllLocalModelInputs(std::span<float> values) const;

  /**
   * @brief Query a single model output.
   *
   * @param[in] index index of the output, 0-35
   *
   * @return Result containing the output value or an error code.
   */
  Result<float> GetModelOutput(unsigned int index) const;

  /**
   * @brief Query all model outputs.
   *
   * @return Result containing the outputs or an error code.
   */
  Result<std::array<float, kModelOutputCount>> GetAllModelOutputs() const;

  /**
   * @brief Query many model outputs.
   *
   * @note It is recommended that the array and span overloads be preferred over
   * this overload whenever possible.
   *
   * @param[out] outputs array of returned model output values, one per channel
   * @param[in] count length of the array (must not be greater than the total
   * number of outputs)
   *
   * @return An error code.
   */
  ErrorCode GetAllModelOutputs(float* outputs, std::size_t count) const;

  /**
   * @brief Query all model outputs.
   *
   * @param[out] outputs array of returned outputs
   *
   * @return An error code.
   */
  ErrorCode GetAllModelOutputs(
      std::array<float, kModelOutputCount>& outputs) const;

  /**
   * @brief Query many model outputs.
   *
   * @param[out] outputs array of returned outputs, one per output (must not be
   * longer than the total number of outputs)
   *
   * @return An error code.
   */
  ErrorCode GetAllModelOutputs(std::span<float> outputs) const;

  ///@}

 protected:
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

 private:
  /// Driver handle.
  std::shared_ptr<drivers::CommDriver> driver_;

  /// Read timeout.
  unsigned int read_timeout_ms_;
};

}  // namespace bci::abs

#endif /* ABS_SCPI_DRIVER_INCLUDE_BCI_ABS_SCPICLIENT_H */
