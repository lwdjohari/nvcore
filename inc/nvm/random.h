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

#include <chrono>
#include <limits>
#include <random>

namespace nvm {
namespace utils {

// Template for random number generation with user-specified min and max
template <typename T>
static inline T Randomize(T min, T max) {
  std::default_random_engine generator(
      std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<T> distribution(min, max);
  return distribution(generator);
}

// Template for random number generation
template <typename T>
static inline T Randomize() {
  static constexpr T min = std::numeric_limits<T>::min();
  static constexpr T max = std::numeric_limits<T>::max();

  return Randomize<T>(min, max);
}

// Randomize for uint32_t
static inline uint32_t RandomizeUint32t() {
  static constexpr uint32_t min = std::numeric_limits<uint32_t>::min();
  static constexpr uint32_t max = std::numeric_limits<uint32_t>::max();

  return Randomize<uint32_t>(min, max);
}

// Randomize for int32_t
static inline int32_t RandomizeInt32t() {
  static constexpr int32_t min = std::numeric_limits<int32_t>::min();
  static constexpr int32_t max = std::numeric_limits<int32_t>::max();

  return Randomize<int32_t>(min, max);
}

// Randomize for uint64_t
static inline uint64_t RandomizeUint64t() {
  static constexpr uint64_t min = std::numeric_limits<uint64_t>::min();
  static constexpr uint64_t max = std::numeric_limits<uint64_t>::max();

  return Randomize<uint64_t>(min, max);
}

// Randomize for int64_t
static inline int64_t RandomizeInt64t() {
  static constexpr int64_t min = std::numeric_limits<int32_t>::min();
  static constexpr int64_t max = std::numeric_limits<int32_t>::max();

  return Randomize<int64_t>(min, max);
}

}  // namespace utils

}  // namespace nvm