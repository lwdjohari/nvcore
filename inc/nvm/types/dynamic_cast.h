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

#ifndef NVM_CORE_TYPES_V2_DYNAMIC_CAST_H
#define NVM_CORE_TYPES_V2_DYNAMIC_CAST_H

#include <optional>
#include <stdexcept>

namespace nvm::types {

/// @brief Cast Base type to Derived type as pointer.
/// Pointer result will pointing to base object.
/// Make sure the derived pointer is not outlived the base object.
/// @tparam TBase Base type
/// @tparam TDerived Derived type
/// @param base_ptr Ptr to base object
/// @return Ptr to Derived object or std::nullopt when failed
/// @throw None, template is wrapped on try-catch return std::nullopt on
/// exception.
template <typename TBase, typename TDerived>
std::optional<TDerived*> ToDerivedType(TBase* base_ptr) {
  try {
    if (auto derived_ptr = dynamic_cast<TDerived*>(base_ptr)) {
      return derived_ptr;
    }
    return std::nullopt;
  } catch (const std::exception& e) {
    return std::nullopt;
  }
}

}  // namespace nvm::types
#endif
