/*
 *  Copyright (c) 2023 Linggawasistha Djohari
 * <linggawasistha.djohari@outlook.com> Licensed to Linggawasistha Djohari under
 * one or more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 *
 *  Linggawasistha Djohari licenses this file to you under the Apache License,
 *  Version 2.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef NVM_CORE_V2_UTF8_STRING_H
#define NVM_CORE_V2_UTF8_STRING_H

#include <string>

#include "utf8.h"

namespace nvm {
namespace strings {

/// @brief Utf8 codepoint representation
class Utf8CodePoint {
 private:
  uint32_t codepoint_;

 public:
  Utf8CodePoint() noexcept : codepoint_(0){};

  explicit Utf8CodePoint(uint32_t codepoint) noexcept : codepoint_(codepoint){};

  uint32_t CodePoint() const { return codepoint_; }

  bool IsValid() const {
    return (codepoint_ <= 0x10FFFF) && (codepoint_ != 0xFFFE) &&
           (codepoint_ != 0xFFFF);
  }
};

/// @brief Utf8 codepoint range
class Ut8CodepointRange {
 private:
  Utf8CodePoint start_;
  Utf8CodePoint end_;

 public:
  Ut8CodepointRange(const Utf8CodePoint& start,
                    const Utf8CodePoint& end) noexcept
      : start_(start), end_(end){};

  Ut8CodepointRange(const uint32_t& start, const uint32_t& end) noexcept
      : start_(start), end_(end){};

  ~Ut8CodepointRange(){};

  size_t Size() const { return end_.CodePoint() - start_.CodePoint(); }

  bool IsValidRange() const {
    return (start_.IsValid()) && (end_.IsValid()) &&
           (start_.CodePoint() < end_.CodePoint());
  };
};

/// @brief Utf8String implementations
class Utf8String {
 private:
  std::string content_;
  bool is_utf8_;

 public:
  Utf8String() noexcept : content_(std::string()), is_utf8_(false){};

  explicit Utf8String(const std::string& content) noexcept
      : content_(std::string()), is_utf8_(false) {
    is_utf8_ = utf8::is_valid(content_.begin(), content_.end());
  };

  ~Utf8String(){};

  /// @brief Check if the string is valid Utf8
  /// @return true if the string is valid Utf8
  bool IsUtf8() const { return is_utf8_; }

  /// @brief Return the bytes size of the string
  /// @return
  size_t Size() const { return content_.size(); }

  /// @brief Return the character length of the string. Be careful on Utf8 it
  /// will return numbers of codepoints.
  /// @return number of characters (num of codepoints in Utf8)
  size_t Len() const {
    if (is_utf8_) {
      // If the string is valid UTF-8, return the number of code points
      return utf8::distance(content_.begin(), content_.end());
    } else {
      // If the string is not valid UTF-8, return the number of bytes
      return content_.size();
    }
  }

  /// @brief Return the string content
  /// @return
  const std::string& Str() const { return content_; }

  // /// @brief
  // /// @param range
  // /// @return
  // bool ContainsCodePointRange(const Ut8CodepointRange& range) const {
  //   size_t num_codepoints = content_.Len();
  //   for (size_t i = 0, cp = 0; cp < num_codepoints; ++cp) {
  //     uint32_t codepoint;
  //     auto it = content_.begin() + i;
  //     utf8::unchecked::next(it, content_.end(), codepoint);
  //     if (codepoint >= range.start_.CodePoint() &&
  //         codepoint <= range.end_.CodePoint()) {
  //       return true;
  //     }
  //     i += std::distance(content_.begin() + i, it);
  //   }
  //   return false;
  // }

  template <typename T>
  static Utf8String MakeUtf8String(const T* bytes, size_t size) noexcept {
    static_assert(
        std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
        "T can only be char or uint8_t");
    auto utf8_string = std::string(bytes, size);
    utf8::replace_invalid(bytes, bytes + size, std::back_inserter(utf8_string));
    return Utf8String(std::move(utf8_string));
  }

  template <typename T>
  static Utf8String MakeUtf8StringUnchecked(const T* bytes,
                                            size_t size) noexcept {
    static_assert(
        std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
        "T can only be char or uint8_t");

    return Utf8String(std::move(std::string(bytes, size)));
  }
};

}  // namespace strings
}  // namespace nvm
#endif  // NVM_CORE_V2_UTF8_STRING_H
