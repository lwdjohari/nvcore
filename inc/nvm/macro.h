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

#ifndef NVM_CORE_V2_MACRO_H
#define NVM_CORE_V2_MACRO_H

#include "nvm/types/type_utility.h"

namespace nvm {

#define NVM_ENUMCLASS_ENABLE_BITMASK_OPERATORS(T)                        \
  inline T operator|(T lhs, T rhs) {                                     \
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) |  \
                          static_cast<std::underlying_type_t<T>>(rhs));  \
  }                                                                      \
                                                                         \
  inline T operator&(T lhs, T rhs) {                                     \
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) &  \
                          static_cast<std::underlying_type_t<T>>(rhs));  \
  }                                                                      \
                                                                         \
  inline T operator^(T lhs, T rhs) {                                     \
    return static_cast<T>(static_cast<std::underlying_type_t<T>>(lhs) ^  \
                          static_cast<std::underlying_type_t<T>>(rhs));  \
  }                                                                      \
                                                                         \
  inline T operator~(T rhs) {                                            \
    return static_cast<T>(~static_cast<std::underlying_type_t<T>>(rhs)); \
  }

#define NVM_CONST_DELETE_COPY_AND_DEFAULT_MOVE(Type) \
  Type(const Type&) = delete;                        \
  Type& operator=(const Type&) = delete;             \
  Type(Type&&) noexcept = default;                   \
  Type& operator=(Type&&) noexcept = default;

#define NVM_ENUM_CLASS_DISPLAY_TRAIT(E)                                  \
  inline std::ostream& operator<<(std::ostream& os, E e) {               \
    return os << static_cast<typename std::underlying_type<E>::type>(e); \
  }

#define NVM_ASSERT_VALID_STRING_TYPE(T)                                   \
  static_assert(                                                          \
      nvm::types::utility::is_string_and_string_view_v<T>() ||            \
          nvm::types::utility::is_string_and_string_view_optional_v<T>(), \
      "T must be in one of std::string, std::string_view, "               \
      "std::optional<std::string>, "                                      \
      "or std::optional<std::string_view> type");

#define NVM_ASSERT_VALID_STRING_SPTR_TYPE(T)                                \
  static_assert(                                                            \
      nvm::types::utility::is_string_and_string_view_v<T>() ||              \
          nvm::types::utility::is_string_and_string_view_optional_v<T>() || \
          nvm::types::utility::is_string_and_string_view_smart_ptr_v<T>(),  \
      "T must be in one of std::string, std::string_view, "                 \
      "std::optional<std::string>, "                                        \
      "or std::optional<std::string_view> and smart_ptr of the families "   \
      "type");

#define NVM_ASSERT_VALID_NULLABLE_OR_EMPTY_TYPE(T)                            \
  static_assert(                                                              \
      nvm::types::utility::is_string_and_string_view_v<T>() ||                \
          nvm::types::utility::is_string_and_string_view_optional_v<T>() ||   \
          nvm::types::utility::is_string_and_string_view_smart_ptr_v<T>() ||  \
          std::is_same_v<T, std::optional<T>> ||                              \
          nvm::types::utility::is_smart_ptr_v<T>(),                           \
      "T must be in one of std::string, std::string_view, std::optional<T>, " \
      "std::unique_ptr<T>, std::shared_ptr<T>, std::weak_ptr<T>, or a raw "   \
      "pointer type");

#define NVM_ASSERT_VALID_NULLABLE_TYPE(T)                                      \
  static_assert(std::is_same_v<T, std::optional<T>> ||                         \
                    nvm::types::utility::is_smart_ptr_v<T>(),                  \
                "T must be in one of std::unique_ptr<T>, std::shared_ptr<T>, " \
                "std::weak_ptr<T> or std::optional<T> type");

#define NVM_ASSERT_VALID_LOGICAL_COMPAREABLE_TYPE(T)                   \
  static_assert(nvm::types::utility::is_logical__comparable<T>::value, \
                "Type T must support all comparison operators: "       \
                "<, >, <=, >=, ==, !=");

#define NVM_ASSERT_VALID_LOGICAL_EQ_NEQ_TYPE(T)                    \
  static_assert(nvm::types::utility::is_logical__eq_neq<T>::value, \
                "Type T must support all comparison operators: "   \
                "==, !=");

#define NVM_ASSERT_VALID_LOGICAL_LT_GT_TYPE(T)                         \
  static_assert(nvm::types::utility::is_logical__comparable<T>::value, \
                "Type T must support all comparison operators: "       \
                "<, >");

#define NVM_ASSERT_VALID_LOGICAL_LTE_GTE_TYPE(T)                       \
  static_assert(nvm::types::utility::is_logical__comparable<T>::value, \
                "Type T must support all comparison operators: "       \
                "<=, =>");

template <typename Enum, typename Lambda>
std::string enum_to_string_impl(Enum e, Lambda lambda) {
  return lambda(e);
}

#define NVM_ENUM_TO_STRING_FORMATTER(EnumType, ...)                  \
                                                                     \
  inline std::string ToStringEnum##EnumType(EnumType e) {            \
    static const auto toStringFunc = [](EnumType e) -> std::string { \
      switch (e) {                                                   \
        __VA_ARGS__                                                  \
        default:                                                     \
          throw std::invalid_argument("Unsupported enum value");     \
      }                                                              \
    };                                                               \
    return nvm::enum_to_string_impl(e, toStringFunc);                \
  }
}  // namespace nvm
#endif