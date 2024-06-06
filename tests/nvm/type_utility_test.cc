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


// Test cases
class A {
 public:
  void PrintCache() const {
    std::cout << "PrintCache without arguments" << std::endl;
  }
  void PrintCache(int value) const {
    std::cout << "PrintCache with argument: " << value << std::endl;
  }
};

struct B {
  void PrintCache(int) const;
};

struct C {
  double PrintCache(double);
};

struct D {
  void PrintCache(const int&);
};

struct E {
  void PrintCache(int&&);
};

struct F {
  void PrintCache(int*);
};

struct G {
  void PrintCache(const int&) const;
};

struct H {
  void PrintCache(const int&&) const;
};

struct I {
  void PrintCache(const int*);
};

struct J {
  const int* PrintCache() const;
};

struct K {
  const int PrintCache() const;
};

// cppcheck-suppress unknownMacro
NVM_DEFINE_TYPE_HAS_METHOD(nvm, has_print_cache, PrintCache)

// Correct usage

TEST_CASE("[meta-has_method] has_method tests", "[meta-method]") {
  STATIC_REQUIRE(nvm_has_print_cache<A, void()>::value);
  STATIC_REQUIRE(nvm_has_print_cache<A, void(int)>::value);
  STATIC_REQUIRE(nvm_has_print_cache<B, void(int)>::value);
  STATIC_REQUIRE(nvm_has_print_cache<C, double(double)>::value);
  STATIC_REQUIRE(nvm_has_print_cache<D, void(const int&)>::value);
  STATIC_REQUIRE(nvm_has_print_cache<E, void(int&&)>::value);
  STATIC_REQUIRE(nvm_has_print_cache<F, void(int*)>::value);
  STATIC_REQUIRE(nvm_has_print_cache<G, void(const int&)>::value);
  STATIC_REQUIRE(nvm_has_print_cache<H, void(const int&&)>::value);
  STATIC_REQUIRE(nvm_has_print_cache<I, void(const int*)>::value);
  STATIC_REQUIRE(nvm_has_print_cache<J, const int*()>::value);
  STATIC_REQUIRE(nvm_has_print_cache<K, const int()>::value);
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