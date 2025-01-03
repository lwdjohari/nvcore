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
#include <memory>
#include <vector>

#include "nvm/bytes/byte_declaration.h"
#include "nvm/strings/utf8string.h"
#include "nvm/bytes/details/internal_byte_ch.h"
#include "nvm/bytes/details/internal_byte_u8.h"
namespace nvm {
namespace bytes {


/// @brief
/// @return
inline EndianessType HostEndianess() {
#if NVM_HOST_ENDIAN == ENDIANESS_LITTLE_ENDIAN
  return EndianessType::LittleEndian;
#elif NVM_HOST_ENDIAN == ENDIANESS_BIG_ENDIAN
  return EndianessType::BigEndian;
#else
  return EndianessType::Mixed;
#endif
}

/// @brief
/// @tparam T char or uint8_t
/// @param src
/// @param dest
/// @param size
/// @return
template <typename T>
ByteOpResult CopyBytes(const T* src, const T* dest,
                       const size_t& size) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  auto result = ByteOpResult::None;
  if constexpr (std::is_same<T, uint8_t>::value) {
    details::u8::CopyBytes(src, dest, size, result);
    return result;
  } else {
    details::ch::CopyBytes(src, dest, size, result);
    return result;
  }
};

/// @brief Convert known-types to raw bytes sequence in big-endian or
/// little-endian format.
/// @tparam TVal type of int8_t to int64_t, uint8_t to uint64_t,
/// double, float, std::string, Utf8Str or bool
/// @tparam TSeq char or uint8_t
/// @param val
/// @param dest
/// @param dest_size
/// @param target
/// @return
template <typename TVal, typename TSeq>
ByteOpResult ToBytes(
    const TVal& val, TSeq* dest, const size_t& dest_size,
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
      details::ch::EncodeAsciiString(val, dest, dest_size, result);
      return result;
    }
  } else if constexpr (std::is_same<TVal, strings::Utf8String>::value) {
    // execute method for Utf8Str
    return ByteOpResult::None;
  } else if constexpr (std::is_same<TVal, bool>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      // execute method for bool

      details::u8::EncodeUInt8(val ? 1 : 0, dest, dest_size, result);
      return result;
    } else {
      details::ch::EncodeUInt8(val ? 1 : 0, dest, dest_size, result);
      return result;
    }

  } else if constexpr (std::is_same<TVal, float>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeFloat(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      details::ch::EncodeFloat(&val, dest, dest_size, result, is_big_endian);
      return result;
    }
  } else if constexpr (std::is_same<TVal, double>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeDouble(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      details::ch::EncodeDouble(&val, dest, dest_size, result, is_big_endian);
      return result;
    }
  } else if constexpr (std::is_same<TVal, int8_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeInt8(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      details::ch::EncodeInt8(&val, dest, dest_size, result, is_big_endian);
      return result;
    }
  } else if constexpr (std::is_same<TVal, int16_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeInt16(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      details::ch::EncodeInt16(&val, dest, dest_size, result, is_big_endian);
      return result;
    }

  } else if constexpr (std::is_same<TVal, int32_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeInt32(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      details::ch::EncodeInt32(&val, dest, dest_size, result, is_big_endian);
      return result;
    }
  } else if constexpr (std::is_same<TVal, int64_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeInt64(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      details::ch::EncodeInt64(&val, dest, dest_size, result, is_big_endian);
      return result;
    }
  } else if constexpr (std::is_same<TVal, uint8_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeUInt8(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      details::ch::EncodeUInt8(&val, dest, dest_size, result, is_big_endian);
      return result;
    }
  } else if constexpr (std::is_same<TVal, uint16_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeUInt16(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      details::ch::EncodeUInt16(&val, dest, dest_size, result, is_big_endian);
      return result;
    }
  } else if constexpr (std::is_same<TVal, uint32_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeUInt32(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      details::ch::EncodeUInt32(&val, dest, dest_size, result, is_big_endian);
      return result;
    }
  } else if constexpr (std::is_same<TVal, uint64_t>::value) {
    ByteOpResult result = ByteOpResult::None;
    if constexpr (std::is_same<TSeq, uint8_t>::value) {
      details::u8::EncodeUInt64(&val, dest, dest_size, result, is_big_endian);
      return result;
    } else {
      details::ch::EncodeUInt64(&val, dest, dest_size, result, is_big_endian);
      return result;
    }
  }

  return ByteOpResult::None;
};

/// @brief
/// @tparam T char or uint8_t
/// @param bytes
/// @param size
/// @param result
/// @return
template <typename T>
uint8_t ToUint8(const T* bytes, const size_t& size,
                ByteOpResult& result) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeUInt8(bytes, size, result);
  } else {
    return details::ch::DecodeUInt8(bytes, size, result);
  }
};

template <typename T>
uint16_t ToUint16(const T* bytes, const size_t& size, ByteOpResult& result,
                  EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeUInt16(bytes, size, result,
                                     target == EndianessType::BigEndian);
  } else {
    return details::ch::DecodeUInt16(bytes, size, result,
                                     target == EndianessType::BigEndian);
  }
};

/// @brief
/// @tparam T char or uint8_t
/// @param bytes
/// @param size
/// @param result
/// @param target
/// @return
template <typename T>
uint32_t ToUint32(const T* bytes, const size_t& size, ByteOpResult& result,
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

/// @brief
/// @tparam T char or uint8_t
/// @param bytes
/// @param size
/// @param result
/// @param target
/// @return
template <typename T>
uint64_t ToUint64(const T* bytes, const size_t& size, ByteOpResult& result,
                  EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeUInt64(bytes, size, result,
                                     target == EndianessType::BigEndian);
  } else {
    return details::ch::DecodeUInt64(bytes, size, result,
                                     target == EndianessType::BigEndian);
  }
};

/// @brief
/// @tparam T char or uint8_t
/// @param bytes
/// @param size
/// @param result
/// @return
template <typename T>
int8_t ToInt8(const T* bytes, const size_t& size,
              ByteOpResult& result) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeInt8(bytes, size, result);
  } else {
    return details::ch::DecodeInt8(bytes, size, result);
  }
};

/// @brief
/// @tparam T char or uint8_t
/// @param bytes
/// @param size
/// @param result
/// @param target
/// @return
template <typename T>
int16_t ToInt16(const T* bytes, const size_t& size, ByteOpResult& result,
                EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeInt16(bytes, size, result,
                                    target == EndianessType::BigEndian);
  } else {
    return details::ch::DecodeInt16(bytes, size, result,
                                    target == EndianessType::BigEndian);
  }
};

/// @brief
/// @tparam T char or uint8_t
/// @param bytes
/// @param size
/// @param result
/// @param target
/// @return
template <typename T>
int32_t ToInt32(const T* bytes, const size_t& size, ByteOpResult& result,
                EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeInt32(bytes, size, result,
                                    target == EndianessType::BigEndian);
  } else {
    return details::ch::DecodeInt32(bytes, size, result,
                                    target == EndianessType::BigEndian);
  }
};

/// @brief
/// @tparam T char or uint8_t
/// @param bytes
/// @param size
/// @param result
/// @param target
/// @return
template <typename T>
int64_t ToInt64(const T* bytes, const size_t& size, ByteOpResult& result,
                EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeInt64(bytes, size, result,
                                    target == EndianessType::BigEndian);
  } else {
    return details::ch::DecodeInt64(bytes, size, result,
                                    target == EndianessType::BigEndian);
  }
};

/// @brief
/// @tparam T char or uint8_t
/// @param bytes
/// @param size
/// @param result
/// @param target
/// @return
template <typename T>
float ToFloat(const T* bytes, const size_t& size, ByteOpResult& result,
              EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeFloat(bytes, size, result,
                                    target == EndianessType::BigEndian);
  } else {
    return details::ch::DecodeFloat(bytes, size, result,
                                    target == EndianessType::BigEndian);
  }
};

/// @brief
/// @tparam T char or uint8_t
/// @param bytes
/// @param size
/// @param result
/// @param target
/// @return
template <typename T>
double ToDouble(const T* bytes, const size_t& size, ByteOpResult& result,
                EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");
  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeDouble(bytes, size, result,
                                     target == EndianessType::BigEndian);
  } else {
    return details::ch::DecodeDouble(bytes, size, result,
                                     target == EndianessType::BigEndian);
  }
};

/// @brief
/// @tparam T char or uint8_t
/// @param bytes
/// @param size
/// @param result
/// @param target
/// @return
template <typename T>
std::string ToAsciiString(
    const T* bytes, const size_t& size, ByteOpResult& result,
    EndianessType target = EndianessType::LittleEndian) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");
  if constexpr (std::is_same<T, uint8_t>::value) {
    return details::u8::DecodeAsciiString(bytes, size, result);
  } else {
    return details::ch::DecodeAsciiString(bytes, size, result);
  }
};

/// @brief
/// @tparam T char or uint8_t
/// @param bytes
/// @param size
/// @return
template <typename T>
strings::Utf8String ToUtf8String(const T* bytes, const size_t& size) noexcept {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  return std::move(strings::Utf8String::MakeUtf8StringUnchecked(bytes, size));
};

/// @brief
/// @tparam T char or uint8_t
/// @param buffer
/// @param length
/// @param err
/// @return
template <typename T>
uint16_t Crc16IBM(const T* buffer, size_t length, ByteOpResult& err) {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return 0;
  }

  uint16_t crc = 0xFFFF;

  for (size_t i = 0; i < length; i++) {
    crc ^= static_cast<uint16_t>(buffer[i]) << 8;

    for (int j = 0; j < 8; j++) {
      if (crc & 0x8000) {
        crc = (crc << 1) ^ 0x8005;
      } else {
        crc <<= 1;
      }
    }
  }

  err = ByteOpResult::Ok;
  return crc;
}

/// @brief
/// @tparam T char or uint8_t
/// @param buffer
/// @param size
/// @param err
/// @param target
/// @return
template <typename T>
uint16_t Crc16CCITT(const T* buffer, size_t size, ByteOpResult& err,
                    EndianessType target) {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  if (!buffer) {
    err = ByteOpResult::Nullptr;
    return 0;
  }

  uint16_t crc = 0xFFFF;
  uint16_t polynomial = 0x1021;
  if (target == EndianessType::BigEndian) {
    for (size_t i = 0; i < size; ++i) {
      crc ^= static_cast<uint16_t>(buffer[i]) << 8;
      for (int j = 0; j < 8; ++j) {
        if (crc & 0x8000) {
          crc = (crc << 1) ^ polynomial;
        } else {
          crc <<= 1;
        }
      }
    }
  } else {
    for (size_t i = 0; i < size; ++i) {
      crc ^= buffer[i];
      for (int j = 0; j < 8; ++j) {
        if (crc & 0x0001) {
          crc = (crc >> 1) ^ polynomial;
        } else {
          crc >>= 1;
        }
      }
    }
  }
  err = ByteOpResult::Ok;
  return crc;
}

/// @brief
/// @tparam T char or uint8_t
/// @param hex_str
/// @return
template <typename T>
std::shared_ptr<std::vector<T>> ToBytesFromHexString(
    const std::string& hex_str) {
  static_assert(std::is_same<T, char>::value || std::is_same<T, uint8_t>::value,
                "T can only be char or uint8_t");

  auto v = std::make_shared<std::vector<T>>();

  // Iterate through the hex stream, two characters at a time
  for (int i = 0; i < hex_str.length(); i += 2) {
    // Parse the two hexadecimal digits and convert to uint8_t
    std::string hex_pair = hex_str.substr(i, 2);
    T decimal_value = static_cast<T>(std::stoi(hex_pair, 0, 16));

    v->emplace_back(decimal_value);
  }

  // cppcheck-suppress returnStdMoveLocal
  return std::move(v);
}

}  // namespace bytes
}  // namespace nvm

#endif  // NVM_CORE_BYTES_V2_BYTE_H
