#ifndef ABS_SCPI_DRIVER_SRC_SCPIUTIL_H
#define ABS_SCPI_DRIVER_SRC_SCPIUTIL_H

#include <bci/abs/CommonTypes.h>

#include <concepts>
#include <ranges>
#include <span>
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

    if (auto v =
            util::StrViewToFloat(std::string_view(val.begin(), val.end()))) {
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
ErrorCode SplitRespMnemonics(std::string_view resp,
                             std::array<std::string, kLen>& out) {
  resp = util::Trim(resp);

  std::size_t i = 0;
  for (const auto val : std::views::split(resp, ',')) {
    if (i >= out.size()) {
      return ErrorCode::kInvalidResponse;
    }

    out[i++] = std::string(val.begin(), val.end());
  }

  if (i < out.size()) {
    return ErrorCode::kInvalidResponse;
  }

  return ErrorCode::kSuccess;
}

template <std::size_t kLen>
constexpr ErrorCode SplitRespMnemonics(
    std::string_view resp, std::array<std::string_view, kLen>& out) {
  resp = util::Trim(resp);

  std::size_t i = 0;
  for (const auto val : std::views::split(resp, ',')) {
    if (i >= out.size()) {
      return ErrorCode::kInvalidResponse;
    }

    out[i++] = std::string_view(val.begin(), val.end());
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

}  // namespace bci::abs::scpi

#endif /* ABS_SCPI_DRIVER_SRC_SCPIUTIL_H */
