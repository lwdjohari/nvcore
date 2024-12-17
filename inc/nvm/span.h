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

#include <array>
#include <cstddef>      // For size_t, ptrdiff_t
#include <iterator>     // For iterators
#include <stdexcept>    // For std::out_of_range
#include <type_traits>  // For std::enable_if, std::is_const, etc.
#include <vector>

namespace nvm {
template <typename T>
class Span {
 public:
  // Type aliases
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using iterator = T*;
  using const_iterator = const T*;

  // Constructors
  constexpr Span() noexcept : data_(nullptr), size_(0) {}

  constexpr Span(T* ptr, size_type size) noexcept : data_(ptr), size_(size) {}

  // From raw array
  template <std::size_t N>
  constexpr Span(T (&arr)[N]) noexcept : data_(arr), size_(N) {}

  // From std::array
  template <std::size_t N>
  constexpr Span(std::array<T, N>& arr) noexcept
                  : data_(arr.data()), size_(N) {}

  template <std::size_t N>
  constexpr Span(const std::array<T, N>& arr) noexcept
                  : data_(arr.data()), size_(N) {}

  // From std::vector
  template <typename Alloc>
  constexpr Span(std::vector<T, Alloc>& vec) noexcept
                  : data_(vec.data()), size_(vec.size()) {}

  template <typename Alloc>
  constexpr Span(const std::vector<T, Alloc>& vec) noexcept
                  : data_(vec.data()), size_(vec.size()) {}

  // Move semantics
  constexpr Span(Span&& other) noexcept
                  : data_(other.data_), size_(other.size_) {
    other.data_ = nullptr;
    other.size_ = 0;
  }

  constexpr Span& operator=(Span&& other) noexcept {
    if (this != &other) {
      data_ = other.data_;
      size_ = other.size_;
      other.data_ = nullptr;
      other.size_ = 0;
    }
    return *this;
  }

  // Element access
  constexpr reference operator[](size_type idx) const noexcept {
    return data_[idx];
  }

  constexpr reference At(size_type idx) const {
    if (idx >= size_)
      throw std::out_of_range("Span: Index out of bounds");
    return data_[idx];
  }

  constexpr pointer Data() const noexcept {
    return data_;
  }
  constexpr size_type Size() const noexcept {
    return size_;
  }
  constexpr bool Empty() const noexcept {
    return size_ == 0;
  }

  // Iterators
  constexpr iterator Begin() const noexcept {
    return data_;
  }
  constexpr iterator End() const noexcept {
    return data_ + size_;
  }
  constexpr const_iterator Cbegin() const noexcept {
    return data_;
  }
  constexpr const_iterator Cend() const noexcept {
    return data_ + size_;
  }

  // Subspan (Slice) functionality
  constexpr Span Subspan(size_type offset,
                         size_type count = size_type(-1)) const {
    if (offset > size_) {
      throw std::out_of_range("Span: Offset out of bounds");
    }

    size_type new_size = (count == size_type(-1) || offset + count > size_)
                             ? size_ - offset
                             : count;

    return Span(data_ + offset, new_size);
  }

  // Create Span from first N elements
  constexpr Span First(size_type count = size_type(-1)) const {
    if (count > size_) {
      throw std::out_of_range("Span::First: count exceeds span size");
    }
    return Span(data_, count);
  }

  // Create Span from last N elements
  constexpr Span Last(size_type count = size_type(-1)) const {
    if (count > size_) {
      throw std::out_of_range("Span::Last: count exceeds span size");
    }
    return Span(data_ + (size_ - count), count);
  }

 private:
  T* data_;
  size_type size_;

  //constexpr bool check_for_invalidate_pointer_;
  // constexpr void CheckPointerValidity() const {
  //   if (check_for_invalidate_pointer_ && source_container_ && source_container_->data() != data_) {
  //     throw std::runtime_error("Span: underlying container has been reallocated");
  //   }
  // }
};
}  // namespace nvm
