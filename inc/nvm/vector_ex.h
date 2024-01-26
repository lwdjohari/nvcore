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

#ifndef NVM_CORE_V2_VECTOR_EX_H
#define NVM_CORE_V2_VECTOR_EX_H

#include <algorithm>
#include <cstddef>
#include <vector>

namespace nvm {
namespace vectorex {

/// @brief 
/// @tparam T 
/// @param seq1 
/// @param seq2 
/// @param size 
/// @return 
template <typename T>
inline bool CompareElements(const T *seq1, const T *seq2, size_t size) {
  if (!seq1 && !seq2 && size == 0) return true;
  if (!seq1 || !seq2 || size == 0) return false;

  return std::equal(seq1, seq1 + size, seq2);
}

// Template function to check if a vector contains a specific value
/// @brief
/// @tparam T
/// @param vec
/// @param value
/// @return
template <typename T>
bool Contains(const std::vector<T> &vec, const T &value) {
  return std::find(vec.begin(), vec.end(), value) != vec.end();
}


/// @brief 
/// @tparam T 
/// @param vec 
/// @param value 
/// @return 
template <typename T>
std::pair<bool, size_t> FindInVector(const std::vector<T> &vec,
                                     const T &value) {
  auto it = std::find(vec.begin(), vec.end(), value);

  if (it != vec.end()) {
    // If found, return the index
    size_t index = std::distance(vec.begin(), it);

    // cppcheck-suppress returnStdMoveLocal
    return std::move(std::make_pair(true, index));
  } else {
    // If not found, return false or some other indicator
    // cppcheck-suppress returnStdMoveLocal
    return std::move(std::make_pair(false, 0));
  }
}

/// @brief Template function to find an object in a vector based on a method's
/// return value. Example : \n
/// ```
/// int valueToFind = 2; \n
/// int index = FindInVectorByMethodValue(myVec, valueToFind,
/// &MyClass::getId);\n
/// ```
/// @tparam T
/// @tparam ValueType
/// @tparam MemberFunction
/// @param vec
/// @param valueToFind
/// @param memberFunction
/// @return
template <typename T, typename ValueType, typename MemberFunction>
std::pair<bool, size_t>  FindInVectorByMethodValue(std::vector<T> &vec, ValueType valueToFind,
                                  MemberFunction memberFunction) {
  auto it = std::find_if(vec.begin(), vec.end(),
                         [valueToFind, memberFunction](const T &obj) {
                           return (obj.*memberFunction)() == valueToFind;
                         });

  if (it != vec.end()) {
    // If found, return the index
    size_t index = std::distance(vec.begin(), it);
    // cppcheck-suppress returnStdMoveLocal
    return std::move(std::make_pair(true, index));
  } else {
    // If not found, return false or some other indicator
    // cppcheck-suppress returnStdMoveLocal
    return std::move(std::make_pair(false, 0));
  }
}

/// @brief 
/// @tparam T 
/// @tparam ValueType 
/// @tparam MemberFunction 
/// @param vec 
/// @param value_to_find 
/// @param member_fn 
/// @return 
template <typename T, typename ValueType, typename MemberFunction>
std::pair<bool, size_t>  FindInVectorPointerByMethodValue(std::vector<T> &vec,
                                         ValueType value_to_find,
                                         MemberFunction member_fn) {
  auto it =
      std::find_if(vec.begin(), vec.end(), [value_to_find, member_fn](const T &obj) {
        return ((*obj).*member_fn)() == value_to_find;
      });

  if (it != vec.end()) {
    // If found, return the index
    size_t index = std::distance(vec.begin(), it);
    // cppcheck-suppress returnStdMoveLocal
    return std::move(std::make_pair(true, index));
  } else {
    // If not found, return false or some other indicator
    // cppcheck-suppress returnStdMoveLocal
    return std::move(std::make_pair(false, 0));
  }
}

}  // namespace vectorex
}  // namespace nvm

#endif  // NVM_CORE_V2_VECTOR_EX_H
