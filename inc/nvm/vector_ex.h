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

namespace nvm {
namespace vectorex {

template <typename T>
inline bool CompareArrays(const T *seq1, const T *seq2, size_t size) {
  if (!seq1 && !seq2 && size == 0) return true;
  if (!seq1 || !seq2 || size == 0) return false;

  return std::equal(seq1, seq1 + size, seq2);
}

}  // namespace vectorex
}  // namespace nvm

#endif  // NVM_CORE_V2_VECTOR_EX_H
