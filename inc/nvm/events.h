/*
 *  Copyright (c) 2020 Linggawasistha Djohari
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

#ifndef NVM_CORE_V2_EVENTS_H
#define NVM_CORE_V2_EVENTS_H

namespace nvm {
namespace events {

class Arg {
 public:
    
    /// @brief Default constructor for Arg class.
    Arg(){};

    /// @brief Destructor for Arg class.
    ~Arg(){};
};

/// @brief 
/// @tparam TSender 
/// @tparam TEventValue 
template <typename TSender, typename TEventValue>
class EventArg {
 private:
  TSender sender_;
  TEventValue value_;

 public:
  EventArg(TSender sender, TEventValue value)
      : sender_(sender), value_(value) {}

  virtual ~EventArg() {}

  auto Sender() {
    // Check if TSender is a pointer type
    if constexpr (std::is_pointer<TSender>::value) {
      // If TSender is already a pointer, return it directly
      return sender_;
    } else {
      // If TSender is not a pointer, return its address
      return &sender_;
    }
  }

  // Function to return value as a pointer
  auto Value() {
    // Check if TEventValue is a pointer type
    if constexpr (std::is_pointer<TEventValue>::value) {
      // If TEventValue is already a pointer, return it directly
      return value_;
    } else {
      // If TEventValue is not a pointer, return its address
      return &value_;
    }
  }
};

}  // namespace events
}  // namespace nvm

#endif  // NVM_CORE_V2_EVENTS_H
