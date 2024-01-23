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
  Type(const Type &) = delete;                       \
  Type &operator=(const Type &) = delete;            \
  Type(Type &&) noexcept = default;                  \
  Type &operator=(Type &&) noexcept = default;

#define NVM_ENUM_CLASS_DISPLAY_TRAIT(E)                                  \
  std::ostream &operator<<(std::ostream &os, E e) {                      \
    return os << static_cast<typename std::underlying_type<E>::type>(e); \
  }

}  // namespace nvm
#endif