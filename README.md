# NvCore
Fluent builder and utils library for C++17 to handling day-to-day use case in 
1. NvSelect: Fluent Generic SQL Builder, SQL Criteria Builder, SQL Sorting (DB driver agnostic) -- Currently only support postgres SQL dialect.
2. NvValidator: Contexable Fluent Validator.
3. Tuple and struct mapper.
4. Binary packer & unpacker.
5. Easy to use & intuitive DateTime with Timezone class.
6. Types utility
7. Flags macro to enable quick enum class to become flags enum.
8. All the swiss army utils for higher productivity.

Designed with header only lib and 
deps only to utf8-cpp and Howard Hinnant date static import
for handling Utf8String and DateTime with timezone.

NvCore mission is only focusing for faster development, eliminates silly mistakes and higher productivity 
in area of generic SQL Builder, SQL Criteria Builder, SQL Sorting, 
Contexable Fluent Validator, Binary packer & unpacker, easy to use DateTime with Timezone. 

The vision: 
Better productivity & better time for developers to enjoying their 
coffee/tea/soft-drink/energy-drink or all at once.

> [!WARNING]
> main branch : v0.3.x and upward is not backward compability with version v0.2.x.
> please find v0.2.x on branch v0.2.1-dev  

## NvSelect: Fluent SQL Builder

## NvValidator
NvValidator is template base fluent validator that can be chaining in contexable manner and<br/>
has reusable common general case validators.<br/>
For flexibility, NvValidator is also supported custom lambda validation and <br/>
class that implemented custom logic overloading operators.

### Example #1
General example
```cxx
#include <iostream>
#include <string>
#include <cstdint>
#include <optional>

#include "nvm/nv_validator.h"

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

int main(){
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
    .IsLength(8, 20, 1002, "Username length must be between 8 and 20")
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

  return 0;
}
```

### Example #2
Object that has logical operator overloading

In this case we are using nvm::dates::DateTime which implemented logical operator overloading.
```cxx
using NvValidator = nvm::validators::NvValidator;
using ValidationOperator = nvm::validators::ValidationOperator;
using ValidationResult = nvm::validators::ValidationResult;
using DateTime = nvm::dates::DateTime;

NvValidator validator;

auto time1 = DateTime::UtcNow();
auto time2 = DateTime(time1);

validator.Validate<DateTime>("datetime",time1,true)
  .IsEqual(time2,1001,"Time is not equal!");

const ValidationResult& result = validator.ValidateAll();
if (!result.is_valid) {
  std::cout << result.GetErrorAsString();
} else {
  std::cout << "All validations passed!" << std::endl;
}
```

### Example #3
Fluent validator will be less usable if it has rigid design, <br/>
to support complex validations scenario that we are faces day-to day <br/>
NvValidator have feature of Custom Lambda Validator.

Lambda function signature
```cxx
[](const T&) noexcept {
  bool state;
  // your custom implementation
  return state;
}
```
Implementation
```cxx
class Balance {
 public:
  Balance(const std::string& account_number, double amount) noexcept
                  : account_number_(account_number), amount_(amount) {}

  double GetBalance() const noexcept {
    return amount_;
  }

  std::string AccountNumber() const noexcept {
    return account_number_;
  }

 private:
  std::string account_number_;
  double amount_;
};

using NvValidator = nvm::validators::NvValidator;
using ValidationOperator = nvm::validators::ValidationOperator;
using ValidationResult = nvm::validators::ValidationResult;

NvValidator validator;

auto balance1 = Balance("90979", 37000);
auto balance2 = Balance("90979", 98000);

validator.Validate<Balance>("custom", balance1, true)
  .IsMust([&balance2](const Balance& b) noexcept {
    return b.GetBalance() < balance2.GetBalance();
  });

const ValidationResult& result = validator.ValidateAll();
if (!result.is_valid) {
  std::cout << result.GetErrorAsString();
} else {
  std::cout << "All validations passed!" << std::endl;
}
```


## DateTime
Nv DateTime is an abstraction of Howard Hinnant Date library.<br/>
We are facing the same painstakingly handling datetime in C++ especially in DateTime with Timezone.<br/>
Nv DateTime is build to supported DateTime with IANA Timezone handling.

Timezone DB for this abstraction currently is set to use the host timezone DB.<br/>
For complete example, please see ```tests/nvm/datetime_test.cc```.

### Include
```cxx
#include "nvm/dates/datetime.h"
```

### Local time Now() with the host Timezone
```cxx
using DateTime = nvm::dates::DateTime;

auto current_local_time = DateTime::Now();
std::cout << "Local time: " << current_local_time << std::endl;

```

### UTC time UtcNow() with the UTC Timezone
```cxx
using DateTime = nvm::dates::DateTime;

auto utc_local_time = DateTime::UtcNow();
std::cout << "UTC time: " << utc_local_time << std::endl;

```

### Specified the DateTime part and the IANA Timezone
```cxx
using DateTime = nvm::dates::DateTime;

auto new_york_time = DateTime(2022, 7, 10, 02, 00, 00, 00, "America/New_York");
std::cout << "New York Time: " << new_york_time << std::endl;

```

### Example 1: Get Start of Month and End of Month from current date.
```cxx
using DateTime = dates::DateTime;

auto jkt_time = DateTime(2022, 7, 10, 13, 0, 0, 0, "Asia/Jakarta");

auto first_day_of_month = jkt_time.GetStartOfMonth();
auto last_day_of_month = jkt_time.GetEndOfMonth();

std::cout << "RESULT" << std::endl;
std::cout << "FIRST   : " << first_day_of_month << std::endl;
std::cout << "LAST    : " << last_day_of_month << std::endl;

```

### Example 2: Add or Subtract Duration from DateTime.
```cxx
using namespace nvm::dates;

auto seconds = 7200;
std::chrono::seconds duration(seconds);
auto now = DateTime();

// Display the current local time
std::cout << "Current local time: \n" << now << std::endl;

auto next = now + ToNanosecondDuration(duration);

// Display the local time after add the duration
std::cout << "Local time after adding " << seconds << " seconds: \n"
          << next << std::endl;

auto result = next - now;
std::chrono::seconds diff =
    std::chrono::duration_cast<std::chrono::seconds>(result.value());

std::cout << "Diff in second: \n" << diff.count() << std::endl;

```

### Example 3: Convert to other IANA Timezone
```cxx
using DateTime = dates::DateTime;
auto jkt_time = DateTime(2022, 7, 10, 13, 00, 00, 00, "Asia/Jakarta");
auto res_utc = jkt_time.ToTimezone("Etc/UTC");
auto res_new_york = jkt_time.ToTimezone("America/New_York");

std::cout << "RESULT" << std::endl;
std::cout << "Jakarta : " << jkt_time << std::endl;
std::cout << "UTC     : " << res_utc << std::endl;
std::cout << "New York: " << res_new_york << std::endl;

```

## How to build
Requirement:
1. CMake
2. Clang or GCC C++17 Compiler (currently developed using clang)

Build:

1. Clone main repository and clone the submodules
```shell
git clone --recurse-submodules https://github.com/lwdjohari/nvm-core.git
```

2. Open in your favorite IDE
3. Run CMake configure
4. Build

## Linking NvCore
To linking NvCore as static library
```cmake
# NvCore deps
add_subdirectory(<nvcore-folder> build-nvcore)

add_library(${PROJECT_NAME}  ${SOURCES} )
target_link_libraries(${PROJECT_NAME} 
    nvcore
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
```

## Contributions

Currently we are still on-going roadmap design and architectural design that might be lead to complete rewrite or complete breaking changes.
We might accept contributors when everything above have better & crytal-clears roadmap.

## License

Copyright [2023] [Linggawasistha Djohari]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the 
