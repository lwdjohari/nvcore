#define CATCH_CONFIG_MAIN
#include "nvm/dates/datetime.h"

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

  std::cout << "Date1: " << local.ToIso8601() << " [" << local.TzName() << "]" << std::endl;
  std::cout << "Date2: " << local_comp.ToIso8601() << " [" << local_comp.TzName() << "]" << std::endl;
  std::cout << "D1_ptr: " << &(*local.TzTime()) << " D2_ptr: "  << &(*local_comp.TzTime()) << std::endl;
  std::cout << "D1_size: " << sizeof(local) << " D2_size: " << sizeof(local_comp) << std::endl;
  REQUIRE(local.TzTime()->get_local_time() == local_comp.TzTime()->get_local_time() );
}

// TEST_CASE("datetime-utc", "[datetime][utc-test]") {
//   auto local = nvm::dates::DateTime();
//   auto utc = nvm::dates::DateTime::UtcNow();
//   CAPTURE(utc.Timepoint());
//   CAPTURE(local.Timepoint());

//   auto dur = utc.Timepoint() - local.Timepoint();
//   REQUIRE(dur.count() != 0);
// }

// TEST_CASE("datetime-iso-local", "[datetime][local-test]") {
//   auto dt = nvm::dates::DateTime();
//   auto iso = dt.ToIso8601();
//   CAPTURE(iso);
//   std::cout << "local:" << iso << std::endl;
//   REQUIRE(!iso.empty() == true);
// }

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
