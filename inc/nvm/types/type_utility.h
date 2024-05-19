/*
 *  Copyright (c) 2024 Linggawasistha Djohari
 *  <linggawasistha.djohari@outlook.com> Licensed to Linggawasistha Djohari
 * under one or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information regarding copyright
 * ownership.
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

#pragma once

#include <optional>
#include <type_traits>
#include <utility>

namespace nvm::types::utility {

// Primary template - default to false
template <typename T>
struct is_smart_ptr : std::false_type {};

// Specialization for std::unique_ptr
template <typename T>
struct is_smart_ptr<std::unique_ptr<T>> : std::true_type {};

// Specialization for std::shared_ptr
template <typename T>
struct is_smart_ptr<std::shared_ptr<T>> : std::true_type {};

// Specialization for std::weak_ptr
template <typename T>
struct is_smart_ptr<std::weak_ptr<T>> : std::true_type {};

/// @brief Get is T smart pointer type
/// @tparam T any type
/// @return true if smart_ptr type
template <typename T>
constexpr bool is_smart_ptr_v() {
  return is_smart_ptr<T>::value;
}

/// @brief Get is T is raw pointer type
/// @tparam T any type
/// @return true if raw pointer type
template <typename T>
constexpr bool is_raw_ptr_v(){
    return std::is_pointer_v<T>;
}

/// @brief Get is T is std::optional<> type
/// @tparam T any type
/// @return true if std::optional<> type
template <typename T>
constexpr bool is_optional_v(){
    return std::is_same_v<T, std::optional<T>>;
}

// Helper type traits to check if a comparison operator exists
template <typename, typename = std::void_t<>>
struct has_less_than : std::false_type {};

template <typename T>
struct has_less_than<
    T, std::void_t<decltype(std::declval<T>() < std::declval<T>())>>
    : std::true_type {};

template <typename, typename = std::void_t<>>
struct has_greater_than : std::false_type {};

template <typename T>
struct has_greater_than<
    T, std::void_t<decltype(std::declval<T>() > std::declval<T>())>>
    : std::true_type {};

template <typename, typename = std::void_t<>>
struct has_less_equal : std::false_type {};

template <typename T>
struct has_less_equal<
    T, std::void_t<decltype(std::declval<T>() <= std::declval<T>())>>
    : std::true_type {};

template <typename, typename = std::void_t<>>
struct has_greater_equal : std::false_type {};

template <typename T>
struct has_greater_equal<
    T, std::void_t<decltype(std::declval<T>() >= std::declval<T>())>>
    : std::true_type {};

template <typename, typename = std::void_t<>>
struct has_equal : std::false_type {};

template <typename T>
struct has_equal<T,
                 std::void_t<decltype(std::declval<T>() == std::declval<T>())>>
    : std::true_type {};

template <typename, typename = std::void_t<>>
struct has_not_equal : std::false_type {};

template <typename T>
struct has_not_equal<
    T, std::void_t<decltype(std::declval<T>() != std::declval<T>())>>
    : std::true_type {};

/// @brief Check if type have logical comparable ==,!=,<,<=,>,>=
template <typename T>
using is_logical__comparable =
    std::conjunction<has_less_than<T>, has_greater_than<T>, has_less_equal<T>,
                     has_greater_equal<T>, has_equal<T>, has_not_equal<T>>;

template <typename T>
using is_logical__eq_neq = std::conjunction<has_equal<T>, has_not_equal<T>>;

/// @brief Check if type have  ==,!=
template <typename T>
using is_logical__equal_comparator =
    std::conjunction<has_equal<T>, has_not_equal<T>>;

/// @brief Check if type have  <,>
template <typename T>
using is_logical__ltgt_comparator =
    std::conjunction<has_less_than<T>, has_greater_than<T>>;

/// @brief Check if type have  <=,=>
template <typename T>
using is_logical__lte_gte_comparator =
    std::conjunction<has_less_equal<T>, has_greater_equal<T>>;

/// @brief 
/// @tparam T 
/// @return 
template <typename T>
constexpr bool is_string_and_string_view_v() {
  return (std::is_same_v<T, std::string> ||
          std::is_same_v<T, std::string_view>);
}

/// @brief 
/// @tparam T 
/// @return 
template <typename T>
constexpr bool is_string_and_string_view_optional_v() {
  return (std::is_same_v<T, std::optional<std::string>> ||
          std::is_same_v<T, std::optional<std::string_view>>);
}

/// @brief 
/// @tparam T 
/// @return 
template <typename T>
constexpr bool is_string_and_string_view_smart_ptr_v() {
  return (std::is_same_v<T, std::unique_ptr<std::string>> ||
          std::is_same_v<T, std::shared_ptr<std::string>> ||
          std::is_same_v<T, std::weak_ptr<std::string>> ||
          std::is_same_v<T, std::unique_ptr<std::string_view>> ||
          std::is_same_v<T, std::shared_ptr<std::string_view>> ||
          std::is_same_v<T, std::weak_ptr<std::string_view>>);
}

/// @brief 
/// @tparam T 
/// @return 
template <typename T>
constexpr bool is_string_and_string_view_family_v() {
  return (is_string_and_string_view_v<T> ||
          is_string_and_string_view_optional_v<T>());
}

/// @brief 
/// @tparam T 
/// @return 
template <typename T>
constexpr bool is_string_and_string_view_family_ptr_v() {
  return (is_string_and_string_view_family_v<T>() ||
          is_string_and_string_view_smart_ptr_v<T>());
}

}  // namespace nvm::types::utility
