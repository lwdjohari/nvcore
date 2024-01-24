#define CATCH_CONFIG_MAIN
#include <cstdint>

#include "catch2/catch_all.hpp"
#include "nvm/logic.h"
#include "nvm/maths/approx.h"

using namespace nvm;

TEST_CASE("logic::IsBetween double test eq", "[logic][IsBetween][double]") {
  double val = 0.1f;
  double start = 0.025f;
  double end = 0.5f;
  auto res = logic::IsBetween(val,start,end);
  REQUIRE(res==true);
}

TEST_CASE("logic::IsBetween double test !eq", "[logic][IsBetween][double]") {
  double val = 0.1f;
  double start = 0.15f;
  double end = 0.5f;
  auto res = logic::IsBetween(val,start,end);
  REQUIRE(res==false);
}


TEST_CASE("logic::IsBetween double test zero", "[logic][IsBetween][double]") {
  double val = 0;
  double start = 0;
  double end = 0;
  auto res = logic::IsBetween(val,start,end);
  REQUIRE(res==true);
}


TEST_CASE("logic::IsIntersect double test eq", "[logic][IsBetween][double]") {

  auto range1 = logic::Range(0.1l,0.9l);
  auto range2 = logic::Range(0.8l,1.0l);

  REQUIRE(range1.IsValid()==true);
  REQUIRE(range2.IsValid()==true);

  auto res = logic::IsIntersect(range1,range2);
  REQUIRE(res==true);
}

TEST_CASE("logic::GetIntersect double test eq", "[logic][IsBetween][double]") {

  auto range1 = logic::Range(0.1,0.9);
  auto range2 = logic::Range(0.8,1.0);

  REQUIRE(range1.IsValid()==true);
  REQUIRE(range2.IsValid()==true);
 
  auto res = logic::GetIntersect(range1,range2);
  REQUIRE(res.start ==0.8);
  REQUIRE(res.end ==0.9);
  REQUIRE(res.Distance() == maths::Approx(0.1).epsilon(0.01));
}
