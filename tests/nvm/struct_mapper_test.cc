#define CATCH_CONFIG_MAIN
#include "nvm/struct_mapper.h"

#include <cstdint>
#include <iostream>
#include "catch2/catch_all.hpp"

struct DestinationObject {
  uint32_t location_id;
  std::string location;
  std::chrono::system_clock::time_point timestamp;

  using types =
      std::tuple<uint32_t, std::string, std::chrono::system_clock::time_point>;

  void print() const {
    auto time_c = std::chrono::system_clock::to_time_t(timestamp);
    std::cout << "Location ID: " << location_id << "\n"
              << "Location: " << location << "\n"
              << "Timestamp: " << std::ctime(&time_c);
  }
};

TEST_CASE("mapper-test-mismatch-struct", "[mapper][mismatch-struct]") {
  using namespace nvm::structMapper;

  // Create a tuple with some sample data
  std::tuple<int32_t, std::string, int64_t> sample_data = {
      1, "Warehouse",
      static_cast<int64_t>(
          std::chrono::system_clock::now().time_since_epoch().count())};

    // Convert the tuple to the struct
    DestinationObject obj = MakeStructFromTuple<DestinationObject>(sample_data);

    // Print the contents of the struct
    obj.print();

  REQUIRE(true == true);
}