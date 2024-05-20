/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

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
