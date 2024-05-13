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

#ifndef NVM_CORE_V2_LOGIC_H
#define NVM_CORE_V2_LOGIC_H

#include <algorithm>
#include <type_traits>

namespace nvm {
namespace logic {

/**
 * @brief Represents a range of values.
 *
 * The Range struct template is used to define a range of values of type T.
 * Only arithmetic types are allowed for the Range struct template.
 *
 * @tparam T The type of values in the range.
 */
template <typename T>
struct Range {
  static_assert(std::is_arithmetic<T>::value,
                "Only arithmetic types are allowed for Range struct template.");

  T start; /**< The start value of the range. */
  T end;   /**< The end value of the range. */

  /**
   * @brief Default constructor.
   * Initializes the start and end values to their default values.
   */
  explicit Range() : start(T()), end(T()){};

  /**
   * @brief Constructor with specified start and end values.
   * @param start The start value of the range.
   * @param end The end value of the range.
   */
  explicit Range(T start, T end) : start(start), end(end){};

  /**
   * @brief Check if the range is valid.
   * A range is considered valid if the start value is less than or equal to the
   * end value.
   * @return true if the range is valid, false otherwise.
   */
  bool IsValid() const { return start <= end; }

  /**
   * Calculates the distance.
   * @return The distance.
   */
  T Distance() { return end - start; }
};

/// @brief Get the value is between range supplied. This method use
/// greater-or-equal comparator & lesser-or-equal comparator. operator.
/// @tparam T Arithmatic type value
/// @param value value to test
/// @param start range start
/// @param end range end
/// @return true if value between range.
template <typename T>
bool IsBetween(const T &value, const T &start, const T &end) {
  static_assert(std::is_arithmetic<T>::value,
                "Only arithmetic types are allowed for IsBetween template.");

  return value >= start && value <= end;
};

/// @brief Get the range is intersect with other range. This method use
/// greater-or-equal comparator & lesser-or-equal comparator. operator.
/// @tparam T Arithmatic type value
/// @param start1 range start 1
/// @param end1 range end 1
/// @param start2 range start 2
/// @param end2 range end 2
/// @return true if range intersect one another.
template <typename T>
bool IsIntersect(const T &start1, const T &end1, const T &start2,
                 const T &end2) {
  static_assert(std::is_arithmetic<T>::value,
                "Only arithmetic types are allowed for IsIntersect template.");

  return (start1 <= end2) && (end1 >= start2);
};

/// @brief Determines whether two ranges intersect.
/// @tparam Arithmatic type value.
/// @param range1 The first range.
/// @param range2 The second range.
/// @return True if the ranges intersect, false otherwise.
template <typename T>
bool IsIntersect(const Range<T> &range1, const Range<T> &range2) {
  if (!range1.IsValid() || !range2.IsValid()) return false;
  return IsIntersect(range1.start, range1.end, range2.start, range2.end);
};

/// @brief Get the start & end value that intersect. This method use
/// greater-or-equal comparator & lesser-or-equal comparator.
/// @tparam T Arithmatic type value
/// @param start1 range start 1
/// @param end1 range end 1
/// @param start2 range start 2
/// @param end2 range end 2
/// @return std::pair<default(),default()> if is not intersect one another.
template <typename T>
Range<T> GetIntersect(const T &start1, const T &end1, const T &start2,
                      const T &end2) {
  static_assert(std::is_arithmetic<T>::value,
                "Only arithmetic types are allowed for GetIntersect template.");
  if ((start1 <= end2) && (end1 >= start2)) {
    // Check for overlap
    // Calculate the intersection
    T intersectStart = std::max(start1, start2);
    T intersectEnd = std::min(end1, end2);

    // cppcheck-suppress returnStdMoveLocal
    return std::move(Range(intersectStart, intersectEnd));
  } else {
    // Return a pair of default-constructed T values to indicate no overlap

    // cppcheck-suppress returnStdMoveLocal
    return std::move(Range(T(), T()));
  }
};

/**
 * Calculates the intersection of two ranges.
 *
 * @tparam T Arithmatic type value.
 * @param range1 The first range.
 * @param range2 The second range.
 * @return A pair representing the intersection of the two ranges.
 */
template <typename T>
Range<T> GetIntersect(const Range<T> &range1, const Range<T> &range2) {
  if (!range1.IsValid() || !range2.IsValid()) {
    // cppcheck-suppress returnStdMoveLocal
    return std::move(Range(T(), T()));
  }

  return GetIntersect(range1.start, range1.end, range2.start, range2.end);
};

/**
 * @brief A class that represents a flag that can be signaled once.
 */
class FlagOnce {
 private:
  bool signaled = false;

 public:
  /**
   * @brief Default constructor.
   */
  FlagOnce() = default;

  /**
   * @brief Signals the flag.
   */
  void Signal() { signaled = true; }

  /**
   * @brief Resets the flag.
   */
  void Reset() { signaled = false; }

  /**
   * @brief Checks if the flag has been signaled.
   * @return true if the flag has been signaled, false otherwise.
   */
  bool IsSignaled() const { return signaled; }
};

}  // namespace logic
}  // namespace nvm

#endif  // NVM_CORE_V2_LOGIC_H
