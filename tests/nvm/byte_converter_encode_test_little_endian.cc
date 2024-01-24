#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>

#include "catch2/catch_all.hpp"
#include "nvm/nvm_core.h"

using namespace nvm;

/** Little-endian byte order */

/** float */
TEST_CASE("Enc | float | max", "[tobyte][le-le][float-max]") {
  bytes::ByteOpResult err;
  
  size_t size = 4;
  auto src = std::numeric_limits<float>::max();
  auto answer = new uint8_t[size]{0xFF, 0xFF, 0x7F, 0x7F};
  auto destp = new uint8_t[size];

  BENCHMARK_ADVANCED("Enc | float | pointer - max")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([ src, destp, size, &err] {
      err = bytes::ToBytes(src, destp, size, bytes::EndianessType::LittleEndian);
      return err;
    });
  };

  REQUIRE(vectorex::CompareElements(destp, answer, size) == true);

  delete[] answer;
  delete[] destp;
}

/** double */
TEST_CASE("Enc | double | max", "[tobyte][le-le][double-max]") {
  bytes::ByteOpResult err;
  
  size_t size = 8;
  auto src = std::numeric_limits<double>::max();
  auto answer = new uint8_t[size]{255, 255, 255, 255, 255, 255, 239, 127};
  auto destp = new uint8_t[size];

  BENCHMARK_ADVANCED("Enc | double | pointer - max")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([ src, destp, size, &err] {
      err = bytes::ToBytes(src, destp, size, bytes::EndianessType::LittleEndian);
      return err;
    });
  };

  REQUIRE(vectorex::CompareElements(destp, answer, size) == true);

  delete[] answer;
  delete[] destp;
}

/** uint64_t */
TEST_CASE("Enc | uint64_t | max", "[tobyte][le-le][uint64_t-max]") {
  bytes::ByteOpResult err;
  
  size_t size = 8;
  auto src = std::numeric_limits<uint64_t>::max();
  auto answer = new uint8_t[size]{255, 255, 255, 255, 255, 255, 255, 255};
  auto destp = new uint8_t[size];

  BENCHMARK_ADVANCED("Enc | uint64_t | pointer - max")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([ src, destp, size, &err] {
      err = bytes::ToBytes(src, destp, size, bytes::EndianessType::LittleEndian);
      return err;
    });
  };

  REQUIRE(vectorex::CompareElements(destp, answer, size) == true);

  delete[] answer;
  delete[] destp;
}

/** uint32_t */
TEST_CASE("Enc | uint32_t | max", "[tobyte][le-le][uint32_t-max]") {
  bytes::ByteOpResult err;
  
  size_t size = 4;
  auto src = std::numeric_limits<uint32_t>::max();
  auto answer = new uint8_t[size]{255, 255, 255, 255};
  auto destp = new uint8_t[size];

  BENCHMARK_ADVANCED("Enc | uint32_t | copy - max to network")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([ src, destp, size, &err] {
      err = bytes::ToBytes(src, destp, size, bytes::EndianessType::LittleEndian);
      return err;
    });
  };

  
  REQUIRE(vectorex::CompareElements(destp, answer, size) == true);

  delete[] answer;
  delete[] destp;
}
