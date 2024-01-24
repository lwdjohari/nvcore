#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>
#include <memory>

#include "catch2/catch_all.hpp"
#include "nvm/bytes/byte_declaration.h"
#include "nvm/bytes/byte.h"

using namespace nvm;

/** float */
TEST_CASE("float max from network", "[byte][be-le][float-max]") {
  auto byte = new uint8_t[4];
  byte[0] = 0x7F;
  byte[1] = 0x7F;
  byte[2] = 0xFF;
  byte[3] = 0xFF;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToFloat(byte, 4, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<float>::max());

  delete[] byte;
}

TEST_CASE("float max ", "[byte][le-le][float-max]") {
  auto byte = new uint8_t[4];
  byte[0] = 0xFF;
  byte[1] = 0xFF;
  byte[2] = 0x7F;
  byte[3] = 0x7F;

  bytes::ByteOpResult err;
  
  auto res = bytes::ToFloat(byte, 4, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<float>::max());

  delete[] byte;
}

TEST_CASE("float min from network", "[byte][be-le][float-min]") {
  auto byte = new uint8_t[4];

  byte[0] = 255;
  byte[1] = 127;
  byte[2] = 255;
  byte[3] = 255;

  bytes::ByteOpResult err;
  
  auto res = bytes::ToFloat(byte, 4, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<float>::lowest());

  delete[] byte;
}

TEST_CASE("float min ", "[byte][le-le][float-min]") {
  auto byte = new uint8_t[4];
  byte[0] = 255;
  byte[1] = 255;
  byte[2] = 127;
  byte[3] = 255;

  bytes::ByteOpResult err;
  
  auto res = bytes::ToFloat(byte, 4, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<float>::lowest());

  delete[] byte;
}

/** double */
TEST_CASE("double max from network", "[byte][be-le][double-max]") {
  auto byte = new uint8_t[8];
  byte[0] = 127;
  byte[1] = 239;
  byte[2] = 255;
  byte[3] = 255;
  byte[4] = 255;
  byte[5] = 255;
  byte[6] = 255;
  byte[7] = 255;

  bytes::ByteOpResult err;
  
  auto res = bytes::ToDouble(byte, 8, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<double>::max());

  delete[] byte;
}

TEST_CASE("double max ", "[byte][le-le][double-max]") {
  auto byte = new uint8_t[8];
  byte[0] = 255;
  byte[1] = 255;
  byte[2] = 255;
  byte[3] = 255;
  byte[4] = 255;
  byte[5] = 255;
  byte[6] = 239;
  byte[7] = 127;

  bytes::ByteOpResult err;
  
  auto res = bytes::ToDouble(byte, 8, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<double>::max());

  delete[] byte;
}

TEST_CASE("double min from network", "[byte][be-le][double-min]") {
  auto byte = new uint8_t[8];

  byte[0] = 255;
  byte[1] = 239;
  byte[2] = 255;
  byte[3] = 255;
  byte[4] = 255;
  byte[5] = 255;
  byte[6] = 255;
  byte[7] = 255;

  bytes::ByteOpResult err;
  
  auto res = bytes::ToDouble(byte, 8, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<double>::lowest());

  delete[] byte;
}

TEST_CASE("double min ", "[byte][le-le][double-min]") {
  auto byte = new uint8_t[8];
  byte[0] = 255;
  byte[1] = 255;
  byte[2] = 255;
  byte[3] = 255;
  byte[4] = 255;
  byte[5] = 255;
  byte[6] = 239;
  byte[7] = 255;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToDouble(byte, 8, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<double>::lowest());

  delete[] byte;
}

/** uint64 */
TEST_CASE("uint64 max from network", "[byte][be-le][uint64-max]") {
  auto byte = new uint8_t[8];
  byte[0] = 255;
  byte[1] = 255;
  byte[2] = 255;
  byte[3] = 255;
  byte[4] = 255;
  byte[5] = 255;
  byte[6] = 255;
  byte[7] = 255;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToUint64(byte, 8, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<uint64_t>::max());

  delete[] byte;
}

TEST_CASE("uint64 max ", "[byte][le-le][uint64-max]") {
  auto byte = new uint8_t[8];
  byte[0] = 255;
  byte[1] = 255;
  byte[2] = 255;
  byte[3] = 255;
  byte[4] = 255;
  byte[5] = 255;
  byte[6] = 255;
  byte[7] = 255;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToUint64(byte, 8, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<uint64_t>::max());

  delete[] byte;
}

TEST_CASE("uint64 min from network", "[byte][be-le][uint64-min]") {
  auto byte = new uint8_t[8];
  byte[0] = 0;
  byte[1] = 0;
  byte[2] = 0;
  byte[3] = 0;
  byte[4] = 0;
  byte[5] = 0;
  byte[6] = 0;
  byte[7] = 0;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToUint64(byte, 8, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == 0);

  delete[] byte;
}

TEST_CASE("uint64 min ", "[byte][le-le][uint64-min]") {
  auto byte = new uint8_t[8];
  byte[0] = 0;
  byte[1] = 0;
  byte[2] = 0;
  byte[3] = 0;
  byte[4] = 0;
  byte[5] = 0;
  byte[6] = 0;
  byte[7] = 0;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToUint64(byte, 8, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == 0);

  delete[] byte;
}

/** int64 */
TEST_CASE("int64 max from network", "[byte][be-le][int64-max]") {
  auto byte = new uint8_t[8];
  byte[0] = 127;
  byte[1] = 255;
  byte[2] = 255;
  byte[3] = 255;
  byte[4] = 255;
  byte[5] = 255;
  byte[6] = 255;
  byte[7] = 255;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToInt64(byte, 8, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<int64_t>::max());

  delete[] byte;
}

TEST_CASE("int64 max ", "[byte][le-le][int64-max]") {
  auto byte = new uint8_t[8];
  byte[0] = 255;
  byte[1] = 255;
  byte[2] = 255;
  byte[3] = 255;
  byte[4] = 255;
  byte[5] = 255;
  byte[6] = 255;
  byte[7] = 127;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToInt64(byte, 8, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<int64_t>::max());

  delete[] byte;
}

TEST_CASE("int64 min from network", "[byte][be-le][int64-min]") {
  auto byte = new uint8_t[8];
  byte[0] = 128;
  byte[1] = 0;
  byte[2] = 0;
  byte[3] = 0;
  byte[4] = 0;
  byte[5] = 0;
  byte[6] = 0;
  byte[7] = 0;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToInt64(byte, 8, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<int64_t>::min());

  delete[] byte;
}

TEST_CASE("int64 min ", "[byte][le-le][int64-min]") {
  auto byte = new uint8_t[8];
  byte[0] = 0;
  byte[1] = 0;
  byte[2] = 0;
  byte[3] = 0;
  byte[4] = 0;
  byte[5] = 0;
  byte[6] = 0;
  byte[7] = 128;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToInt64(byte, 8, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == std::numeric_limits<int64_t>::min());

  delete[] byte;
}

/** uint32 */
TEST_CASE("uint32 max from network", "[byte][be-le][uint32-max]") {
  auto byte = new uint8_t[4];
  byte[0] = 255;
  byte[1] = 255;
  byte[2] = 255;
  byte[3] = 255;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToUint32(byte, 4, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == 4294967295);

  delete[] byte;
}

TEST_CASE("uint32 max ", "[byte][le-le][uint32-max]") {
  auto byte = new uint8_t[4];
  byte[0] = 255;
  byte[1] = 255;
  byte[2] = 255;
  byte[3] = 255;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToUint32(byte, 4, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == 4294967295);

  delete[] byte;
}

TEST_CASE("uint32 min from network", "[byte][be-le][uint32-min]") {
  auto byte = new uint8_t[4];
  byte[0] = 0;
  byte[1] = 0;
  byte[2] = 0;
  byte[3] = 0;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToUint32(byte, 4, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == 0);

  delete[] byte;
}

TEST_CASE("uint32 min ", "[byte][le-le][uint32-min]") {
  auto byte = new uint8_t[4];
  byte[0] = 0;
  byte[1] = 0;
  byte[2] = 0;
  byte[3] = 0;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToUint32(byte, 4, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == 0);

  delete[] byte;
}

/** int32 */

TEST_CASE("int32 max from network", "[byte][be-le][int32-max]") {
  auto byte = new uint8_t[4];
  byte[0] = 127;
  byte[1] = 255;
  byte[2] = 255;
  byte[3] = 255;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToInt32(byte, 4, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == 2147483647);

  delete[] byte;
}

TEST_CASE("int32 max ", "[byte][le-le][int32-max]") {
  auto byte = new uint8_t[4];
  byte[0] = 255;
  byte[1] = 255;
  byte[2] = 255;
  byte[3] = 127;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToInt32(byte, 4, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == 2147483647);

  delete[] byte;
}

TEST_CASE("int32 min from network", "[byte][be-le][int32-min]") {
  auto byte = new uint8_t[4];
  byte[0] = 128;
  byte[1] = 0;
  byte[2] = 0;
  byte[3] = 0;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToInt32(byte, 4, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == -2147483648);

  delete[] byte;
}

TEST_CASE("int32 min ", "[byte][le-le][int32-min]") {
  auto byte = new uint8_t[4];
  byte[0] = 0;
  byte[1] = 0;
  byte[2] = 0;
  byte[3] = 128;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToInt32(byte, 4, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == -2147483648);

  delete[] byte;
}

/** uint16 */

TEST_CASE("uint16 max from network", "[byte][be-le][uint16-max]") {
  auto byte = new uint8_t[2];
  byte[0] = 255;
  byte[1] = 255;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToUint16(byte, 2, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == 65535);

  delete[] byte;
}

TEST_CASE("uint16 max ", "[byte][le-le][uint16-max]") {
  auto byte = new uint8_t[2];
  byte[0] = 255;
  byte[1] = 255;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToUint16(byte, 2, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == 65535);

  delete[] byte;
}

TEST_CASE("uint16 min from network", "[byte][be-le][uint16-min]") {
  auto byte = new uint8_t[2];
  byte[0] = 0;
  byte[1] = 0;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToUint16(byte, 2, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == 0);

  delete[] byte;
}

TEST_CASE("uint16 min ", "[byte][le-le][uint16-min]") {
  auto byte = new uint8_t[2];
  byte[0] = 0;
  byte[1] = 0;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToUint16(byte, 2, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == 0);

  delete[] byte;
}

/** int16 */
TEST_CASE("int16 max from network", "[byte][be-le][int16-max]") {
  auto byte = new uint8_t[2];
  byte[0] = 127;
  byte[1] = 255;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToInt16(byte, 2, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == 32767);

  delete[] byte;
}

TEST_CASE("int16 max ", "[byte][le-le][int16-max]") {
  auto byte = new uint8_t[2];
  byte[0] = 255;
  byte[1] = 127;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToInt16(byte, 2, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == 32767);

  delete[] byte;
}

TEST_CASE("int16 min from network", "[byte][be-le][int16-min]") {
  auto byte = new uint8_t[2];
  byte[0] = 128;
  byte[1] = 0;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToInt16(byte, 2, err, bytes::EndianessType::BigEndian);
  CAPTURE(res);

  REQUIRE(res == -32768);

  delete[] byte;
}

TEST_CASE("int16 min ", "[byte][le-le][int16-min]") {
  auto byte = new uint8_t[2];
  byte[0] = 0;
  byte[1] = 128;

  bytes::ByteOpResult err;

  
  auto res = bytes::ToInt16(byte, 2, err, bytes::EndianessType::LittleEndian);
  CAPTURE(res);

  REQUIRE(res == -32768);

  delete[] byte;
}
