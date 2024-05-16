#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>

#include "catch2/catch_all.hpp"
#include "nvm/containers/validations.h"

using namespace nvm;

class CustomObject {
 public:
  CustomObject(int a, double b) noexcept : a_(a), b_(b) {}
  int GetA() const noexcept { return a_; }
  double GetB() const noexcept { return b_; }

 private:
  int a_;
  double b_;
};

TEST_CASE("validation-test", "[validator][normal-case]") {
  // using Validator = nvm::containers::validations::Validator;
  using NvValidator = nvm::containers::validations::NvValidator;
  using ValidationOperator = nvm::containers::validations::ValidationOperator;
  using ValidationResult = nvm::containers::validations::ValidationResult;

  NvValidator validator;

  std::string value = "john_doe";
  std::string email = "john.doe@example.com";
  uint32_t age = 25;
  std::optional<std::string> optional_value = "Hello world";

  CustomObject obj(10, 3.14);
  
  validator.Validate<std::string>("username", value, true)
      .IsNotEmptyNullOrWhiteSpace(1001, "Username must not be empty")
      .IsLength(3, 20, 1002, "Username length must be between 3 and 20")
      .IsAlphanumeric({'_'}, 1003, "Username must be alphanumeric");

  validator.Validate<std::string>("email", email, true)
      .IsLength(8, 80, 1004, "Email length must be between 8 and 80")
      .IsEmail({'-', '_', '.'}, 1005, "Email must be valid");

  validator.Validate<uint32_t>("age",age,true)
    .Is(ValidationOperator::kGreaterOrEqual,18,1006,"Minimum age must be 18 years old");

  validator
      .Validate<std::optional<std::string>>("optional_value", optional_value,
                                            true)
      .IsNotNull(3001, "Optional value must not be null")
      .IsNotEmpty(3002, "Optional value must not be empty");

  validator.Validate<CustomObject>("custom_object", obj, true)
      .Is([](const CustomObject& o) noexcept { return o.GetA() > 5; }, 4001,
          "CustomObject.a must be greater than 5")
      .Is([](const CustomObject& o) noexcept { return o.GetB() < 4.0; }, 4002,
          "CustomObject.b must be less than 4.0");

  const ValidationResult& result = validator.ValidateAll();
  if (!result.is_valid) {
    std::cout << result.GetErrorAsString();
  } else {
    std::cout << "All validations passed!" << std::endl;
  }

  REQUIRE(result.is_valid == true);
}


TEST_CASE("validation-test-failed", "[validator][normal-case]") {
  // using Validator = nvm::containers::validations::Validator;
  using NvValidator = nvm::containers::validations::NvValidator;
  using ValidationOperator = nvm::containers::validations::ValidationOperator;
  using ValidationResult = nvm::containers::validations::ValidationResult;

  NvValidator validator;

  std::string value = "   ";
  std::string email = "john.doe_example.com";
  uint32_t age = 13;
  std::optional<std::string> optional_value = std::nullopt;

  CustomObject obj(10, 3.14);
  
  validator.Validate<std::string>("username", value, true)
      .IsNotEmptyNullOrWhiteSpace(1001, "Username must not be empty")
      .IsLength(3, 20, 1002, "Username length must be between 3 and 20")
      .IsAlphanumeric({'_'}, 1003, "Username must be alphanumeric");

  validator.Validate<std::string>("email", email, true)
      .IsLength(8, 80, 1004, "Email length must be between 8 and 80")
      .IsEmail({'-', '_', '.'}, 1005, "Email must be valid");

  validator.Validate<uint32_t>("age",age,true)
    .Is(ValidationOperator::kGreaterOrEqual,18,1006,"Minimum age must be 18 years old");

  validator
      .Validate<std::optional<std::string>>("optional_value", optional_value,
                                            true)
      .IsNotNull(3001, "Optional value must not be null")
      .IsNotEmpty(3002, "Optional value must not be empty");

  validator.Validate<CustomObject>("custom_object", obj, true)
      .Is([](const CustomObject& o) noexcept { return o.GetA() < 5; }, 4001,
          "CustomObject.a must be greater than 5")
      .Is([](const CustomObject& o) noexcept { return o.GetB() > 4.0; }, 4002,
          "CustomObject.b must be less than 4.0");

  const ValidationResult& result = validator.ValidateAll();
  if (!result.is_valid) {
    std::cout << result.GetErrorAsString();
  } else {
    std::cout << "All validations passed!" << std::endl;
  }

  REQUIRE(result.is_valid == false);
}
