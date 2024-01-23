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

#ifndef NVM_CORE_BYTES_V2_BYTE_DECLARATION_H
#define NVM_CORE_BYTES_V2_BYTE_DECLARATION_H

#include <cstdint>
#include <type_traits>

namespace nvm {
namespace bytes {

enum class EndianessType : uint8_t {
  LittleEndian = 0,
  BigEndian = 1,
  Mixed = 2,
};

// cppcheck-suppress unknownMacro
NVM_ENUM_CLASS_DISPLAY_TRAIT(EndianessType)

enum class ByteOpResult {
  Ok = 0,
  Nullptr = 1,
  SizeMismatch = 2,
  None = 4,
};


NVM_ENUM_CLASS_DISPLAY_TRAIT(ByteOpResult)

}  // namespace bytes
}  // namespace nvm

#endif  // NVM_CORE_BYTES_V2_BYTE_DECLARATION_H
