#ifndef ABS_SCPI_DRIVER_SRC_SCPIUTIL_H
#define ABS_SCPI_DRIVER_SRC_SCPIUTIL_H

#include <bci/abs/CommonTypes.h>

#include <ranges>
#include <span>
#include <string_view>

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
}

constexpr Result<float> ParseFloatResponse(std::string_view text) {
  if (auto f = util::StrViewToFloat(text)) {
    return *f;
  }
  return util::Err(ErrorCode::kInvalidResponse);
}

}  // namespace bci::abs::scpi

#endif /* ABS_SCPI_DRIVER_SRC_SCPIUTIL_H */
