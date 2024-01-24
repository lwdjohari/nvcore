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

#ifndef NVM_CORE_V2_MEMORY_H
#define NVM_CORE_V2_MEMORY_H

#include <algorithm>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>

namespace nvm {
namespace memory {

/// @brief Forward pointer n-element. No bound checking.
/// @tparam T
/// @param ptr start of pointer
/// @param n number of elements to move forward.
/// @return nullptr if failed.
/// @note UNSAFE: Memory Address Violation could occurs if out-of-bound.
template <typename T>
T *PtrFwd(T *ptr, const size_t &n) {
  if (!ptr) return nullptr;

  return ptr + n;
}

/// @brief Forward pointer n-element with bound checking.
/// @tparam T
/// @param begin pointer of start element
/// @param end end of sequence pointer
/// @param n number of elements to move forward
/// @return nullptr if out-of bound or failed.
template <typename T>
T *PtrFwd(T *begin, const T *end, const size_t &n) {
  if (!begin || !end) return nullptr;
  bool out_of_bound = (begin + n) > end;

  if (out_of_bound) return nullptr;
  return begin + n;
}

/// @brief Rewind pointer n-element with no bound checking.
/// @tparam T
/// @param ptr pointer of current position
/// @param n number elements to rewind.
/// @return nullptr if failed.
/// @note UNSAFE: Memory Address Violation could occurs if out-of-bound.
template <typename T>
T *PtrRwd(const T *ptr, const size_t &n) {
  if (!ptr) return nullptr;

  return ptr - n;
}

/// @brief Rewind pointer n-element with bound checking.
/// @tparam T
/// @param begin the begin or first element pointer
/// @param current pointer of current position
/// @param n number elements to rewind
/// @return nullptr if out-of-bound or failed
template <typename T>
T *PtrRwd(const T *begin, const T *current, const size_t &n) {
  if (!begin || !current) return nullptr;
  bool out_of_bound = (current - begin) < n;

  if (out_of_bound) return nullptr;

  return current - n;
}

/// @brief Initialize default element for fundamental types.\n
/// @details Fundamentals type are : \n
/// - Integral Types: ```bool```, ```char```, ```signed char```,
///```unsigned char```, ```wchar_t```, ```char16_t```, ```char32_t```,
/// ```short```, ```int```, ```long```, ```long long``` and their ```unsigned```
/// variants.\n
/// - Floating-Point Types: ```float```, ```double```, ```long double```.
/// - Void Type: ```void```.
/// - Null Pointer Type: ```std::nullptr_t```
/// @tparam T Fundamental types
/// @param el Pointer to fist element (array, vector or any contagious memory
/// block).
/// @param size n-elements (n-element is not bytes size).
/// @return true if initialization successed.
template <typename T>
bool InitializeDefaultElements(T *el, const size_t &size) {
  static_assert(std::is_integral<T>::value,
                "T must be a primitive or fundamental type.");

  if (!el) return false;
  std::memset(el, 0, sizeof(T) * size);
  return true;
}

/// @brief Initialize default element for fundamental types.\n
/// @details Fundamentals type are : \n
/// - Integral Types: ```bool```, ```char```, ```signed char```,
///```unsigned char```, ```wchar_t```, ```char16_t```, ```char32_t```,
/// ```short```, ```int```, ```long```, ```long long``` and their ```unsigned```
/// variants.\n
/// - Floating-Point Types: ```float```, ```double```, ```long double```.
/// - Void Type: ```void```.
/// - Null Pointer Type: ```std::nullptr_t```
/// @tparam T Fundamental types
/// @param el Pointer to fist element (array, vector or any contagious memory
/// block).
/// @param size n-elements (n-element is not bytes size).
/// @return true if initialization successed.
template <typename T>
bool FillElements(T *el, const T &val, const size_t &size) {
  static_assert(std::is_integral<T>::value,
                "T must be a primitive or fundamental type.");

  if (!el) return false;
  std::fill_n(el, size, val);
  return true;
}

/// @brief Move memory from std::allocator to std::shared_ptr.
/// The ownership and the memory lifetime is transfered to std::shared_ptr.
/// Original allocator will not maintain the moved object after the object is
/// sucessfuly moved to std::shared_ptr.
/// @tparam T object type
/// @tparam TAllocator original std::allocator that allocated memory for the
/// object that being moved
/// @param ptr_to_obj_in_allocator pointer to object in std::allocater
/// @param allocator Original allocator, the one that allocated memory for the
/// object currently being moved
/// @param size size of element, default never more than 1-element.
/// @return nullptr if unsuccessful, std::shared_ptr with object that being
/// moved if success.
template <typename T, typename TAllocator = std::allocator<T>>
std::shared_ptr<T> MoveFromAllocatorAsSharedPtr(
    T *ptr_to_obj_in_allocator, TAllocator &allocator, const size_t &size = 1,
    const bool &call_destructor = true) noexcept {
  if (!ptr_to_obj_in_allocator || size == 0) return nullptr;

  std::shared_ptr<T> moved_object(new (std::nothrow)
                                      T(std::move(*ptr_to_obj_in_allocator)));
  if (!moved_object) return nullptr;

  if (call_destructor) allocator.destroy(ptr_to_obj_in_allocator);
  allocator.deallocate(ptr_to_obj_in_allocator, size);

  return moved_object;
}

template <typename T, typename TAllocator = std::allocator<T>>
T MoveFromAllocator(T *ptr_to_obj_in_allocator, TAllocator &allocator,
                    const size_t &size = 1,
                    const bool &call_destructor = true) noexcept {
  if (!ptr_to_obj_in_allocator || size == 0) return nullptr;

  T moved_object(new (std::nothrow) T(std::move(*ptr_to_obj_in_allocator)));

  if (call_destructor) allocator.destroy(ptr_to_obj_in_allocator);
  allocator.deallocate(ptr_to_obj_in_allocator, size);

  return std::move(moved_object);
}

}  // namespace memory
}  // namespace nvm

#endif  // NVM_CORE_V2_MEMORY_H
