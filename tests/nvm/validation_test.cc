#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>

#include "catch2/catch_all.hpp"
#include "nvm/nv_validator.h"

using namespace nvm;

class CustomObject {
 public:
  CustomObject(int a, double b) noexcept : a_(a), b_(b) {}
  int GetA() const noexcept {
    return a_;
  }
  double GetB() const noexcept {
    return b_;
  }

 private:
  int a_;
  double b_;
};

TEST_CASE("validation-test", "[validator][normal-case]") {
  
  using NvValidator = nvm::validators::NvValidator;
  using ValidationOperator = nvm::validators::ValidationOperator;
  using ValidationResult = nvm::validators::ValidationResult;

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

  validator.Validate<uint32_t>("age", age, true)
      .IsGreaterThanEqual(18, 1006, "Minimum age must be 18 years old");

  validator
      .Validate<std::optional<std::string>>("optional_value", optional_value,
                                            true)
      .IsNotEmpty(3002, "Optional value must not be empty");

  validator.Validate<CustomObject>("custom_object", obj, true)
      .IsMust([](const CustomObject& o) noexcept { return o.GetA() > 5; }, 4001,
              "CustomObject.a must be greater than 5")
      .IsMust([](const CustomObject& o) noexcept { return o.GetB() < 4.0; },
              4002, "CustomObject.b must be less than 4.0");

  const ValidationResult& result = validator.ValidateAll();
  if (!result.is_valid) {
    std::cout << result.GetErrorAsString();
  } else {
    std::cout << "All validations passed!" << std::endl;
  }

  REQUIRE(result.is_valid == true);
}

TEST_CASE("validation-test-failed", "[validator][normal-case]") {
  // using Validator = nvm::validators::Validator;
  using NvValidator = nvm::validators::NvValidator;
  using ValidationOperator = nvm::validators::ValidationOperator;
  using ValidationResult = nvm::validators::ValidationResult;

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

  validator.Validate<uint32_t>("age", age, true)
      .IsGreaterThanEqual(18, 1006, "Minimum age must be 18 years old");

  validator
      .Validate<std::optional<std::string>>("optional_value", optional_value,
                                            true)
      .IsNotEmpty(3002, "Optional value must not be empty");

  validator.Validate<CustomObject>("custom_object", obj, true)
      .IsMust([](const CustomObject& o) noexcept { return o.GetA() < 5; }, 4001,
              "CustomObject.a must be greater than 5")
      .IsMust([](const CustomObject& o) noexcept { return o.GetB() > 4.0; },
              4002, "CustomObject.b must be less than 4.0");

  const ValidationResult& result = validator.ValidateAll();
  if (!result.is_valid) {
    std::cout << result.GetErrorAsString();
  } else {
    std::cout << "All validations passed!" << std::endl;
  }

  REQUIRE(result.is_valid == false);
}

TEST_CASE("validation-email", "[validator][normal-case]") {
  // using Validator = nvm::validators::Validator;
  using NvValidator = nvm::validators::NvValidator;
  using ValidationOperator = nvm::validators::ValidationOperator;
  using ValidationResult = nvm::validators::ValidationResult;

  NvValidator validator;

  int32_t min, max;
  min = 8;
  max = 80;
  std::string email_c1 = "john.doe@example.com";
  std::string email_c2 = "john_doe@example.com";
  std::string email_c3 = "john-doe1236.1212@example.com";
  std::string email_c4 = "john-doe.sgp@example.com";
  std::string email_c5 = "john-doe-sgp@example.com";
  std::string email_c6 = "john--doe_-.sgp@example.com";

  validator.Validate<std::string>("email_c1", email_c1)
      .IsLength(min, max)
      .IsEmail({'.', '_', '-'});

  validator.Validate<std::string>("email_c2", email_c2)
      .IsLength(min, max)
      .IsEmail({'.', '_', '-'});

  validator.Validate<std::string>("email_c3", email_c3)
      .IsLength(min, max)
      .IsEmail({'.', '_', '-'});

  validator.Validate<std::string>("email_c4", email_c4)
      .IsLength(min, max)
      .IsEmail({'.', '_', '-'});

  validator.Validate<std::string>("email_c5", email_c5)
      .IsLength(min, max)
      .IsEmail({'.', '_', '-'});

  validator.Validate<std::string>("email_c6", email_c6)
      .IsLength(min, max)
      .IsEmail({'.', '_', '-'});

  const ValidationResult& result = validator.ValidateAll();
  if (!result.is_valid) {
    std::cout << result.GetErrorAsString();
  } else {
    std::cout << "All validations passed!" << std::endl;
  }

  REQUIRE(result.is_valid == true);
}

TEST_CASE("validation-email-failed", "[validator][normal-case]") {
  using NvValidator = nvm::validators::NvValidator;
  using ValidationOperator = nvm::validators::ValidationOperator;
  using ValidationResult = nvm::validators::ValidationResult;

  NvValidator validator;

  int32_t min, max;
  min = 8;
  max = 80;
  std::string email_c1 = "john.doe@example.com";
  std::string email_c2 = "john_doe@example.com";
  std::string email_c3 = "john-doe1236.1212@example.com";
  std::string email_c4 = "john-doe.sgp@example.com";
  std::string email_c5 = "john-doe-sgp@example.com";
  std::string email_c6 = "john--doe_-.sgp@example.com";

  validator.Validate<std::string>("email_c1", email_c1)
      .IsLength(min, max)
      .IsEmail({'.'});

  validator.Validate<std::string>("email_c2", email_c2)
      .IsLength(min, max)
      .IsEmail({'.'});

  validator.Validate<std::string>("email_c3", email_c3)
      .IsLength(min, max)
      .IsEmail({'.'});

  validator.Validate<std::string>("email_c4", email_c4)
      .IsLength(min, max)
      .IsEmail({'.'});

  validator.Validate<std::string>("email_c5", email_c5)
      .IsLength(min, max)
      .IsEmail({
          '.',
      });

  validator.Validate<std::string>("email_c6", email_c6)
      .IsLength(min, max)
      .IsEmail({'.'});

  const ValidationResult& result = validator.ValidateAll();
  if (!result.is_valid) {
    std::cout << result.GetErrorAsString();
  } else {
    std::cout << "All validations passed!" << std::endl;
  }

  REQUIRE(result.is_valid == false);
}

TEST_CASE("validation-smart_ptr", "[validator][normal-case]") {
  using NvValidator = nvm::validators::NvValidator;
  using ValidationOperator = nvm::validators::ValidationOperator;
  using ValidationResult = nvm::validators::ValidationResult;

  NvValidator validator;

  int32_t min, max;
  min = 8;
  max = 80;
  auto email = std::make_shared<std::string>("john.doe@example.com");
  auto username = std::make_shared<std::string>("gabriela");
  auto numeric_string = std::make_shared<std::string>("1234.56");
  auto c_object = std::make_shared<CustomObject>(10, 15);
  // auto age = std::make_shared<uint32_t>(25);
  // auto quality = std::make_shared<float>(100);

  validator.Validate<std::shared_ptr<std::string>>("email", email)
      .IsNotEmptyNullOrWhiteSpace()
      .IsLength(min, max)
      .IsEmail({'.', '_', '-'});

  validator.Validate<std::shared_ptr<std::string>>("username", username)
      .IsNotEmptyNullOrWhiteSpace()
      .IsLength(8, 16)
      .IsAlphanumeric({'.', '_', '-'});

  validator.Validate<std::shared_ptr<std::string>>("numeric", numeric_string)
      .IsNumericString();

  validator.Validate<std::shared_ptr<std::string>>("alphabet", username)
      .IsAlphabet();

  validator.Validate<std::shared_ptr<CustomObject>>("custom_object", c_object)
      .IsNotNull();

  // TODO: FIX IMPLEMENTATION OF ARITHMATIC TYPE SMART PTR
  // THIS IS UB FEATURE FOR NOW
  // validator.Validate<std::shared_ptr<uint32_t>>("age", age)
  //     .IsGreaterThanEqual(std::make_shared<uint32_t>(18));

  // validator.Validate<std::shared_ptr<float>>("quality", quality)
  //     .IsBetween(std::make_shared<float>(0),std::make_shared<float>(100));

  const ValidationResult& result = validator.ValidateAll();
  if (!result.is_valid) {
    std::cout << result.GetErrorAsString();
  } else {
    std::cout << "All validations passed!" << std::endl;
  }

  REQUIRE(result.is_valid == true);
}

TEST_CASE("validation-numeric-string", "[validator][normal-case]") {
  using NvValidator = nvm::validators::NvValidator;
  using ValidationOperator = nvm::validators::ValidationOperator;
  using ValidationResult = nvm::validators::ValidationResult;

  NvValidator validator;

  std::string n1 = "2183";
  std::string n2 = "0.456";
  std::string n3 = "-0.456";
  std::string n4 = "-1243";
  std::string n5 = "-90998.98";
  std::string n6 = "-90000000.00";

  validator.Validate<std::string>("n1", n1)
      .IsNotEmptyNullOrWhiteSpace()
      .IsNumericString();

  validator.Validate<std::string>("n2", n2)
      .IsNotEmptyNullOrWhiteSpace()
      .IsNumericString();

  validator.Validate<std::string>("n3", n3)
      .IsNotEmptyNullOrWhiteSpace()
      .IsNumericString();

  validator.Validate<std::string>("n4", n4)
      .IsNotEmptyNullOrWhiteSpace()
      .IsNumericString();

  validator.Validate<std::string>("n5", n5)
      .IsNotEmptyNullOrWhiteSpace()
      .IsNumericString();

  validator.Validate<std::string>("n6", n6)
      .IsNotEmptyNullOrWhiteSpace()
      .IsNumericString();

  const ValidationResult& result = validator.ValidateAll();
  if (!result.is_valid) {
    std::cout << result.GetErrorAsString();
  } else {
    std::cout << "All validations passed!" << std::endl;
  }

  REQUIRE(result.is_valid == true);
}

TEST_CASE("validation-aphabet", "[validator][normal-case]") {
  using NvValidator = nvm::validators::NvValidator;
  using ValidationOperator = nvm::validators::ValidationOperator;
  using ValidationResult = nvm::validators::ValidationResult;

  NvValidator validator;

  std::string n1 = "the quick brown fox jump over the lazy dogs";
  std::string n2 = "AbCdE";
  std::string n3 = "AbCdE-6283";
  std::string n4 = "AbCdE.";
  std::string n5 = "AbCdE_";
  std::string n6 = "_., ";

  validator.Validate<std::string>("n1", n1)
      .IsNotEmptyNullOrWhiteSpace()
      .IsAlphabet({' '});

  validator.Validate<std::string>("n2", n2)
      .IsNotEmptyNullOrWhiteSpace()
      .IsAlphabet();

  // Although this is not the best practice
  // Use IsRegexString instead!
  // This just to give an ilustrations of possibility but with overhead penalty
  validator.Validate<std::string>("n3", n3)
      .IsNotEmptyNullOrWhiteSpace()
      .IsAlphabet({' ', '-', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'});

  validator.Validate<std::string>("n4", n4)
      .IsNotEmptyNullOrWhiteSpace()
      .IsAlphabet({' ', '.', ',', '_'});

  validator.Validate<std::string>("n5", n5)
      .IsNotEmptyNullOrWhiteSpace()
      .IsAlphabet({' ', '.', ',', '_'});

  validator.Validate<std::string>("n6", n6)
      .IsNotEmptyNullOrWhiteSpace()
      .IsAlphabet({' ', '.', ',', '_'});

  const ValidationResult& result = validator.ValidateAll();
  if (!result.is_valid) {
    std::cout << result.GetErrorAsString();
  } else {
    std::cout << "All validations passed!" << std::endl;
  }

  REQUIRE(result.is_valid == true);
}