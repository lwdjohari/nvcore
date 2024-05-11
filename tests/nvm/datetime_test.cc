#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>
#include <memory>

#include "catch2/catch_all.hpp"
#include "nvm/date/datetime.h"

using namespace nvm;

/** float */
TEST_CASE("datetime", "[datetime][local]") {
  
  auto local = nvm::date::DateTime();
  auto local_comp = nvm::date::DateTime(local) ;
  CAPTURE(local.Timepoint());

  REQUIRE(local == local_comp);
}


