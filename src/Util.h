#ifndef ABS_SCPI_DRIVER_SRC_UTIL_H
#define ABS_SCPI_DRIVER_SRC_UTIL_H

#include <bci/abs/CommonTypes.h>

#include <utility>

namespace bci::abs::util {

template <class T>
inline constexpr auto Err(T&& val) {
  return tl::unexpected(std::forward<T>(val));
}

}  // namespace bci::abs::util

#endif  /* ABS_SCPI_DRIVER_SRC_UTIL_H */
