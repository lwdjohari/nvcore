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

#ifndef NVM_CORE_BYTES_DETAILS_V2_INTERNAL_BYTE_U8_H
#define NVM_CORE_BYTES_DETAILS_V2_INTERNAL_BYTE_U8_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>

#include "nvm/bytes/byte_declaration.h"

#ifndef __STDC_IEC_559__
#error \
    "Floating-point operations must conform to the IEEE 754 Standard, please change the compilers!"
#endif

#define ENDIANESS_LITTLE_ENDIAN 0
#define ENDIANESS_BIG_ENDIAN 1
#define ENDIANESS_MIXED 2

#if defined(__GNUC__) || defined(__clang__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define NVM_HOST_ENDIAN 0
#else
#define NVM_HOST_ENDIAN 1
#endif
#else
// Mixed endian platforms
// Unsupported
#error "Unsupported platform: Endianness is unknown."
#endif

namespace nvm {
namespace bytes {
namespace details {
namespace u8 {

inline void CopyBytes(const uint8_t *src, uint8_t *dest, size_t size,
                      ByteOpResult &err) noexcept {
  if (size == 0) {
    err = ByteOpResult::SizeMismatch;
    return;
  }

  if (!src || !dest) {
    err = ByteOpResult::Nullptr;
    return;
  }

  // size always 1
  std::memcpy(dest, src, size);
}

inline void EncodeInt8(const int8_t &value, uint8_t *buffer, size_t size,
                       ByteOpResult &err) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return;
  }

  if (size < 1) {
    err = ByteOpResult::SizeMismatch;
    return;
  }

  buffer[0] = static_cast<uint8_t>(int8_t(value));
  err = ByteOpResult::Ok;
}

inline void EncodeUInt8(const uint8_t &value, uint8_t *buffer, size_t size,
                        ByteOpResult &err) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return;
  }

  if (size < 1) {
    err = ByteOpResult::SizeMismatch;
    return;
  }

  buffer[0] = uint8_t(value);
  err = ByteOpResult::Ok;
}

inline void EncodeInt16(const int16_t *value, uint8_t *buffer, size_t size,
                        ByteOpResult &err, bool is_big_endian) {
  if (!buffer || !value) {
    err = ByteOpResult::Nullptr;
    return;
  }

  if (size < 2) {
    err = ByteOpResult::SizeMismatch;
    return;
  }

  alignas(int16_t)
      std::aligned_storage<sizeof(int16_t), alignof(int16_t)>::type align;
  std::memcpy(&align, value, sizeof(int16_t));

  // Retrieve the pointer value from the aligned storage
  int16_t *copied_ptr = reinterpret_cast<int16_t *>(&align);
#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN
  if (!is_big_endian) {
    buffer[0] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
  } else {
    buffer[1] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[0] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
  }
#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  if (is_big_endian) {
    buffer[0] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
  } else {
    buffer[1] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[0] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
  }
#endif
  err = ByteOpResult::Ok;
}

inline void EncodeUInt16(const uint16_t *value, uint8_t *buffer, size_t size,
                         ByteOpResult &err, bool is_big_endian) {
  if ((!buffer) | (!value)) {
    err = ByteOpResult::Nullptr;
    return;
  }

  if (size < 4) {
    err = ByteOpResult::SizeMismatch;
    return;
  }

  alignas(uint16_t)
      std::aligned_storage<sizeof(uint16_t), alignof(uint16_t)>::type align;
  std::memcpy(&align, value, sizeof(uint16_t));

  // Retrieve the pointer value from the aligned storage
  uint16_t *copied_ptr = reinterpret_cast<uint16_t *>(&align);

#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN
  if (!is_big_endian) {
    buffer[0] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
  } else {
    buffer[1] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[0] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
  }
#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  if (is_big_endian) {
    buffer[0] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
  } else {
    buffer[1] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[0] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
  }
#endif
  err = ByteOpResult::Ok;
}

inline void EncodeInt32(const int32_t *value, uint8_t *buffer, size_t size,
                        ByteOpResult &err, bool is_big_endian) {
  if (!buffer || !value) {
    err = ByteOpResult::Nullptr;
    return;
  }

  if (size < 4) {
    err = ByteOpResult::SizeMismatch;
    return;
  }

  alignas(int32_t)
      std::aligned_storage<sizeof(int32_t), alignof(int32_t)>::type align;
  std::memcpy(&align, value, sizeof(int32_t));

  // Retrieve the pointer value from the aligned storage
  int32_t *copied_ptr = reinterpret_cast<int32_t *>(&align);

#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN

  if (!is_big_endian) {
    buffer[0] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[3] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
  } else {
    buffer[3] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[0] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
  }
#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  if (is_big_endian) {
    buffer[0] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[3] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
  } else {
    buffer[3] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[0] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
  }
#endif
  err = ByteOpResult::Ok;
}

inline void EncodeUInt32(const uint32_t *value, uint8_t *buffer, size_t size,
                         ByteOpResult &err, bool is_big_endian) {
  if (!buffer || !value) {
    err = ByteOpResult::Nullptr;
    return;
  }

  if (size < 4) {
    err = ByteOpResult::SizeMismatch;
    return;
  }

  // inline void *alignedPtr;
  // bool isAligned = std::align(alignof(uint32_t), sizeof(uint32_t),
  // alignedPtr, sizeof(uint32_t), reinterpret_cast<inline void *>(value));

  alignas(uint32_t)
      std::aligned_storage<sizeof(uint32_t), alignof(uint32_t)>::type align;
  std::memcpy(&align, value, sizeof(uint32_t));

  // Retrieve the pointer value from the aligned storage
  // Ainline void stack allocation by direct access to align
  uint32_t *copied_ptr = reinterpret_cast<uint32_t *>(&align);

#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN

  if (!is_big_endian) {
    buffer[0] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[3] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
  } else {
    buffer[3] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[0] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
  }
#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  if (is_big_endian) {
    buffer[0] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[3] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
  } else {
    buffer[3] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[0] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
  }
#endif
  err = ByteOpResult::Ok;
}

inline void EncodeInt64(const int64_t *value, uint8_t *buffer, size_t size,
                        ByteOpResult &err, bool is_big_endian) {
  if (!buffer || !value) {
    err = ByteOpResult::Nullptr;
    return;
  }

  if (size < 8) {
    err = ByteOpResult::SizeMismatch;
    return;
  }

  alignas(int64_t)
      std::aligned_storage<sizeof(int64_t), alignof(int64_t)>::type align;
  std::memcpy(&align, value, sizeof(int64_t));

  // Retrieve the pointer value from the aligned storage
  int64_t *copied_ptr = reinterpret_cast<int64_t *>(&align);
#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN
  if (!is_big_endian) {
    buffer[0] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[3] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
    buffer[4] = static_cast<uint8_t>((*copied_ptr >> 32) & 0xff);
    buffer[5] = static_cast<uint8_t>((*copied_ptr >> 40) & 0xff);
    buffer[6] = static_cast<uint8_t>((*copied_ptr >> 48) & 0xff);
    buffer[7] = static_cast<uint8_t>((*copied_ptr >> 56) & 0xff);
  } else {
    buffer[7] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[6] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[5] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[4] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
    buffer[3] = static_cast<uint8_t>((*copied_ptr >> 32) & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 40) & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 48) & 0xff);
    buffer[0] = static_cast<uint8_t>((*copied_ptr >> 56) & 0xff);
  }

#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  if (is_big_endian) {
    buffer[0] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[3] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
    buffer[4] = static_cast<uint8_t>((*copied_ptr >> 32) & 0xff);
    buffer[5] = static_cast<uint8_t>((*copied_ptr >> 40) & 0xff);
    buffer[6] = static_cast<uint8_t>((*copied_ptr >> 48) & 0xff);
    buffer[7] = static_cast<uint8_t>((*copied_ptr >> 56) & 0xff);
  } else {
    buffer[7] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[6] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[5] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[4] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
    buffer[3] = static_cast<uint8_t>((*copied_ptr >> 32) & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 40) & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 48) & 0xff);
    buffer[0] = static_cast<uint8_t>((*copied_ptr >> 56) & 0xff);
  }
#endif
  err = ByteOpResult::Ok;
}

inline void EncodeUInt64(const uint64_t *value, uint8_t *buffer, size_t size,
                         ByteOpResult &err, bool is_big_endian) {
  if (!buffer || !value) {
    err = ByteOpResult::Nullptr;
    return;
  }

  if (size < 8) {
    err = ByteOpResult::SizeMismatch;
    return;
  }

  alignas(uint64_t)
      std::aligned_storage<sizeof(uint64_t), alignof(uint64_t)>::type align;
  std::memcpy(&align, value, sizeof(uint64_t));

  // Retrieve the pointer value from the aligned storage
  uint64_t *copied_ptr = reinterpret_cast<uint64_t *>(&align);

#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN

  if (!is_big_endian) {
    buffer[0] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[3] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
    buffer[4] = static_cast<uint8_t>((*copied_ptr >> 32) & 0xff);
    buffer[5] = static_cast<uint8_t>((*copied_ptr >> 40) & 0xff);
    buffer[6] = static_cast<uint8_t>((*copied_ptr >> 48) & 0xff);
    buffer[7] = static_cast<uint8_t>((*copied_ptr >> 56) & 0xff);
  } else {
    buffer[7] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[6] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[5] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[4] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
    buffer[3] = static_cast<uint8_t>((*copied_ptr >> 32) & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 40) & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 48) & 0xff);
    buffer[0] = static_cast<uint8_t>((*copied_ptr >> 56) & 0xff);
  }
#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  if (is_big_endian) {
    buffer[0] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[3] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
    buffer[4] = static_cast<uint8_t>((*copied_ptr >> 32) & 0xff);
    buffer[5] = static_cast<uint8_t>((*copied_ptr >> 40) & 0xff);
    buffer[6] = static_cast<uint8_t>((*copied_ptr >> 48) & 0xff);
    buffer[7] = static_cast<uint8_t>((*copied_ptr >> 56) & 0xff);
  } else {
    buffer[7] = static_cast<uint8_t>(*copied_ptr & 0xff);
    buffer[6] = static_cast<uint8_t>((*copied_ptr >> 8) & 0xff);
    buffer[5] = static_cast<uint8_t>((*copied_ptr >> 16) & 0xff);
    buffer[4] = static_cast<uint8_t>((*copied_ptr >> 24) & 0xff);
    buffer[3] = static_cast<uint8_t>((*copied_ptr >> 32) & 0xff);
    buffer[2] = static_cast<uint8_t>((*copied_ptr >> 40) & 0xff);
    buffer[1] = static_cast<uint8_t>((*copied_ptr >> 48) & 0xff);
    buffer[0] = static_cast<uint8_t>((*copied_ptr >> 56) & 0xff);
  }
#endif
  err = ByteOpResult::Ok;
}

inline void EncodeFloat(const float *value, uint8_t *buffer, size_t size,
                        ByteOpResult &err, bool is_big_endian) {
  if (!buffer || !value) {
    err = ByteOpResult::Nullptr;
    return;
  }

  if (size < 4) {
    err = ByteOpResult::SizeMismatch;
    return;
  }

  uint32_t int_value;

  // Create a suitably aligned buffer
  alignas(float)
      std::aligned_storage<sizeof(float), alignof(float)>::type align;
  std::memcpy(&align, value, sizeof(float));
  std::memcpy(&int_value, &align, sizeof(int_value));

  // std::memcpy(&int_value, value, sizeof(*value));

  EncodeUInt32(&int_value, buffer, size, err, is_big_endian);

  err = ByteOpResult::Ok;
}

inline void EncodeDouble(const double *value, uint8_t *buffer, size_t size,
                         ByteOpResult &err, bool is_big_endian) {
  if (!buffer || !value) {
    err = ByteOpResult::Nullptr;
    return;
  }

  if (size < 8) {
    err = ByteOpResult::SizeMismatch;
    return;
  }

  uint64_t long_value;

  // Create a suitably aligned buffer
  alignas(double)
      std::aligned_storage<sizeof(double), alignof(double)>::type align;
  std::memcpy(&align, value, sizeof(double));
  std::memcpy(&long_value, &align, sizeof(long_value));

  // std::memcpy(&long_value, value, sizeof(*value));

  EncodeUInt64(&long_value, buffer, size, err, is_big_endian);
}

inline void EncodeAsciiString(const std::string &value, uint8_t *buffer,
                              size_t size, ByteOpResult &err) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return;
  }

  if (size < value.length()) {
    err = ByteOpResult::SizeMismatch;
    return;
  }

  std::memcpy(buffer, value.c_str(), value.length());

  err = ByteOpResult::Ok;
}

inline int8_t DecodeInt8(const uint8_t *buffer, size_t size,
                         ByteOpResult &err) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return 0;
  }

  if (size < sizeof(int8_t)) {
    err = ByteOpResult::SizeMismatch;
    return 0;
  }

  int8_t value = static_cast<int8_t>(buffer[0]);

  err = ByteOpResult::Ok;
  return value;
}

inline int16_t DecodeInt16(const uint8_t *buffer, size_t size,
                           ByteOpResult &err, bool is_big_endian) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return 0;
  }

  if (size < sizeof(int16_t)) {
    err = ByteOpResult::SizeMismatch;
    return 0;
  }

  int16_t value;

#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN
  if (!is_big_endian) {
    value = (static_cast<int16_t>(buffer[0]) << 0) |
            (static_cast<int16_t>(buffer[1]) << 8);
  } else {
    value = (static_cast<int16_t>(buffer[1]) << 0) |
            (static_cast<int16_t>(buffer[0]) << 8);
  }
#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  if (is_big_endian) {
    value = (static_cast<int16_t>(buffer[0]) << 0) |
            (static_cast<int16_t>(buffer[1]) << 8);
  } else {
    value = (static_cast<int16_t>(buffer[1]) << 0) |
            (static_cast<int16_t>(buffer[0]) << 8);
  }
#endif

  err = ByteOpResult::Ok;
  return value;
}

inline int32_t DecodeInt32(const uint8_t *buffer, size_t size,
                           ByteOpResult &err, bool is_big_endian) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return 0;
  }

  if (size < sizeof(int32_t)) {
    err = ByteOpResult::SizeMismatch;
    return 0;
  }

  int32_t value;

#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN
  if (!is_big_endian) {
    value = (static_cast<int32_t>(buffer[0]) << 0) |
            (static_cast<int32_t>(buffer[1]) << 8) |
            (static_cast<int32_t>(buffer[2]) << 16) |
            (static_cast<int32_t>(buffer[3]) << 24);
  } else {
    value = (static_cast<int32_t>(buffer[3]) << 0) |
            (static_cast<int32_t>(buffer[2]) << 8) |
            (static_cast<int32_t>(buffer[1]) << 16) |
            (static_cast<int32_t>(buffer[0]) << 24);
  }

#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  if (is_big_endian) {
    value = (static_cast<int32_t>(buffer[0]) << 0) |
            (static_cast<int32_t>(buffer[1]) << 8) |
            (static_cast<int32_t>(buffer[2]) << 16) |
            (static_cast<int32_t>(buffer[3]) << 24);
  } else {
    value = (static_cast<int32_t>(buffer[3]) << 0) |
            (static_cast<int32_t>(buffer[2]) << 8) |
            (static_cast<int32_t>(buffer[1]) << 16) |
            (static_cast<int32_t>(buffer[0]) << 24);
  }
#endif

  err = ByteOpResult::Ok;
  return value;
}

inline int64_t DecodeInt64(const uint8_t *buffer, size_t size,
                           ByteOpResult &err, bool is_big_endian) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return 0;
  }

  if (size < sizeof(int64_t)) {
    err = ByteOpResult::SizeMismatch;
    return 0;
  }

  int64_t value;

#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN

  if (!is_big_endian) {
    value = (static_cast<int64_t>(buffer[0]) << 0) |
            (static_cast<int64_t>(buffer[1]) << 8) |
            (static_cast<int64_t>(buffer[2]) << 16) |
            (static_cast<int64_t>(buffer[3]) << 24) |
            (static_cast<int64_t>(buffer[4]) << 32) |
            (static_cast<int64_t>(buffer[5]) << 40) |
            (static_cast<int64_t>(buffer[6]) << 48) |
            (static_cast<int64_t>(buffer[7]) << 56);
  } else {
    value = (static_cast<int64_t>(buffer[7]) << 0) |
            (static_cast<int64_t>(buffer[6]) << 8) |
            (static_cast<int64_t>(buffer[5]) << 16) |
            (static_cast<int64_t>(buffer[4]) << 24) |
            (static_cast<int64_t>(buffer[3]) << 32) |
            (static_cast<int64_t>(buffer[2]) << 40) |
            (static_cast<int64_t>(buffer[1]) << 48) |
            (static_cast<int64_t>(buffer[0]) << 56);
  }

#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  if (is_big_endian) {
    value = (static_cast<int64_t>(buffer[0]) << 0) |
            (static_cast<int64_t>(buffer[1]) << 8) |
            (static_cast<int64_t>(buffer[2]) << 16) |
            (static_cast<int64_t>(buffer[3]) << 24) |
            (static_cast<int64_t>(buffer[4]) << 32) |
            (static_cast<int64_t>(buffer[5]) << 40) |
            (static_cast<int64_t>(buffer[6]) << 48) |
            (static_cast<int64_t>(buffer[7]) << 56);
  } else {
    value = (static_cast<int64_t>(buffer[7]) << 0) |
            (static_cast<int64_t>(buffer[6]) << 8) |
            (static_cast<int64_t>(buffer[5]) << 16) |
            (static_cast<int64_t>(buffer[4]) << 24) |
            (static_cast<int64_t>(buffer[3]) << 32) |
            (static_cast<int64_t>(buffer[2]) << 40) |
            (static_cast<int64_t>(buffer[1]) << 48) |
            (static_cast<int64_t>(buffer[0]) << 56);
  }
#endif

  err = ByteOpResult::Ok;
  return value;
}

inline uint8_t DecodeUInt8(const uint8_t *buffer, size_t size,
                           ByteOpResult &err) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return 0;
  }

  if (size < sizeof(uint8_t)) {
    err = ByteOpResult::SizeMismatch;
    return 0;
  }

  uint8_t value = buffer[0];

  err = ByteOpResult::Ok;
  return value;
}

inline uint16_t DecodeUInt16(const uint8_t *buffer, size_t size,
                             ByteOpResult &err, bool is_big_endian) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return 0;
  }

  if (size < sizeof(uint16_t)) {
    err = ByteOpResult::SizeMismatch;
    return 0;
  }

  uint16_t value;
#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN

  if (!is_big_endian) {
    value = (static_cast<uint16_t>(buffer[0]) << 0) |
            (static_cast<uint16_t>(buffer[1]) << 8);
  } else {
    value = (static_cast<uint16_t>(buffer[1]) << 0) |
            (static_cast<uint16_t>(buffer[0]) << 8);
  }
#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  if (is_big_endian) {
    value = (static_cast<uint16_t>(buffer[0]) << 0) |
            (static_cast<uint16_t>(buffer[1]) << 8);
  } else {
    value = (static_cast<uint16_t>(buffer[1]) << 0) |
            (static_cast<uint16_t>(buffer[0]) << 8);
  }
#endif
  err = ByteOpResult::Ok;
  return value;
}

inline uint32_t DecodeUInt32(const uint8_t *buffer, size_t size,
                             ByteOpResult &err, bool is_big_endian) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return 0;
  }

  if (size < sizeof(uint32_t)) {
    err = ByteOpResult::SizeMismatch;
    return 0;
  }

  uint32_t value;

#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN

  if (!is_big_endian) {
    value = (static_cast<uint32_t>(buffer[3]) << 24) |
            (static_cast<uint32_t>(buffer[2]) << 16) |
            (static_cast<uint32_t>(buffer[1]) << 8) |
            static_cast<uint32_t>(buffer[0]);
  } else {
    value = (static_cast<uint32_t>(buffer[0]) << 24) |
            (static_cast<uint32_t>(buffer[1]) << 16) |
            (static_cast<uint32_t>(buffer[2]) << 8) |
            static_cast<uint32_t>(buffer[3]);
  }

#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  if (is_big_endian) {
    value = (static_cast<uint32_t>(buffer[3]) << 24) |
            (static_cast<uint32_t>(buffer[2]) << 16) |
            (static_cast<uint32_t>(buffer[1]) << 8) |
            static_cast<uint32_t>(buffer[0]);
  } else {
    value = (static_cast<uint32_t>(buffer[0]) << 24) |
            (static_cast<uint32_t>(buffer[1]) << 16) |
            (static_cast<uint32_t>(buffer[2]) << 8) |
            static_cast<uint32_t>(buffer[3]);
  }
#endif

  err = ByteOpResult::Ok;
  return value;
}

inline uint64_t DecodeUInt64(const uint8_t *buffer, size_t size,
                             ByteOpResult &err, bool is_big_endian) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return 0;
  }

  if (size < sizeof(uint64_t)) {
    err = ByteOpResult::SizeMismatch;
    return 0;
  }

  uint64_t value;

#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN

  if (!is_big_endian) {
    value = (static_cast<uint64_t>(buffer[7]) << 56) |
            (static_cast<uint64_t>(buffer[6]) << 48) |
            (static_cast<uint64_t>(buffer[5]) << 40) |
            (static_cast<uint64_t>(buffer[4]) << 32) |
            (static_cast<uint64_t>(buffer[3]) << 24) |
            (static_cast<uint64_t>(buffer[2]) << 16) |
            (static_cast<uint64_t>(buffer[1]) << 8) |
            static_cast<uint64_t>(buffer[0]);
  } else {
    value = (static_cast<uint64_t>(buffer[0]) << 56) |
            (static_cast<uint64_t>(buffer[1]) << 48) |
            (static_cast<uint64_t>(buffer[2]) << 40) |
            (static_cast<uint64_t>(buffer[3]) << 32) |
            (static_cast<uint64_t>(buffer[4]) << 24) |
            (static_cast<uint64_t>(buffer[5]) << 16) |
            (static_cast<uint64_t>(buffer[6]) << 8) |
            static_cast<uint64_t>(buffer[7]);
  }

#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  if (is_big_endian) {
    value = (static_cast<uint64_t>(buffer[7]) << 56) |
            (static_cast<uint64_t>(buffer[6]) << 48) |
            (static_cast<uint64_t>(buffer[5]) << 40) |
            (static_cast<uint64_t>(buffer[4]) << 32) |
            (static_cast<uint64_t>(buffer[3]) << 24) |
            (static_cast<uint64_t>(buffer[2]) << 16) |
            (static_cast<uint64_t>(buffer[1]) << 8) |
            static_cast<uint64_t>(buffer[0]);
  } else {
    value = (static_cast<uint64_t>(buffer[0]) << 56) |
            (static_cast<uint64_t>(buffer[1]) << 48) |
            (static_cast<uint64_t>(buffer[2]) << 40) |
            (static_cast<uint64_t>(buffer[3]) << 32) |
            (static_cast<uint64_t>(buffer[4]) << 24) |
            (static_cast<uint64_t>(buffer[5]) << 16) |
            (static_cast<uint64_t>(buffer[6]) << 8) |
            static_cast<uint64_t>(buffer[7]);
  }
#endif
  err = ByteOpResult::Ok;
  return value;
}

inline float DecodeFloat(const uint8_t *buffer, size_t size, ByteOpResult &err,
                         bool is_big_endian) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return 0.0f;
  }

  if (size < sizeof(float)) {
    err = ByteOpResult::SizeMismatch;
    return 0.0f;
  }

  float result;

  uint32_t temp = DecodeUInt32(buffer, 4, err, is_big_endian);
  std::memcpy(&result, &temp, sizeof(float));

  return result;
}

inline double DecodeDouble(const uint8_t *buffer, size_t size,
                           ByteOpResult &err, bool is_big_endian) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return 0.0;
  }

  if (size < sizeof(double)) {
    err = ByteOpResult::SizeMismatch;
    return 0.0f;
  }

  double result;

  uint64_t temp = DecodeUInt64(buffer, 8, err, is_big_endian);
  std::memcpy(&result, &temp, sizeof(double));

  return result;
}

inline std::string DecodeAsciiString(const uint8_t *buffer, size_t size,
                                     ByteOpResult &err) {
  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return std::string();
  }
  return std::string(reinterpret_cast<const char *>(buffer), size);
}

}  // namespace u8
}  // namespace details
}  // namespace bytes
}  // namespace nvm

#endif  // NVM_CORE_BYTES_DETAILS_V2_INTERNAL_BYTE_H
