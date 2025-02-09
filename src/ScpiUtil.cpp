/*
 * Copyright (c) 2024, Bloomy Controls, Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can be
 * found in the LICENSE file or at https://opensource.org/license/BSD-3-Clause
 */

#include "ScpiUtil.h"

#include <bci/abs/CommonTypes.h>

#include <optional>
#include <string>
#include <string_view>

#include "StringUtil.h"
#include "Util.h"

namespace bci::abs::scpi {

using ec = ErrorCode;
using util::Err;

std::optional<std::string> ParseQuotedString(std::string_view str) {
  str = util::Trim(str);
  if (str.size() < 2) {
    return std::nullopt;
  }

  const char delim = str[0];
  if (delim != '"' && delim != '\'') {
    return std::nullopt;
  }

  if (str[str.size() - 1] != delim) {
    return std::nullopt;
  }

  str.remove_prefix(1);
  str.remove_suffix(1);

  std::string ret;
  ret.reserve(str.size());

  for (std::size_t i = 0; i < str.size(); ++i) {
    if (str[i] != delim) {
      ret += str[i];
    } else {
      if (i < str.size() - 1) {
        if (str[i + 1] == delim) {
          ret += str[i];
          ++i;
        } else {
          return std::nullopt;
        }
      } else {
        return std::nullopt;
      }
    }
  }

  return ret;
}

std::optional<std::string> ParseQuotedStringUntil(std::string_view str,
                                                  std::string_view& suffix) {
  str = util::Trim(str);
  if (str.size() < 2) {
    return std::nullopt;
  }

  const char delim = str[0];
  if (delim != '"' && delim != '\'') {
    return std::nullopt;
  }

  str.remove_prefix(1);

  std::string ret;

  for (std::size_t i = 0; i < str.size(); ++i) {
    if (str[i] != delim) {
      ret += str[i];
    } else {
      if (i < str.size() - 1) {
        if (str[i + 1] == delim) {
          ret += str[i];
          ++i;
        } else {
          suffix = str.substr(i + 1);
          break;
        }
      } else {
        suffix = {};
        break;
      }
    }
  }

  return ret;
}

Result<std::string> ParseStringResponse(std::string_view str) {
  auto e = ParseQuotedString(util::Trim(str));
  if (e) {
    return *std::move(e);
  } else {
    return Err(ec::kInvalidResponse);
  }
}

Result<ScpiError> ParseScpiError(std::string_view str) {
  str = util::Trim(str);

  if (str.empty()) {
    return Err(ec::kInvalidResponse);
  }

  auto sep_pos = str.find(',');
  if (sep_pos == str.npos) {
    return Err(ec::kInvalidResponse);
  }

  auto err_code = util::Trim(str.substr(0, sep_pos));
  if (err_code.empty()) {
    return Err(ec::kInvalidResponse);
  }

  auto err_num = ParseIntResponse<std::int16_t>(err_code);
  if (!err_num) {
    return Err(err_num.error());
  }

  auto message = util::Trim(str.substr(sep_pos + 1));
  if (message.empty()) {
    return Err(ec::kInvalidResponse);
  }

  auto err_msg = ParseQuotedString(message);
  if (!err_msg) {
    return Err(ec::kInvalidResponse);
  }

  return ScpiError{*err_num, *std::move(err_msg)};
}

}  // namespace bci::abs::scpi
