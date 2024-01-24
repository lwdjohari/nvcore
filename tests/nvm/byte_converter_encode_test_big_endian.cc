#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>


#include "catch2/catch_all.hpp"
#include "nvm/nvm_core.h"

using namespace nvm;

/** Big-endian byte order */

/** float */
TEST_CASE("Enc | float | max to network", "[tobyte][be-le][float-max]") {
  bytes::ByteOpResult err;
  
  size_t size = 4;
  auto src = std::numeric_limits<float>::max();
  auto answer = new uint8_t[size]{0x7F, 0x7F, 0xFF, 0xFF};
  auto destp = new uint8_t[size];

  BENCHMARK_ADVANCED("Enc | float | pointer - max to network")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([ src, destp, size, &err] {
      err = bytes::ToBytes(src, destp, size, bytes::EndianessType::BigEndian);
      return err;
    });
  };

  REQUIRE(vectorex::CompareElements(destp, answer, size) == true);

  delete[] answer;
  delete[] destp;
}

/** double */
TEST_CASE("Enc | double | max to network", "[tobyte][be-le][double-max]") {
  bytes::ByteOpResult err;
  
  size_t size = 8;
  auto src = std::numeric_limits<double>::max();
  auto answer = new uint8_t[size]{127, 239, 255, 255, 255, 255, 255, 255};

  auto destp = new uint8_t[size];

  BENCHMARK_ADVANCED("Enc | double | copy - max to network")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([ src, destp, size, &err] {
      err = bytes::ToBytes(src, destp, size,  bytes::EndianessType::BigEndian);
      return err;
    });
  };

  
  REQUIRE(vectorex::CompareElements(destp, answer, size) == true);

  delete[] answer;
  delete[] destp;
}

/** uint64_t */
TEST_CASE("Enc | uint64_t | max to network", "[tobyte][be-le][uint64_t-max]") {
  bytes::ByteOpResult err;
  
  size_t size = 8;
  auto src = std::numeric_limits<uint64_t>::max();
  auto answer = new uint8_t[size]{255, 255, 255, 255, 255, 255, 255, 255};
  auto destp = new uint8_t[size];

  BENCHMARK_ADVANCED("Enc | uint64_t | pointer - max to network")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([ src, destp, size, &err] {
      err = bytes::ToBytes(src, destp, size, bytes::EndianessType::BigEndian);
      return err;
    });
  };

  REQUIRE(vectorex::CompareElements(destp, answer, size) == true);

  delete[] answer;
  delete[] destp;
}

/** uint32_t */
TEST_CASE("Enc | uint32_t | max to network", "[tobyte][be-le][uint32_t-max]") {
  bytes::ByteOpResult err;
  

  size_t size = 4;
  auto src = std::numeric_limits<uint32_t>::max();
  auto answer = new uint8_t[size]{255, 255, 255, 255};
  auto destp = new uint8_t[size];

  BENCHMARK_ADVANCED("Enc | uint32_t | pointer - max to network")
  (Catch::Benchmark::Chronometer meter) {
    meter.measure([ src, destp, size, &err] {
      err = bytes::ToBytes(src, destp, size, bytes::EndianessType::BigEndian);
      return err;
    });
  };

  REQUIRE(vectorex::CompareElements(destp, answer, size) == true);

  delete[] answer;
  delete[] destp;
}
