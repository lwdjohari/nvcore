# NvCore
Fluent SQL builder, Fluent Validator, DateTime and utils library for C++17 to handling day-to-day C++17 development use cases.<br/>

[NvSQL: Fluent SQL Builder](#nvsql-fluent-sql-builder-top)<br/>
[NvValidator: Fluent validator](#nvvalidator-top)<br/>
[Nv Datetime: Easy DateTime with Timezone](#nv-datetime-top)<br/>
[How To Build](#how-to-build-top)<br/>
[Contributions](#contributions-top)<br/>
[License](#license-top)<br/>

## NvCore Library Components
1. NvSQL: Fluent Generic SQL Builder, SQL Criteria Builder ( Select, Insert, Update, Delete with DB driver-agnostic design -- Currently only support postgres SQL dialect.
2. NvValidator: Contexable Fluent Validator.
3. Tuple and struct mapper.
4. Binary packer & unpacker.
5. Easy to use & intuitive DateTime with Timezone class.
6. Types utility
7. Flags macro to enable quick enum class to become flags enum.
8. Floating point Approximation and epsilon.
9. All the swiss army utils for higher productivity.

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

## NvSQL: Fluent SQL Builder [[Top]](#nvcore)
NvSQL is an SQL Builder for fluent sql query and parameters builder in SELECT, INSERT, UPDATE and DELETE.<br/>
NvSQL is a DB Driver-Agnostic fluent sql builder which is mean no coupled to certain database driver implementations.<br/>
The library have ParameterParser template specialization approach to convert std::vector<DbSupportedParameterType><br/> 
to other parameter values datatype that has specific implementations.<br/>

### NvSQL Builder Components
1. NvSelect: Fluent SQL SELECT Builder with support of Select, From or From with subquery, Join, Where, Group By and Order By.
2. NvInsert: Fluent SQL INSERT [Warning: WIP & BREAKING CHANGES!]
3. NvUpdate: Fluent SQL INSERT [Warning: WIP & BREAKING CHANGES!]  
4. NvDelete: Fluent SQL INSERT [Warning: WIP & BREAKING CHANGES!]  
   
Notes: <br/>
Currenty only supported PostgreSQL dialect. <br/>
WIP supported other database dialects.<br/>

Complete examples located on ```tests/``` folder.

### NvSQL Include
```
// NvSelect
#include "nvm/sqlbuilder/nv_select_builder.h"

// NvInsert, NvUpdate and NvDelete
#include "nvm/sqlbuilder/operation.h"
```

### Example 1: Select with Join, Where and Order By with Parameters.
```cxx
using DefaultPostgresParamType = nvm::sqlbuilder::DefaultPostgresParamType;
using NvSelect = nvm::sqlbuilder::NvSelect<DefaultPostgresParamType>;
using SqlOperator = nvm::sqlbuilder::SqlOperator;
using RecordKey = nvm::sqlbuilder::RecordKey;
using SqlAggregateFn = nvm::sqlbuilder::SqlAggregateFunction;
using ParameterParser =
    nvm::sqlbuilder::PostgresDefaultParameterParser<DefaultPostgresParamType>;

std::string service_code("RS");
 auto equipment_types = std::vector<std::string>({"HMT", "HLB"});

 auto select = std::make_unique<NvSelect>(1);

 // clang-format off
 
 (*select)
   .Field<int32_t>("equipment_id","e")
   .Field<int32_t>("company_id","c")
   .Field<int32_t>("service_id","s")
   .Field<int32_t>("unit_type_id","e")
   .Field<int32_t>("unit_class_id","e")
   .Field<int32_t>("unit_subclass_id","e")
   .Field<std::string>("reg_id","e")
   .Field<std::string>("code","e", "equipment_code")
   .Field<std::string>("name","c","company_name")
   .Field<std::string>("name","s","service_name")
   .Field<std::string>("code","ut","unit_type_code")
   .Field<std::string>("name","ut","unit_type_name")
   .Field<std::string>("code","uc","unit_class_code")
   .Field<std::string>("name","uc","unit_class_name")
   .Field<std::string>("code","us","unit_sub_class_code")
   .Field<std::string>("name","us", "unit_subclass_name")
   .Field<int16_t>("flags","e")
   .Field<int16_t>("status","e")
   .Field<std::string>("username","ua","add_username")
   .Field<std::string>("username","um", "mod_username")
   .From()
     .AddTable("equipment","e")
   .EndFromTableBlock()
   .Join()
     .InnerJoin(
       RecordKey("equipment","equipment_type_id","e"),
       RecordKey("equipment_type","equipment_type_id","ut"))
     .InnerJoin(
       RecordKey("equipment","equipment_class_id","e"),
       RecordKey("equipment_class","equipment_class_id","uc"))
     .InnerJoin(
       RecordKey("equipment","equipment_sub_class_id","e"),
       RecordKey("equipment_sub_class","equipment_sub_class_id","us"))
     .InnerJoin(
       RecordKey("equipment","service_id","e"),
       RecordKey("services","service_id","s"))
     .InnerJoin(
       RecordKey("services","company_id","s"),
       RecordKey("company","company_id","c"))
     .LeftJoin(
       RecordKey("equipment","add_by","e"),
       RecordKey("users","user_id","ua"))
     .LeftJoin(
       RecordKey("equipment","mod_by","e"),
       RecordKey("users","user_id","um"))
     .InnerJoin(
       RecordKey("equipment","equipment_id","e"),
       RecordKey("ad","equipment_id","ad"))
   .EndJoinBlock()
   .Where()
       .AddConditionIn("unit_type_code",equipment_types)
       .And()
       .AddCondition("s.service_code",SqlOperator::kEqual, service_code)
     .EndWhereBlock()
   .OrderBy()
     .Asc("company_name")
     .Asc("service_name")
     .Asc("unit_type_code")
     .Asc("unit_type_class_code")
   .EndOrderByBlock();

 // clang-format on

 std::cout << select->GenerateQuery(true) << std::endl;

 ParameterParser parser(select->Values());
 std::cout << "\nPARAMETER VALUES:" << std::endl;
 std::cout << parser.GetAllParameterValuesAsString() << std::endl;

```

## NvValidator [[Top]](#nvcore)
NvValidator is template base fluent validator that can be chaining in contexable manner and<br/>
has reusable common general case validators.<br/>
For flexibility, NvValidator is also supported custom lambda validation and <br/>
class that implemented custom logic overloading operators.

### Example #1 [[Top]](#nvcore)
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

### Example #2 [[Top]](#nvcore)
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

### Example #3 [[Top]](#nvcore)
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


## Nv DateTime [[Top]](#nvcore)
Nv DateTime is an abstraction of Howard Hinnant Date library.<br/>
We are facing the same painstakingly handling datetime in C++ especially in DateTime with Timezone.<br/>
Nv DateTime is build to supported DateTime with IANA Timezone handling.

Timezone DB for this abstraction currently is set to use the host timezone DB.<br/>
For complete example, please see ```tests/nvm/datetime_test.cc```.

### Include
```cxx
#include "nvm/dates/datetime.h"
```

### Local time Now() with the host Timezone [[Top]](#nvcore)
```cxx
using DateTime = nvm::dates::DateTime;

auto current_local_time = DateTime::Now();
std::cout << "Local time: " << current_local_time << std::endl;

```

### UTC time UtcNow() with the UTC Timezone [[Top]](#nvcore)
```cxx
using DateTime = nvm::dates::DateTime;

auto utc_local_time = DateTime::UtcNow();
std::cout << "UTC time: " << utc_local_time << std::endl;

```

### Specified the DateTime part and the IANA Timezone [[Top]](#nvcore)
```cxx
using DateTime = nvm::dates::DateTime;

auto new_york_time = DateTime(2022, 7, 10, 02, 00, 00, 00, "America/New_York");
std::cout << "New York Time: " << new_york_time << std::endl;

```

### Example 1: Get Start of Month and End of Month from current date. [[Top]](#nvcore)
```cxx
using DateTime = dates::DateTime;

auto jkt_time = DateTime(2022, 7, 10, 13, 0, 0, 0, "Asia/Jakarta");

auto first_day_of_month = jkt_time.GetStartOfMonth();
auto last_day_of_month = jkt_time.GetEndOfMonth();

std::cout << "RESULT" << std::endl;
std::cout << "FIRST   : " << first_day_of_month << std::endl;
std::cout << "LAST    : " << last_day_of_month << std::endl;

```

### Example 2: Add or Subtract Duration from DateTime. [[Top]](#nvcore)
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

### Example 3: Convert to other IANA Timezone [[Top]](#nvcore)
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

## How to build [[Top]](#nvcore)
Requirement:
1. CMake
2. Clang or GCC C++17 Compiler (currently developed using clang)

Build:

1. Clone main repository and clone the submodules
```shell
git clone --recurse-submodules .git
```

2. Open in your favorite IDE
3. Run CMake configure
4. Build

## Linking NvCore [[Top]](#nvcore)
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

## Contributions [[Top]](#nvcore)

Currently we are still on-going roadmap design and architectural design that might be lead to complete rewrite or complete breaking changes.
We might accept contributors when everything above have better & crytal-clears roadmap.

## License [[Top]](#nvcore)

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
