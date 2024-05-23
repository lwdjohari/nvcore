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

namespace nvm::structMapper {

template <typename From, typename To>
struct TypeCaster {
  static To cast(const From& from) {
    return static_cast<To>(from);
  }
};

template <>
struct TypeCaster<int32_t, uint32_t> {
  static uint32_t cast(const int32_t& from) {
    return static_cast<uint32_t>(from);
  }
};

template <>
struct TypeCaster<int64_t, std::chrono::system_clock::time_point> {
  static std::chrono::system_clock::time_point cast(const int64_t& from) {
    return std::chrono::system_clock::time_point(
        std::chrono::system_clock::duration(from));
  }
};

template <typename T, typename Tuple, std::size_t... I>
T MakeStructFromTuple_Impl(const Tuple& t, std::index_sequence<I...>) {
  return T{TypeCaster<std::tuple_element_t<I, Tuple>,
                      typename std::tuple_element<I, typename T::types>::type>::
               cast(std::get<I>(t))...};
}

template <typename T, typename Tuple>
T MakeStructFromTuple(const Tuple& t) {
  return MakeStructFromTuple_Impl<T>(
      t, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
}

}  // namespace nvm::structMapper
