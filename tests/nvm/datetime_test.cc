#define CATCH_CONFIG_MAIN
#include "nvm/date/datetime.h"

#include <cstdint>
#include <iostream>
#include <memory>

#include "catch2/catch_all.hpp"

using namespace nvm;

/** float */
TEST_CASE("datetime", "[datetime][local-test]") {
  auto local = nvm::date::DateTime();
  auto local_comp = nvm::date::DateTime(local);
  CAPTURE(local.Timepoint());

  REQUIRE(local == local_comp);
}

TEST_CASE("datetime-utc", "[datetime][utc-test]") {
  auto local = nvm::date::DateTime();
  auto utc = nvm::date::DateTime::UtcNow();
  CAPTURE(utc.Timepoint());
  CAPTURE(local.Timepoint());

  auto dur = utc.Timepoint() - local.Timepoint();
  REQUIRE(dur.count() != 0);
}

TEST_CASE("iso-local", "[datetime][local-test]") {
  auto dt = nvm::date::DateTime();
  auto iso = dt.ToIso8601();
  CAPTURE(iso);
  std::cout << "local:" << iso << std::endl;
  REQUIRE(!iso.empty() == true);
}

TEST_CASE("iso-utc", "[datetime][utc-test]") {
  auto dt = nvm::date::DateTime::UtcNow();
  auto iso = dt.ToIso8601();
  CAPTURE(iso);
  std::cout << "utc:" << iso << std::endl;
  REQUIRE(!iso.empty() == true);
}
