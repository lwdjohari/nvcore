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

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace nvm::types::utility {

// Get if the type is same type
template <typename T, typename U>
struct is_type_of : std::is_same<T, U> {};

// Get if the type is same type
template <typename T, typename U>
inline constexpr bool is_type_of_v = is_type_of<T, U>::value;

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
constexpr bool is_raw_ptr_v() {
  return std::is_pointer_v<T>;
}

/// @brief Get is T is std::optional<> type
/// @tparam T any type
/// @return true if std::optional<> type
template <typename T>
constexpr bool is_optional_v() {
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

// template <typename T, typename = std::void_t<>>
// struct has_static_kComponent : std::false_type {};

// template <typename T>
// struct has_static_kComponent<T, std::void_t<decltype(T::kComponent)>>
//                 : std::true_type {};

// Check if TType inherited from TBaseType

template <typename TType, typename TBaseType, typename = std::void_t<>>
struct has_base_of : std::false_type {};

// Specialization that uses SFINAE to set the value to true if TType is derived
// from TBaseType
template <typename TType, typename TBaseType>
struct has_base_of<TType, TBaseType,
                   std::void_t<typename std::enable_if<
                       std::is_base_of<TBaseType, TType>::value>::type>>
                : std::true_type {};

// C++17 and later version
#if __cplusplus >= 201703L
template <typename TType, typename TBaseType>
constexpr bool is_has_base_of_v = has_base_of<TType, TBaseType>::value;
#else
constexpr bool is_has_base_of_v = has_base_of<TType, TBaseType>::value;
#endif

// Helper to determine if a type T is constructible with exact argument forms
// using SFINAE
template <typename T, typename = void, typename... Args>
struct is_constructible_helper : std::false_type {};

template <typename T, typename... Args>
struct is_constructible_helper<
    T, std::enable_if_t<std::is_constructible<T, Args...>::value>, Args...>
                : std::true_type {};

// template <typename T, typename... Args>
// struct is_constructible_helper<T,
// std::void_t<decltype(T(std::declval<Args>()...))>, Args...>
//     : std::conjunction<std::is_constructible<T, Args...>, std::is_same<Args,
//     Args>...> {};

template <typename T, typename... Args>
using is_constructible = is_constructible_helper<T, void, Args...>;

// Define is_constructible_v based on the C++ standard version
#if __cplusplus >= 201703L
// Compile-time check if the type can be constructed using defined constructor.
//
// Example
//
// ```cpp
//
// class BasicComponent {
// public:
//   BasicComponent(){}
//   explicit BasicComponent(int, double) {}
//   }
// };
//
// STATIC_REQUIRE(is_constructible_v<BasicComponent>);
// STATIC_REQUIRE(is_constructible_v<BasicComponent, int, double>);
// STATIC_REQUIRE_FALSE(is_constructible_v<BasicComponent, int, std::string>);
//
// // This is fundamental limitation
// // implicit conversion still happen, it up to dev to avoid this UB
// STATIC_REQUIRE(is_constructible_v<BasicComponent, double, int>);
// ```
template <typename T, typename... Args>
constexpr bool is_constructible_v = is_constructible<T, Args...>::value;
#else
// Compile-time check if the type can be constructed using defined constructor.
//
// Example
//
// ```cpp
//
// class BasicComponent {
// public:
//   BasicComponent(){}
//   explicit BasicComponent(int, double) {}
//   }
// };
//
// STATIC_REQUIRE(is_constructible_v<BasicComponent>);
// STATIC_REQUIRE(is_constructible_v<BasicComponent, int, double>);
// STATIC_REQUIRE_FALSE(is_constructible_v<BasicComponent, int, std::string>);
//
// // This is fundamental limitation
// // implicit conversion still happen, it up to dev to avoid this UB
// STATIC_REQUIRE(is_constructible_v<BasicComponent, double, int>);
// ```
template <typename T, typename... Args>
constexpr bool is_constructible_v() {
  return is_constructible<T, Args...>::value;
}

// Compile-time check if the type can be constructed using defined constructor.
//
// Example
//
// ```cpp
//
// class BasicComponent {
// public:
//   BasicComponent(){}
//   explicit BasicComponent(int, double) {}
//   }
// };
//
// STATIC_REQUIRE(is_constructible_v<BasicComponent>);
// STATIC_REQUIRE(is_constructible_v<BasicComponent, int, double>);
// STATIC_REQUIRE_FALSE(is_constructible_v<BasicComponent, int, std::string>);
//
// // This is fundamental limitation
// // implicit conversion still happen, it up to dev to avoid this UB
// STATIC_REQUIRE(is_constructible_v<BasicComponent, double, int>);
// ```
#define is_constructible_v(T, ...) is_constructible_v<T, __VA_ARGS__>()
#endif

// Macro to define a trait that checks for a specific member function & exact
// signature.
// LIMITATION:
// For declared Func() const, it will detect both Func() & Func() const,
// for any Func const mismatch constness calling,
// it's up to compiler to catch during compilers   
//
// Example:
// ```cpp
// struct A {
// std::string generate(int) {
//     return "Print A";
// }
// std::string toString(int) {
//     return "A";
// }
// void Print() {}
// const int* PrintCache() const {
//     static int x = 0;
//     return &x;
// }
// int& PrintIndex() {
//     static int x = 0;
//     return x;
// }
// };
//
// NVM_DEFINE_TYPE_HAS_METHOD(nvm, has_print, Print)
// NVM_DEFINE_TYPE_HAS_METHOD(nvm, has_print_index, PrintIndex)
// NVM_DEFINE_TYPE_HAS_METHOD(nvm, has_to_string, toString)
// NVM_DEFINE_TYPE_HAS_METHOD(nvm, has_generate, generate)
// NVM_DEFINE_TYPE_HAS_METHOD(nvm, has_print_cache, PrintCache)
//
// #define STATIC_REQUIRE(...) static_assert(__VA_ARGS__, #__VA_ARGS__)
// #define STATIC_REQUIRE_FALSE(...) static_assert(!(__VA_ARGS__), #__VA_ARGS__)
//
// void test_case() {
//     STATIC_REQUIRE_FALSE(nvm_has_print<A, std::string(int)>::value);
//     STATIC_REQUIRE(nvm_has_to_string<A, std::string(int)>::value);
//     STATIC_REQUIRE(nvm_has_generate<A, std::string(int)>::value);
//     STATIC_REQUIRE(nvm_has_print<A, void()>::value);
//     STATIC_REQUIRE(nvm_has_print_cache<A, const int*()>::value);
//     STATIC_REQUIRE(nvm_has_print_index<A, int&()>::value);
// }
// ```
#define NVM_DEFINE_TYPE_HAS_METHOD(PREFIX, TEMPLATE_NAME, FUNC)                \
  template <typename, typename T>                                         \
  struct PREFIX##_##TEMPLATE_NAME : std::false_type {};                   \
                                                                          \
  template <typename TClass, typename Ret, typename... Args>              \
  struct PREFIX##_##TEMPLATE_NAME<TClass, Ret(Args...)> {                 \
   private:                                                               \
    template <typename U>                                                 \
    static auto test(U*)                                                  \
        -> decltype(static_cast<Ret (U::*)(Args...)>(&U::FUNC),           \
                    std::true_type{}) {                                   \
      return std::true_type{};                                            \
    }                                                                     \
                                                                          \
    template <typename>                                                   \
    static std::false_type test(...);                                     \
                                                                          \
    template <typename U>                                                 \
    static auto test_const(U*)                                            \
        -> decltype(static_cast<Ret (U::*)(Args...) const>(&U::FUNC),     \
                    std::true_type{}) {                                   \
      return std::true_type{};                                            \
    }                                                                     \
                                                                          \
    template <typename>                                                   \
    static std::false_type test_const(...);                               \
                                                                          \
    template <typename U>                                                 \
    static auto test_ptr(U*)                                              \
        -> decltype(static_cast<Ret (U::*)(std::add_pointer_t<Args>...)>( \
                        &U::FUNC),                                        \
                    std::true_type{}) {                                   \
      return std::true_type{};                                            \
    }                                                                     \
                                                                          \
    template <typename>                                                   \
    static std::false_type test_ptr(...);                                 \
                                                                          \
    template <typename U>                                                 \
    static auto test_ptr_const(U*)                                        \
        -> decltype(static_cast<Ret (U::*)(std::add_pointer_t<Args>...)   \
                                    const>(&U::FUNC),                     \
                    std::true_type{}) {                                   \
      return std::true_type{};                                            \
    }                                                                     \
                                                                          \
    template <typename>                                                   \
    static std::false_type test_ptr_const(...);                           \
                                                                          \
    template <typename U>                                                 \
    static auto test_ref(U*)                                              \
        -> decltype(static_cast<Ret (U::*)(                               \
                        std::add_lvalue_reference_t<Args>...)>(&U::FUNC), \
                    std::true_type{}) {                                   \
      return std::true_type{};                                            \
    }                                                                     \
                                                                          \
    template <typename>                                                   \
    static std::false_type test_ref(...);                                 \
                                                                          \
    template <typename U>                                                 \
    static auto test_ref_const(U*)                                        \
        -> decltype(static_cast<Ret (U::*)(                               \
                        std::add_lvalue_reference_t<Args>...) const>(     \
                        &U::FUNC),                                        \
                    std::true_type{}) {                                   \
      return std::true_type{};                                            \
    }                                                                     \
                                                                          \
    template <typename>                                                   \
    static std::false_type test_ref_const(...);                           \
                                                                          \
    template <typename U>                                                 \
    static auto test_rref(U*)                                             \
        -> decltype(static_cast<Ret (U::*)(                               \
                        std::add_rvalue_reference_t<Args>...)>(&U::FUNC), \
                    std::true_type{}) {                                   \
      return std::true_type{};                                            \
    }                                                                     \
                                                                          \
    template <typename>                                                   \
    static std::false_type test_rref(...);                                \
                                                                          \
    template <typename U>                                                 \
    static auto test_rref_const(U*)                                       \
        -> decltype(static_cast<Ret (U::*)(                               \
                        std::add_rvalue_reference_t<Args>...) const>(     \
                        &U::FUNC),                                        \
                    std::true_type{}) {                                   \
      return std::true_type{};                                            \
    }                                                                     \
                                                                          \
    template <typename>                                                   \
    static std::false_type test_rref_const(...);                          \
                                                                          \
    using result = std::integral_constant<                                \
        bool, decltype(test_const<TClass>(nullptr))::value ||             \
                  decltype(test<TClass>(nullptr))::value ||               \
                  decltype(test_ptr<TClass>(nullptr))::value ||           \
                  decltype(test_ptr_const<TClass>(nullptr))::value ||     \
                  decltype(test_ref<TClass>(nullptr))::value ||           \
                  decltype(test_ref_const<TClass>(nullptr))::value ||     \
                  decltype(test_rref<TClass>(nullptr))::value ||          \
                  decltype(test_rref_const<TClass>(nullptr))::value>;     \
                                                                          \
   public:                                                                \
    static constexpr bool value = result::value;                          \
  };

//  template<class T>
// struct is_pointer_to_const_member_function : std::false_type {};

// template<class R, class T, class... Args>
// struct is_pointer_to_const_member_function<R (T::*)(Args...) const> :
// std::true_type {};

// template<class R, class T, class... Args>
// struct is_pointer_to_const_member_function<R (T::*)(Args...) const &> :
// std::true_type {};

// template<class R, class T, class... Args>
// struct is_pointer_to_const_member_function<R (T::*)(Args...) const &&> :
// std::true_type {};

// template<class R, class T, class... Args>
// struct is_pointer_to_const_member_function<R (T::*)(Args..., ...) const> :
// std::true_type {};

// template<class R, class T, class... Args>
// struct is_pointer_to_const_member_function<R (T::*)(Args..., ...) const &>
// : std::true_type {};

// template<class R, class T, class... Args>
// struct is_pointer_to_const_member_function<R (T::*)(Args..., ...) const &&>
// : std::true_type {};


}  // namespace nvm::types::utility
