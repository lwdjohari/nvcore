#define CATCH_CONFIG_MAIN
#include "nvm/types/type_utility.h"

#include <cstdint>
#include <iostream>

#include "catch2/catch_all.hpp"
using namespace nvm::types::utility;

class BasicComponent {
 public:
  BasicComponent() {}
  explicit BasicComponent(int, double) {}
  std::string IsPrint(int) {
    return std::string();
  }
};

class AdvancedComponent : public BasicComponent {
 public:
  explicit AdvancedComponent(const char* x, int y) : BasicComponent(y, y) {}
};

class AdvancheChild : public AdvancedComponent {
 public:
  explicit AdvancheChild() : AdvancedComponent("test", 10) {}
};

class NonConstructibleComponent {
 protected:
  explicit NonConstructibleComponent(int) {}  // Protected constructor
};

class ReferenceComponent {
 public:
  explicit ReferenceComponent(int&) {}
  explicit ReferenceComponent(const int&) {}
  explicit ReferenceComponent(int&&) {}
};

struct A {
  std::string generate(int) {
    return "Print A";
  }
  std::string toString(int) {
    return "A";
  }
  void Print() {}
  const int* PrintCache() const {
    static int x = 0;
    return &x;
  }
  int& PrintIndex() {
    static int x = 0;
    return x;
  }
};

// Define traits to check for specific member functions
NVM_DEFINE_HAS_MEMBER(nvm, has_print, Print)
NVM_DEFINE_HAS_MEMBER(nvm, has_print_index, PrintIndex)
NVM_DEFINE_HAS_MEMBER(nvm, has_to_string, toString)
NVM_DEFINE_HAS_MEMBER(nvm, has_generate, generate)
NVM_DEFINE_HAS_MEMBER(nvm, has_print_cache, PrintCache)

TEST_CASE("[meta-has_method] has_method tests", "[meta-method]") {
  STATIC_REQUIRE_FALSE(nvm_has_print<A, std::string(int)>::value);
  STATIC_REQUIRE(nvm_has_to_string<A, std::string(int)>::value);
  STATIC_REQUIRE(nvm_has_generate<A, std::string(int)>::value);
  STATIC_REQUIRE(nvm_has_print<A, void()>::value);
  //   STATIC_REQUIRE (nvm_has_print_cache<A, const int*() const>::value );
  STATIC_REQUIRE(nvm_has_print_index<A, int&()>::value);
}

TEST_CASE("[meta-has_base_of] has_base_of tests", "[meta-has_base_of]") {
  STATIC_REQUIRE(is_has_base_of_v<AdvancedComponent, BasicComponent>);
  STATIC_REQUIRE(is_has_base_of_v<AdvancheChild, BasicComponent>);
  STATIC_REQUIRE_FALSE(is_has_base_of_v<BasicComponent, AdvancedComponent>);
  STATIC_REQUIRE_FALSE(is_has_base_of_v<AdvancedComponent, ReferenceComponent>);
  STATIC_REQUIRE_FALSE(is_has_base_of_v<ReferenceComponent, AdvancedComponent>);
}

TEST_CASE("[meta-construct] BasicComponent constructibility checks",
          "[meta-construct]") {
  STATIC_REQUIRE(is_constructible_v<BasicComponent>);
  STATIC_REQUIRE(is_constructible_v<BasicComponent, int, double>);
  STATIC_REQUIRE_FALSE(is_constructible_v<BasicComponent, int, std::string>);

  // This is fundamental limitation
  // implicit conversion still happen, it up to dev to avoid this UB
  STATIC_REQUIRE(is_constructible_v<BasicComponent, double, int>);
}

TEST_CASE("[meta-construct] AdvancedComponent constructibility checks",
          "[meta-construct]") {
  STATIC_REQUIRE(is_constructible_v<AdvancedComponent, const char*, int>);
  STATIC_REQUIRE_FALSE(is_constructible_v<AdvancedComponent, int, double>);
}

TEST_CASE("[meta-construct] NonConstructibleComponent constructibility checks",
          "[meta-construct]") {
  STATIC_REQUIRE_FALSE(is_constructible_v<NonConstructibleComponent, int>);
}

TEST_CASE("[meta-construct] ReferenceComponent constructibility checks",
          "[meta-construct]") {
  STATIC_REQUIRE(is_constructible_v<ReferenceComponent, int&>);
  STATIC_REQUIRE(is_constructible_v<ReferenceComponent, const int&>);
  STATIC_REQUIRE(is_constructible_v<ReferenceComponent, int&&>);

  // This is fundamental limitation
  // implicit conversion still happen, it up to dev to avoid this UB
  STATIC_REQUIRE(is_constructible_v<ReferenceComponent, double>);
}