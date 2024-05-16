#ifndef ABS_SCPI_DRIVER_SRC_SCPIUTIL_H
#define ABS_SCPI_DRIVER_SRC_SCPIUTIL_H

#include <bci/abs/CommonTypes.h>

#include <concepts>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include "StringUtil.h"
#include "Util.h"

namespace bci::abs::scpi {

template <std::size_t kLen>
constexpr ErrorCode SplitRespFloats(std::string_view resp,
                                    std::span<float, kLen> out) {
  resp = util::Trim(resp);

  std::size_t i = 0;
  for (const auto val : std::views::split(resp, ',')) {
    if (i >= out.size()) {
      return ErrorCode::kInvalidResponse;
    }

    if (auto v = util::StrViewToFloat(
            std::string_view(&*val.begin(), std::ranges::distance(val)))) {
      out[i++] = *v;
    } else {
      return ErrorCode::kInvalidResponse;
    }
  }

  if (i < out.size()) {
    return ErrorCode::kInvalidResponse;
  }

  return ErrorCode::kSuccess;
}

template <std::size_t kLen>
constexpr ErrorCode SplitRespFloats(std::string_view resp,
                                    std::array<float, kLen>& out) {
  return SplitRespFloats(resp, std::span{out});
}

template <std::size_t kLen>
constexpr Result<std::array<float, kLen>> ParseRespFloatArray(
    std::string_view resp) {
  std::array<float, kLen> res{};
  auto e = SplitRespFloats(resp, res);
  if (e != ErrorCode::kSuccess) {
    return util::Err(e);
  }
  return res;
}

template <std::size_t kLen>
constexpr ErrorCode SplitRespBools(std::string_view resp,
                                   std::span<bool, kLen> out) {
  resp = util::Trim(resp);

  std::size_t i = 0;
  for (const auto val : std::views::split(resp, ',')) {
    if (i >= out.size()) {
      return ErrorCode::kInvalidResponse;
    }

    if (auto v = util::StrViewToBool(
            std::string_view(&*val.begin(), std::ranges::distance(val)))) {
      out[i++] = *v;
    } else {
      return ErrorCode::kInvalidResponse;
    }
  }

  if (i < out.size()) {
    return ErrorCode::kInvalidResponse;
  }

  return ErrorCode::kSuccess;
}

template <std::size_t kLen>
constexpr ErrorCode SplitRespBools(std::string_view resp,
                                   std::array<bool, kLen>& out) {
  return SplitRespBools(resp, std::span{out});
}

template <std::size_t kLen>
constexpr Result<std::array<bool, kLen>> ParseRespBoolArray(
    std::string_view resp) {
  std::array<bool, kLen> res{};
  auto e = SplitRespBools(resp, res);
  if (e != ErrorCode::kSuccess) {
    return util::Err(e);
  }
  return res;
}

template <std::size_t kLen>
constexpr ErrorCode SplitRespMnemonics(std::string_view resp,
                                       std::span<std::string_view, kLen> out) {
  resp = util::Trim(resp);

  std::size_t i = 0;
  for (const auto val : std::views::split(resp, ',')) {
    if (i >= out.size()) {
      return ErrorCode::kInvalidResponse;
    }

    out[i++] = std::string_view(&*val.begin(), std::ranges::distance(val));
  }

  if (i < out.size()) {
    return ErrorCode::kInvalidResponse;
  }

  return ErrorCode::kSuccess;
}

template <std::size_t kLen>
static ErrorCode SplitRespMnemonics(std::string_view resp,
                                    std::array<std::string, kLen>& out) {
  std::array<std::string_view, kLen> split;
  auto e = SplitRespMnemonics(resp, std::span{split});
  if (e != ErrorCode::kSuccess) {
    return e;
  }

  for (std::size_t i = 0; i < kLen; ++i) {
    out[i] = std::string{split[i].begin(), split[i].end()};
  }

  return ErrorCode::kSuccess;
}

template <std::size_t kLen>
constexpr ErrorCode SplitRespMnemonics(
    std::string_view resp, std::array<std::string_view, kLen>& out) {
  return SplitRespMnemonics(resp, std::span{out});
}

template <class T, std::size_t kLen>
constexpr ErrorCode ParseRespMnemonics(std::string_view resp,
                                       std::span<T, kLen> out,
                                       Result<T> (*func)(std::string_view)) {
  if (!func) {
    return ErrorCode::kInvalidArgument;
  }

  resp = util::Trim(resp);

  std::size_t i = 0;
  for (const auto val : std::views::split(resp, ',')) {
    if (i >= out.size()) {
      return ErrorCode::kInvalidResponse;
    }

    std::string_view elem_str(&*val.begin(), std::ranges::distance(val));

    if (auto r = func(elem_str)) {
      out[i++] = *r;
    } else {
      return r.error();
    }
  }

  if (i < out.size()) {
    return ErrorCode::kInvalidResponse;
  }

  return ErrorCode::kSuccess;
}

constexpr Result<float> ParseFloatResponse(std::string_view text) {
  if (auto f = util::StrViewToFloat(text)) {
    return *f;
  }
  return util::Err(ErrorCode::kInvalidResponse);
}

constexpr Result<bool> ParseBoolResponse(std::string_view text) {
  if (auto b = util::StrViewToBool(text)) {
    return *b;
  }
  return util::Err(ErrorCode::kInvalidResponse);
}

template <std::integral T>
constexpr Result<T> ParseIntResponse(std::string_view str) noexcept {
  if (auto i = util::StrViewToInt<T>(str)) {
    return *i;
  }
  return util::Err(ErrorCode::kInvalidResponse);
}

constexpr std::string_view CellFaultMnemonic(CellFault fault) noexcept {
  switch (fault) {
    case CellFault::kNone:
      return "NONE";
    case CellFault::kOpenCircuit:
      return "OPEN";
    case CellFault::kShortCircuit:
      return "SHORT";
    case CellFault::kPolarity:
      return "POL";
  }
  return "";
}

constexpr Result<CellFault> ParseCellFault(std::string_view str) noexcept {
  constexpr std::pair<std::string_view, CellFault> kOpts[] = {
      {"NONE", CellFault::kNone},
      {"OPENCIRCUIT", CellFault::kOpenCircuit},
      {"SHORTCIRCUIT", CellFault::kShortCircuit},
      {"POLARITY", CellFault::kPolarity},
  };

  str = util::Trim(str);

  for (auto [text, fault] : kOpts) {
    if (str == text) {
      return fault;
    }
  }

  return util::Err(ErrorCode::kInvalidResponse);
}

template <std::size_t kLen>
constexpr Result<std::array<CellFault, kLen>> ParseCellFaultArray(
    std::string_view str) noexcept {
  std::array<CellFault, kLen> res;
  auto e = ParseRespMnemonics(str, std::span{res}, ParseCellFault);
  if (e != ErrorCode::kSuccess) {
    return util::Err(e);
  }
  return res;
}

constexpr std::string_view CellSenseRangeMnemonic(
    CellSenseRange range) noexcept {
  switch (range) {
    case CellSenseRange::kAuto:
      return "AUTO";
    case CellSenseRange::kLow:
      return "LO";
    case CellSenseRange::kHigh:
      return "HI";
  }
  return "";
}

constexpr Result<CellSenseRange> ParseCellSenseRange(
    std::string_view str) noexcept {
  constexpr std::pair<std::string_view, CellSenseRange> kOpts[] = {
      {"AUTO", CellSenseRange::kAuto},
      {"LOW", CellSenseRange::kLow},
      {"HIGH", CellSenseRange::kHigh},
  };

  str = util::Trim(str);

  for (auto [text, range] : kOpts) {
    if (str == text) {
      return range;
    }
  }

  return util::Err(ErrorCode::kInvalidResponse);
}

template <std::size_t kLen>
constexpr Result<std::array<CellSenseRange, kLen>> ParseCellSenseRangeArray(
    std::string_view str) noexcept {
  std::array<CellSenseRange, kLen> res;
  auto e = ParseRespMnemonics(str, std::span{res}, ParseCellSenseRange);
  if (e != ErrorCode::kSuccess) {
    return util::Err(e);
  }
  return res;
}

constexpr Result<CellMode> ParseCellOperatingMode(
    std::string_view str) noexcept {
  constexpr std::pair<std::string_view, CellMode> kOpts[] = {
      {"CV", CellMode::kConstantVoltage},
      {"ILIM", CellMode::kCurrentLimited},
  };

  str = util::Trim(str);

  for (auto [text, mode] : kOpts) {
    if (str == text) {
      return mode;
    }
  }

  return util::Err(ErrorCode::kInvalidResponse);
}

template <std::size_t kLen>
constexpr Result<std::array<CellMode, kLen>> ParseCellOperatingModeArray(
    std::string_view str) noexcept {
  std::array<CellMode, kLen> res;
  auto e = ParseRespMnemonics(str, std::span{res}, ParseCellOperatingMode);
  if (e != ErrorCode::kSuccess) {
    return util::Err(e);
  }
  return res;
}

constexpr std::string_view CellPrecisionModeMnemonic(
    CellPrecisionMode mode) noexcept {
  switch (mode) {
    case CellPrecisionMode::kNormal:
      return "NORM";
    case CellPrecisionMode::kHighPrecision:
      return "PREC";
    case CellPrecisionMode::kNoiseRejection:
      return "FILT";
  }
  return "";
}

constexpr Result<CellPrecisionMode> ParseCellPrecisionMode(
    std::string_view str) noexcept {
  constexpr std::pair<std::string_view, CellPrecisionMode> kOpts[] = {
      {"NORMAL", CellPrecisionMode::kNormal},
      {"PRECISION", CellPrecisionMode::kHighPrecision},
      {"FILTER", CellPrecisionMode::kNoiseRejection},
  };

  str = util::Trim(str);

  for (auto [text, mode] : kOpts) {
    if (str == text) {
      return mode;
    }
  }

  return util::Err(ErrorCode::kInvalidResponse);
}

// Parse quoted SCPI <String> data.
std::optional<std::string> ParseQuotedString(std::string_view str);

// Parse a quoted string, but return a string view containing the rest of the
// string, if any.
std::optional<std::string> ParseQuotedStringUntil(std::string_view str,
                                                  std::string_view& suffix);

Result<std::string> ParseStringResponse(std::string_view str);

Result<ScpiError> ParseScpiError(std::string_view str);

template <std::size_t kLen>
static Result<std::array<std::string, kLen>> ParseStringArrayResponse(
    std::string_view str) {
  std::array<std::string, kLen> res{};
  for (std::size_t i = 0; i < kLen; ++i) {
    std::string_view suffix;
    if (auto s = ParseQuotedStringUntil(str, suffix)) {
      res[i] = *std::move(s);
      if (i < kLen - 1) {
        suffix = util::Trim(suffix);
        if (suffix.empty() || suffix[0] != ',') {
          return util::Err(ErrorCode::kInvalidResponse);
        }
        suffix.remove_prefix(1);
        str = suffix;
      }
    } else {
      return util::Err(ErrorCode::kInvalidResponse);
    }
  }
  return res;
}

}  // namespace bci::abs::scpi

#endif /* ABS_SCPI_DRIVER_SRC_SCPIUTIL_H */
