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

#ifndef NVM_CORE_MATHS_V2_APPROX_H
#define NVM_CORE_MATHS_V2_APPROX_H

#include <type_traits>

namespace nvm {
namespace maths {

/**
 * @brief A class for approximate comparison of floating-point values.
 *
 * The Approx class provides a way to compare floating-point values with a
 * certain tolerance. It is useful when exact equality is not required and a
 * small difference is acceptable.
 *
 * @tparam T The type of the floating-point value.
 */
template <typename T>
class Approx {
 public:
  /**
   * @brief Constructs an Approx object with the given value and epsilon.
   *
   * @param value The value to be compared.
   * @param epsilon The tolerance for comparison. Defaults to 1e-9.
   */
  explicit Approx(T value, T epsilon = static_cast<T>(1e-9))
      : value_(value), epsilon_(epsilon) {}

  /**
   * @brief Compares a floating-point value with the Approx object.
   *
   * @param lhs The floating-point value to be compared.
   * @param rhs The Approx object to compare with.
   * @return true if the difference between lhs and rhs.value_ is less than
   * rhs.epsilon_, false otherwise.
   */
  friend bool operator==(T lhs, const Approx& rhs) {
    return std::abs(lhs - rhs.value_) < rhs.epsilon_;
  }

  /**
   * @brief Compares the Approx object with a floating-point value.
   *
   * @param lhs The Approx object to compare.
   * @param rhs The floating-point value to be compared with.
   * @return true if the difference between lhs.value_ and rhs is less than
   * lhs.epsilon_, false otherwise.
   */
  friend bool operator==(const Approx& lhs, T rhs) {
    return std::abs(lhs.value_ - rhs) < lhs.epsilon_;
  }

  /**
   * @brief Sets a new epsilon value for the Approx object.
   *
   * @param newEpsilon The new tolerance value.
   * @return Reference to the modified Approx object.
   */
  Approx& Epsilon(T newEpsilon) {
    epsilon_ = newEpsilon;
    return *this;
  }

 private:
  T value_;    ///< The value to be compared.
  T epsilon_;  ///< The tolerance for comparison.
};

}  // namespace maths
}  // namespace nvm

#endif  // NVM_CORE_MATHS_V2_APPROX_H
