/*
 *  Copyright (c) 2024 Linggawasistha Djohari
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

#pragma once
#include <chrono>
#include <cstdint>
#include <string>
#include <tuple>

namespace nvm::mapper {

template <typename TSourceType, typename TDestType>
struct TypeCaster {
  static TDestType Cast(const TSourceType& source) {
    static_assert(std::is_convertible_v<TSourceType, TDestType>,
                  "No suitable conversion found for these types.");
    return static_cast<TDestType>(source);
  }
};

static std::string ToString(const char* from) {
  return std::string(from);
};

template <>
struct TypeCaster<const char*, std::string> {
  static std::string Cast(const char* from) {
    return std::string(from);
  }
};

template <>
struct TypeCaster<int32_t, uint32_t> {
  static uint32_t Cast(const int32_t& source) {
    return static_cast<uint32_t>(source);
  }
};

template <>
struct TypeCaster<int64_t, std::chrono::system_clock::time_point> {
  static std::chrono::system_clock::time_point Cast(const int64_t& source) {
    return std::chrono::system_clock::time_point(
        std::chrono::system_clock::duration(source));
  }
};

template <>
struct TypeCaster<int8_t, std::string> {
  static std::string Cast(const int8_t& source) {
    return std::to_string(source);
  }
};

template <>
struct TypeCaster<int16_t, std::string> {
  static std::string Cast(const int16_t& source) {
    return std::to_string(source);
  }
};

template <>
struct TypeCaster<int32_t, std::string> {
  static std::string Cast(const int32_t& source) {
    return std::to_string(source);
  }
};

template <>
struct TypeCaster<int64_t, std::string> {
  static std::string Cast(const int64_t& source) {
    return std::to_string(source);
  }
};

template <>
struct TypeCaster<uint8_t, std::string> {
  static std::string Cast(const uint8_t& source) {
    return std::to_string(source);
  }
};

template <>
struct TypeCaster<uint16_t, std::string> {
  static std::string Cast(const uint16_t& source) {
    return std::to_string(source);
  }
};

template <>
struct TypeCaster<uint32_t, std::string> {
  static std::string Cast(const uint32_t& source) {
    return std::to_string(source);
  }
};

template <>
struct TypeCaster<uint64_t, std::string> {
  static std::string Cast(const uint64_t& source) {
    return std::to_string(source);
  }
};

template <>
struct TypeCaster<float, std::string> {
  static std::string Cast(const float& source) {
    return std::to_string(source);
  }
};

template <>
struct TypeCaster<double, std::string> {
  static std::string Cast(const double& source) {
    return std::to_string(source);
  }
};

template <typename T, typename Tuple, std::size_t... I>
T MakeStructFromTuple_Impl(const Tuple& t, std::index_sequence<I...>) {
  return T{TypeCaster<std::tuple_element_t<I, Tuple>,
                      typename std::tuple_element<I, typename T::types>::type>::
               Cast(std::get<I>(t))...};
}

template <typename T, typename Tuple>
T MakeStructFromTuple(const Tuple& t) {
  return MakeStructFromTuple_Impl<T>(
      t, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
}

/// @brief Generic struct mapper template
/// @tparam TFrom
/// @tparam TDest
template <typename TFrom, typename TDest>
struct Mapper {
  static TDest Map(const TFrom& from);
};

}  // namespace nvm::mapper
