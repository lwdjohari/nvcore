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

#ifndef NVM_CORE_BYTES_DETAILS_V2_INTERNAL_BYTE_CH_H
#define NVM_CORE_BYTES_DETAILS_V2_INTERNAL_BYTE_CH_H

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
namespace ch {

void CopyBytes(const char* src, char* dest, size_t size,
               ByteOpResult& err) noexcept;

void EncodeInt8(const int8_t& value, char* buffer, size_t size,
                ByteOpResult& err);

void EncodeUInt8(const uint8_t& value, char* buffer, size_t size,
                 ByteOpResult& err);

void EncodeInt16(const int16_t* value, char* buffer, size_t size,
                 ByteOpResult& err, bool is_big_endian);

void EncodeUInt16(const uint16_t* value, char* buffer, size_t size,
                  ByteOpResult& err, bool is_big_endian);

void EncodeInt32(const int32_t* value, char* buffer, size_t size,
                 ByteOpResult& err, bool is_big_endian);

void EncodeUInt32(const uint32_t* value, char* buffer, size_t size,
                  ByteOpResult& err, bool is_big_endian);

void EncodeInt64(const int64_t* value, char* buffer, size_t size,
                 ByteOpResult& err, bool is_big_endian);

void EncodeUInt64(const uint64_t* value, char* buffer, size_t size,
                  ByteOpResult& err, bool is_big_endian);

void EncodeAsciiString(const std::string& value, char* buffer,
                              size_t size, ByteOpResult& err);

void EncodeFloat(const float* value, char* buffer, size_t size,
                 ByteOpResult& err, bool is_big_endian);

void EncodeDouble(const double* value, char* buffer, size_t size,
                  ByteOpResult& err, bool is_big_endian);

int8_t DecodeInt8(const char* buffer, size_t size, ByteOpResult& err);

int16_t DecodeInt16(const char* buffer, size_t size, ByteOpResult& err,
                    bool is_big_endian);

int32_t DecodeInt32(const char* buffer, size_t size, ByteOpResult& err,
                    bool is_big_endian);

int64_t DecodeInt64(const char* buffer, size_t size, ByteOpResult& err,
                    bool is_big_endian);

uint8_t DecodeUInt8(const char* buffer, size_t size, ByteOpResult& err);

uint16_t DecodeUInt16(const char* buffer, size_t size, ByteOpResult& err,
                      bool is_big_endian);

uint32_t DecodeUInt32(const char* buffer, size_t size, ByteOpResult& err,
                      bool is_big_endian);

uint64_t DecodeUInt64(const char* buffer, size_t size, ByteOpResult& err,
                      bool is_big_endian);

float DecodeFloat(const char* buffer, size_t size, ByteOpResult& err,
                  bool is_big_endian);

double DecodeDouble(const char* buffer, size_t size, ByteOpResult& err,
                    bool is_big_endian);
                    
std::string DecodeAsciiString(const char* buffer, size_t size,
                              ByteOpResult& err);

}  // namespace ch
}  // namespace details
}  // namespace bytes
}  // namespace nvm

#endif  // NVM_CORE_BYTES_DETAILS_V2_INTERNAL_BYTE_H
