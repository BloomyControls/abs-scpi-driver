#ifndef ABS_SCPI_DRIVER_SRC_STRINGUTIL_H
#define ABS_SCPI_DRIVER_SRC_STRINGUTIL_H

#include <algorithm>
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

}  // namespace bci::abs::util

#endif  /* ABS_SCPI_DRIVER_SRC_STRINGUTIL_H */
