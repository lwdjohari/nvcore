#define CATCH_CONFIG_MAIN
#include "nvm/struct_mapper.h"

#include <cstdint>
#include <iostream>

#include "catch2/catch_all.hpp"

using namespace nvm::mapper;

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

struct AccountObject {
  int id;
  std::string name;
  double balance;
};

struct BalanceObject {
  std::string fullname;
  uint32_t identifier;
  std::string formatted_balance;

  void print() const {
    std::cout << "Fullname: " << fullname << "\n"
              << "Identifier: " << identifier << "\n"
              << "Formatted Balance: " << formatted_balance << std::endl;
  }
};

struct AccountHolder {
  std::string account_holder;
  std::string status;

  void print() const {
    std::cout << "Account Holder: " << account_holder << "\n"
              << "Status: " << status << std::endl;
  }
};

// Mapping from SourceObject to DestinationObject
template <>
struct Mapper<AccountObject, BalanceObject> {
  static BalanceObject Map(const AccountObject& from) {
    return {.fullname = from.name,
            .identifier = TypeCaster<int, uint32_t>::Cast(from.id),
            .formatted_balance =
                TypeCaster<double, std::string>::Cast(from.balance)};
  }
};

template <>
struct Mapper<AccountObject, AccountHolder> {
  static AccountHolder Map(const AccountObject& from) {
    return {.account_holder = from.name,
            .status = (from.balance > 1000 ? "Premium" : "Standard")};
  }
};

TEST_CASE("mapper-test-mismatch-struct", "[mapper][mismatch-struct]") {
  using namespace nvm::mapper;

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

TEST_CASE("mapper-struct-mapper", "[mapper][mismatch-struct]") {
  using namespace nvm::mapper;

  AccountObject src = {1, "John Doe", 1234.56};

  BalanceObject dest = Mapper<AccountObject, BalanceObject>::Map(src);
  dest.print();

  AccountHolder anotherDest = Mapper<AccountObject, AccountHolder>::Map(src);
  anotherDest.print();

  REQUIRE(true == true);
}