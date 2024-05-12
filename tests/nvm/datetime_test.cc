#define CATCH_CONFIG_MAIN
#include "nvm/dates/datetime.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>

#include "catch2/catch_all.hpp"
#include "date/tz.h"

using namespace nvm;

TEST_CASE("datetime", "[datetime][local-test]") {
  auto local = nvm::dates::DateTime("Etc/UTC");
  auto local_comp = nvm::dates::DateTime(local);
  CAPTURE(local.TzTime()->get_local_time());
  CAPTURE(local_comp.TzTime()->get_local_time());

  std::cout << "Date1: " << local.ToIso8601() << " [" << local.TzName() << "]"
            << std::endl;
  std::cout << "Date2: " << local_comp.ToIso8601() << " ["
            << local_comp.TzName() << "]" << std::endl;
  std::cout << "D1_ptr: " << &(*local.TzTime())
            << " D2_ptr: " << &(*local_comp.TzTime()) << std::endl;
  std::cout << "D1_size: " << sizeof(local)
            << " D2_size: " << sizeof(local_comp) << std::endl;
  REQUIRE(local.TzTime()->get_local_time() ==
          local_comp.TzTime()->get_local_time());
}

TEST_CASE("datetime-print", "[datetime][local-test]") {
  auto now = std::chrono::system_clock::now();

  // Convert it to local time using the HH Date library
  auto local_time = date::make_zoned(date::current_zone(), now);

  // Print the local time
  std::cout << "Current local time: " << local_time << '\n';

  REQUIRE(local_time.get_time_zone() != nullptr);
}

TEST_CASE("datetime-utc-print", "[datetime][utc-test]") {
  auto now = std::chrono::system_clock::now();

  // Convert it to local time using the HH Date library
  auto local_time = date::make_zoned(date::locate_zone("Etc/UTC"), now);

  // Print the local time
  std::cout << "Current UTC time: " << local_time << '\n';

  REQUIRE(local_time.get_time_zone() != nullptr);
}

TEST_CASE("datetime-test-add-duration", "[datetime][local-test]") {
  auto seconds = 7200;
  std::chrono::seconds duration(seconds);
  auto now = nvm::dates::DateTime();
  
  // Display the current local time
  std::cout << "Current local time: \n" << *now.TzTime()
            << std::endl;

  auto next = now + duration;

  // Display the local time after adding the duration
  std::cout << "Local time after adding " << seconds
            << " seconds: \n" << *next.TzTime()
            << std::endl;

  REQUIRE(next.TzTime() != nullptr);
}

TEST_CASE("datetime-test-subtract-duration", "[datetime][local-test]") {
  auto seconds = 7200;
  std::chrono::seconds duration(seconds);
  auto now = nvm::dates::DateTime();
  
  // Display the current local time
  std::cout << "Current local time: \n" << *now.TzTime()
            << std::endl;

  auto next = now - duration;

  // Display the local time after subtract the duration
  std::cout << "Local time after subtract " << seconds
            << " seconds: \n" <<  *next.TzTime()
            << std::endl;

  REQUIRE(next.TzTime() != nullptr);
}

// TEST_CASE("datetime-iso-utc", "[datetime][utc-test]") {
//   auto dt = nvm::dates::DateTime::UtcNow();
//   auto iso = dt.ToIso8601();
//   CAPTURE(iso);
//   std::cout << "utc:" << iso << std::endl;
//   REQUIRE(!iso.empty() == true);
// }

// TEST_CASE("datetime-tz-offset", "[datetime][utc-test]") {
//   auto offset = nvm::dates::GetSystemTimezoneOffset();

//   CAPTURE(offset);
//   std::cout << "offset:" << offset.count() << std::endl;
//   REQUIRE(offset.count() > 0);
// }
