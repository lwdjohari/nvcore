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

#ifndef NVM_CORE_BYTES_V2_BYTE_H
#define NVM_CORE_BYTES_V2_BYTE_H

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "nvm/bytes/byte_declaration.h"
#include "nvm/bytes/details/internal_byte_u8.h"
#include "nvm/bytes/details/internal_byte_ch.h"
#include "nvm/strings/utf8string.h"

namespace nvm {
namespace bytes {
using namespace strings;

EndianessType HostEndianess() {
#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN
  return EndianessType::LittleEndian;
#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  return EndianessType::BigEndian;
#else
  return EndianessType::Mixed;
#endif
}

template <typename T>
ByteOpResult CopyBytes(const T *src, const T *dest,
                       const size_t &size) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  ByteOpResult result = ByteOpResult::None;
  if constexpr (std::is_same<T, uint8_t>::value) {
    details::u8::CopyBytes(src, dest, size);
    return result;
  } else {
    return ByteOpResult::None;
  }
};

template <typename TVal, typename TSeq>
ByteOpResult ToBytes(
    const TVal &val, const TSeq *dest, const size_t &dest_size,
    EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(
      std::is_same<TSeq, char>::value || std::is_same<TSeq, uint8_t>::value,
      "T can only be char or uint8_t");

  static_assert(
      std::is_same<TSeq, char>::value || std::is_same<TSeq, uint8_t>::value,
      "TVal can only be int8_t to int64_t, uint8_t to uint64_t, "
      "double, float, std::string, Utf8Str and bool");

  bool is_big_endian = target == EndianessType::BigEndian;

  if constexpr (std::is_same<TVal, std::string>::value) {
    // execute method for std::string
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeAsciiString(val, dest, dest_size, result);
      return result;
    } else {
      return result;
    }
  } else if constexpr (std::is_same<TVal, Utf8String>::value) {
    // execute method for Utf8Str
  } else if constexpr (std::is_same<TVal, bool>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      // execute method for bool

      details::u8::EncodeUInt8(val ? 1 : 0, dest, dest_size, result);
      return result;
    } else {
      return result;
    }

  } else if constexpr (std::is_same<TVal, float>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeFloat(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      return result;
    }
  } else if constexpr (std::is_same<TVal, double>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeDouble(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      return result;
    }
  } else if constexpr (std::is_same<TVal, int8_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeInt8(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      return result;
    }
  } else if constexpr (std::is_same<TVal, int16_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeInt16(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
    }
  } else if constexpr (std::is_same<TVal, int32_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeInt32(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      return result;
    }
  } else if constexpr (std::is_same<TVal, int64_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeInt64(&val, dest, dest_size, result, is_big_endian);
      ;
      return result;
    } else {
      return result;
    }
  } else if constexpr (std::is_same<TVal, uint8_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeUInt8(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      return result;
    }
  } else if constexpr (std::is_same<TVal, uint16_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeUInt16(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      return result;
    }
  } else if constexpr (std::is_same<TVal, uint32_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeUInt32(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      return result;
    }
  } else if constexpr (std::is_same<TVal, uint64_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeUInt64(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      return result;
    }
  }

  return ByteOpResult::None;
};

template <typename T>
uint8_t ToUint8(const T *bytes, const size_t &size,
                ByteOpResult &result) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeUInt8(bytes, size, result);
  } else {
    return 0;
  }
};

template <typename T>
uint16_t ToUint16(const T *bytes, const size_t &size, ByteOpResult &result,
                  EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeUInt16(bytes, size, result,
                                     target == EndianessType::BigEndian);
  } else {
    return 0;
  }
};

template <typename T>
uint32_t ToUint32(const T *bytes, const size_t &size, ByteOpResult &result,
                  EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeUInt32(bytes, size, result,
                                     target == EndianessType::BigEndian);
  } else {
    return details::ch::DecodeUInt32(bytes, size, result,
                                     target == EndianessType::BigEndian);
  }
};

template <typename T>
uint64_t ToUint64(const T *bytes, const size_t &size, ByteOpResult &result,
                  EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeUInt64(bytes, size, result,
                                     target == EndianessType::BigEndian);
  } else {
    return 0;
  }
};

template <typename T>
int8_t ToInt8(const T *bytes, const size_t &size,
              ByteOpResult &result) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeInt8(bytes, size, result);
  } else {
    return 0;
  }
};

template <typename T>
int16_t ToInt16(const T *bytes, const size_t &size, ByteOpResult &result,
                EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeInt16(bytes, size, result),
           target == EndianessType::BigEndian;
  } else {
    return 0;
  }
};

template <typename T>
int32_t ToInt32(const T *bytes, const size_t &size, ByteOpResult &result,
                EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeInt32(bytes, size, result,
                                    target == EndianessType::BigEndian);
  } else {
    return 0;
  }
};

template <typename T>
int64_t ToInt64(const T *bytes, const size_t &size, ByteOpResult &result,
                EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeInt64(bytes, size, result,
                                    target == EndianessType::BigEndian);
  } else {
    return 0;
  }
};

template <typename T>
float ToFloat(const T *bytes, const size_t &size, ByteOpResult &result,
              EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeFloat(bytes, size, result,
                                    target == EndianessType::BigEndian);
  } else {
    return 0;
  }
};

template <typename T>
double ToDouble(const T *bytes, const size_t &size, ByteOpResult &result,
                EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");
  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeDouble(bytes, size, result,
                                     target == EndianessType::BigEndian);
  } else {
    return 0;
  }
};

template <typename T>
std::string ToAsciiString(
    const T *bytes, const size_t &size, ByteOpResult &result,
    EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");
  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeAsciiString(bytes, size, result);
  } else {
    return 0;
  }
};

template <typename T>
Utf8String ToUtf8String(const T *bytes, const size_t &size) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  return std::move(Utf8String::MakeUtf8StringUnchecked(bytes, size));
};

}  // namespace bytes
}  // namespace nvm

#endif  // NVM_CORE_BYTES_V2_BYTE_H
