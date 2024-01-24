/*
 *  Copyright (c) 2020 Linggawasistha Djohari
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

#ifndef NVM_CORE_V2_NULLABLE_H
#define NVM_CORE_V2_NULLABLE_H

#include <optional>

namespace nvm {

/// @brief Nullable for value type. Nullable class is inherit std::optional and
/// as wrapper for std::optional for easy usage.
/// Example usage: Nullable&lt;std::string&gt;,Nullable&lt;int&gt;.
/// @tparam T any value supported by std::optional.
template <typename T>
class Nullable : public std::optional<T> {
 public:
  Nullable() noexcept = default;

  // cppcheck-suppress noExplicitConstructor
  Nullable(std::nullptr_t) noexcept {}

  // cppcheck-suppress noExplicitConstructor
  Nullable(T const &val) : std::optional<T>(val) {}

  // cppcheck-suppress noExplicitConstructor
  Nullable(std::optional<T> const &opt) noexcept : std::optional<T>(opt) {}

  // cppcheck-suppress noExplicitConstructor
  Nullable(std::optional<T> &&opt) noexcept
      : std::optional<T>(std::move(opt)) {}

  Nullable<T> &operator=(T const &v) {
    std::optional<T>::operator=(v);
    return *this;
  }

  Nullable<T> &operator=(std::nullptr_t) noexcept {
    std::optional<T>::operator=(std::nullopt);
    return *this;
  }

  Nullable<T> &operator=(std::optional<T> const &opt) noexcept {
    std::optional<T>::operator=(opt);
    return *this;
  }

  Nullable<T> &operator=(std::optional<T> &&opt) noexcept {
    std::optional<T>::operator=(std::move(opt));
    return *this;
  }

  explicit operator bool() const noexcept { return this->has_value(); }

  T &Value() {
    if (this->has_value()) return std::optional<T>::value();
    throw std::bad_optional_access();
  }

  const T &Value() const {
    if (this->has_value()) return std::optional<T>::value();
    throw std::bad_optional_access();
  }

  T &ValueOr(T &def) noexcept {
    if (this->has_value())
      return std::optional<T>::value();
    else
      return def;
  }

  const T &ValueOr(T const &def) const noexcept {
    if (this->has_value())
      return std::optional<T>::value();
    else
      return def;
  }
};

}  // namespace nvm

#endif