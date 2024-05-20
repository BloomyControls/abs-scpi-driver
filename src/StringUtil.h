/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#ifndef ABS_SCPI_DRIVER_SRC_STRINGUTIL_H
#define ABS_SCPI_DRIVER_SRC_STRINGUTIL_H

#include <algorithm>
#include <charconv>
#include <concepts>
#include <optional>
#include <string_view>
#include <system_error>

#include <fast_float/fast_float.h>

namespace bci::abs::util {

inline constexpr std::string_view kTrimChars{" \t\v\r\n\0"};

inline constexpr std::string_view Trim(std::string_view v) noexcept {
  if (v.size() == 0) {
    return v;
  }

  auto s = v.find_first_not_of(kTrimChars);
  v.remove_prefix((std::min)(s, v.size()));
  if (v.size() <= 1) {
    return v;
  }

  s = v.find_last_not_of(kTrimChars);
  if (s != v.npos) {
    v.remove_suffix(v.size() - s - 1);
  }

  return v;
}

inline constexpr std::optional<float> StrViewToFloat(std::string_view sv) {
  sv = Trim(sv);

  if (sv.empty()) {
    return std::nullopt;
  }

  float f;
  auto [ptr, ec] = fast_float::from_chars(sv.data(), sv.data() + sv.size(), f);
  if (ec == std::errc()) {
    return f;
  } else {
    return std::nullopt;
  }
}

inline constexpr std::optional<bool> StrViewToBool(std::string_view sv) {
  sv = Trim(sv);

  if (sv.size() != 1) {
    return std::nullopt;
  }

  switch (sv[0]) {
    case '0':
      return false;
    case '1':
      return true;
    default:
      return std::nullopt;
  }
}

template <std::integral T>
inline constexpr std::optional<T> StrViewToInt(std::string_view sv) {
  sv = Trim(sv);

  if (sv.empty()) {
    return std::nullopt;
  }

  int base = 10;
  // check for a SCPI base prefix
  if (sv.starts_with("#H") || sv.starts_with("#h")) {
    base = 16;
  } else if (sv.starts_with("#Q") || sv.starts_with("#q")) {
    base = 8;
  } else if (sv.starts_with("#B") || sv.starts_with("#b")) {
    base = 2;
  }

  if (base != 10) {
    sv.remove_prefix(2);
    if (sv.empty()) {
      return std::nullopt;
    }
  }

  T val{};

  auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val, base);
  if (ec == std::errc()) {
    return val;
  }

  return std::nullopt;
}

}  // namespace bci::abs::util

#endif  /* ABS_SCPI_DRIVER_SRC_STRINGUTIL_H */
