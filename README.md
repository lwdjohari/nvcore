# NvCore
Fluent SQL builder, Fluent Validator, Tuple and Object Mapper, DateTime and utils library for C++17<br/> to handling day-to-day C++17 development use cases.<br/>

[NvSQL: Fluent SQL Builder](#nvsql-fluent-sql-builder-top)<br/>
[NvValidator: Fluent validator](#nvvalidator-top)<br/>
[Nv Datetime: Easy DateTime with Timezone](#nv-datetime-top)<br/>
[How To Build](#how-to-build-top)<br/>
[Contributions](#contributions-top)<br/>
[License](#license-top)<br/>

> [!WARNING]
> main branch : v0.3.x and upward is not backward compability with v0.2.x.<br/>
> please find v0.2.x on branch v0.2.1-dev 

> [!WARNING]
> v0.3.x lib name ```nvcore```<br/>
> v0.2.x lib name ```nvm-core``` 

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

NvCore mission is focusing for faster development, eliminates silly mistakes and higher productivity 
in area of generic SQL Builder, SQL Criteria Builder, SQL Sorting, 
Contexable Fluent Validator, Binary packer & unpacker, easy to use DateTime with Timezone. 

Visions:
Better productivity & better time for developers to enjoying their 
coffee/tea/soft-drink/energy-drink or all at once.



# NvSQL: Fluent SQL Builder [[Top]](#nvcore)
NvSQL is a SQL Builder for fluent sql query and parameters builder in SELECT, INSERT, UPDATE and DELETE.<br/>
NvSQL is a DB Driver-Agnostic fluent sql builder which is mean no coupled to certain database driver implementations.<br/>
The library have ParameterParser template specialization approach to convert std::vector&lt;DbSupportedParameterType&gt;<br/> 
to other parameter values datatype that has specific implementations.<br/>

## NvSQL Builder Components
1. NvSelect: Fluent SQL SELECT Builder with support of Select, From or From with subquery, Join, Where, Group By and Order By.
2. NvInsert: Fluent SQL INSERT [Warning: WIP & BREAKING CHANGES!]
3. NvUpdate: Fluent SQL INSERT [Warning: WIP & BREAKING CHANGES!]  
4. NvDelete: Fluent SQL INSERT [Warning: WIP & BREAKING CHANGES!]  
   
><b>Notes:</b> <br/>
>Currently only supported PostgreSQL dialect. <br/>
>WIP to supported other database dialects.<br/>

Complete examples located on ```tests/nvm``` folder.

### NvSQL Include
```
// NvSelect
#include "nvm/sqlbuilder/nv_select_builder.h"

// NvInsert, NvUpdate and NvDelete
#include "nvm/sqlbuilder/operation.h"
```


## NvSelect

NvSelect is a fluent SQL Select Builder that is DB Driver-agnostic that is not coupled to any specific DB driver implementations.<br/>

```cxx
// Instancing NvSelect with start parameter from 1
auto select = std::make_unique<NvSelect>(1);
```
Field and Function Call Usage
```cxx
(*select)
  // Use the Field<T> method if you need to generate row tuples
  .Field<int32_t>("equipment_id","e") 

  // Use the Field method if you don't need to generate row tuples
  .Field("equipment_id","e")

  // Function call 
  // Send parameters as vector<std::string>
  // alias the output AS Brand column
  // For advance parameterized values still in WIP.  
  .Fn("UPPER",{"e.Brand"},"Brand")
 
```

FROM Statement Block Usage
```cxx
(*select)
  // FROM STATEMENT BLOCK
  .From() // Create SQL FROM Statement block 
    .AddTable("table_name","alias") // Add table to include in FROM
    
    // Subquery support
    // Each Subquery statement must be terminated by
    // .EndSubqueryInsideFrom()
    // Otherwise you will end up going deeper and 
    // never come back to root select
    .BeginSubquery("subquery_table_alias")
      // NvSelect Statement implementation inside this block
    .EndSubqueryInsideFrom()

  // End of SQL From Statement Block and return to Select Statement
  .EndFromTableBlock() 
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

// Parameter values
std::string service_code("RS");
auto equipment_types = std::vector<std::string>({"HMT", "HLB"});

// Instancing NvSelect with start parameter from 1
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

### NvSelect Output

A. Parameter Values Output<br/>
```cxx
using ParameterParser =
      nvm::sqlbuilder::PostgresDefaultParameterParser<DefaultPostgresParamType>;
```

B. Sql query string<br/>
If you want to generate unformatted inline sql string use ```GenerateQuery()``` 
with default value of parameter```bool pretty_print = false```.
```sql
SELECT
  e.equipment_id,
  c.company_id,
  s.service_id,
  e.unit_type_id,
  e.unit_class_id,
  e.unit_subclass_id,
  e.reg_id,
  e.code AS equipment_code,
  c.name AS company_name,
  s.name AS service_name,
  ut.code AS unit_type_code,
  ut.name AS unit_type_name,
  uc.code AS unit_class_code,
  uc.name AS unit_class_name,
  us.code AS unit_sub_class_code,
  us.name AS unit_subclass_name,
  e.flags,
  e.status,
  ua.username AS add_username,
  um.username AS mod_username
FROM
  equipment AS e
INNER JOIN
  equipment_type AS ut
  ON
    e.equipment_type_id = ut.equipment_type_id
INNER JOIN
  equipment_class AS uc
  ON
    e.equipment_class_id = uc.equipment_class_id
INNER JOIN
  equipment_sub_class AS us
  ON
    e.equipment_sub_class_id = us.equipment_sub_class_id
INNER JOIN
  services AS s
  ON
    e.service_id = s.service_id
INNER JOIN
  company AS c
  ON
    s.company_id = c.company_id
LEFT JOIN
  users AS ua
  ON
    e.add_by = ua.user_id
LEFT JOIN
  users AS um
  ON
    e.mod_by = um.user_id
INNER JOIN
  ad AS ad
  ON
    e.equipment_id = ad.equipment_id
WHERE
  unit_type_code IN ($1, $2) AND
  s.service_code = $3
ORDER BY
  company_name ASC, service_name ASC, unit_type_code ASC, unit_type_class_code ASC

```

# NvSQL Parameter Values Handling & Conversion
One of benefits using NvSQL is <u>automatic parameter values handling and parameter values ordering with parameter index number</u>.<br/>
The datatype of each parameter is still preserverd, and <br/>
we can take advantage by still getting auto-sanitazion on driver or DB level.<br/>

This because NvSQL (NvSelect, NvInsert, NvUpdate and NvDelete) is designed using template programming approach and <br/>
<b>handles serialization and ordering parameter index and values automatically</b>, <br/>
 so you <u>dont have to keep strugling with duplicate parameters index, wrong parameter index or wrong datatype anymore</u>.  <br/>

### <u>Parameter Values</u>

Parameter values is all datatype must be supported by driver <br/>
when you need to pass parameters to the database.<br/>
Each database or driver having different set of diffrent supported datatype.

To address this is issue, NVSQL use typedef of ```std::variant<supported_type...>``` <br/>
for specific DB implementations.<br/>This type in NvSQL called as ```TParamType```.<br/>

> NvSQL ```TParamType``` : <br/> 
> Definitions of datatype supported natively by the DB Driver use for this datatype.<br/>

NvSQL currently come with ```DefaultPostgresParamType```, so what is this?<br/>
This is TParamType that is exected by all NvSQL template class.<br/>


```cxx
using DefaultPostgresParamType = nvm::sqlbuilder::DefaultPostgresParamType;
```
How NvSQL declare the ```TParamType``` for ```DefaultPostgresParamType```
```cxx
using DefaultPostgresParamType =
    std::variant<int, long long, float, double, std::string, bool,
                 std::chrono::system_clock::time_point, std::vector<int>>;
```
<br/>

> When using different DB Driver or Database <br/>
> you can define new ```TParamType``` or <br/>
> handling and convert it using ```ParameterParser```.


### <u>Parameter Parser</u>
```ParameterParser<TParameterType>``` <br/> is base template for developer that need to overrides the ```Parse()``` and ```GetAllParameterValuesAsString()``` <br/>
to meet requirement from different DB Driver Specific implementation.<br/>

```PostgresDefaultParameterParser<TParameterType>``` <br/>
 is an example of template specialiazation from ```ParameterParser<TParameterType>```.<br/>
 Serves as default parameter type and example supported by PostgresSQL DB Driver when <br/>execute sql query with parameter values.<br/>

 <u>PostgresDefaultParameterParser&lt;TParameterType&gt; template specialization implementation</u>:
```cxx
// Example of a derived class
template <typename TParameterType = DefaultPostgresParamType>
class PostgresDefaultParameterParser : public ParameterParser<TParameterType> {
 public:
  using ParameterParser<TParameterType>::ParameterParser;

 protected:
  std::shared_ptr<void> ParseImplInternal() const override {
    // Implement the actual parsing logic here
    // For simplicity, returning the same parameter values cast to void
    return std::make_shared<std::vector<TParameterType>>(
        *this->parameter_values_);
  }
};

```
Example of using ParameterParser
```cxx
using PgDefParser =
      nvm::sqlbuilder::PostgresDefaultParameterParser<DefaultPostgresParamType>;

NvSelect select();
// Doing Select Builder
// select.

PgDefParser parser(select.Values());

// Print all parameter values as in order
std::cout << "\nPARAMETER VALUES:" << std::endl;
std::cout << parser.GetAllParameterValuesAsString() << std::endl;

// simulate to convert from parameters type to other parameter types
auto parameters_specific_db_driver = Parse<DefaultPostgresParamType>();
  
```

> <b>How To Convert or Transform Parameter Values  std::vector&lt;TParameterType&gt; <br/>For Specific DB Driver Implementations.</b><br/>
>
> <u>Take PQXX as an Example.</u><br/>
>
> PQXX expected different datatype for parameter values when executed sql with parameter values.<br/>
> You have to ```Create Template Specialization from ParameterParser<TParameterType>```<br/> 
> to transform into PQXX expected parameter values datatype.<br/>
>
>Check above template specialization of ```PostgresDefaultParameterParser<DefaultPostgresParamType>``` <br/>above as guideline



# NvValidator [[Top]](#nvcore)
NvValidator is a template base fluent validator that can be chaining in contexable manner and<br/>
has reusable common general case validators.<br/>
For flexibility, NvValidator is also supported custom lambda validation and <br/>
class that are implemented custom logic overloading operators.

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
Object that is implemented logical operator overloading

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
to support complex validations scenario that we are faces day-to-day <br/>
NvValidator have feature of Custom Lambda Validator.

Lambda function signature
```cxx
[](const T& value) noexcept {
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


# Nv DateTime [[Top]](#nvcore)
Nv DateTime is an abstraction of Howard Hinnant Date library.<br/>
We are facing the same painstakingly handling datetime in C++ especially in DateTime with Timezone.<br/>
Nv DateTime is build to supported DateTime with IANA Timezone handling and easy datepart handling or easy date-manipulations.

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

# How to build [[Top]](#nvcore)
Requirement:
1. CMake
2. Clang or GCC C++17 Compiler (currently developed using clang)

Build:

1. Clone main repository and clone the submodules
```sh
git clone --recurse-submodules https://github.com/lwdjohari/nvm-core.git
```

2. Open in your favorite IDE
3. Run CMake configure
4. Build

# Linking NvCore [[Top]](#nvcore)
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

# Contributions [[Top]](#nvcore)

Currently we are still on-going roadmap design and architectural design that might be lead to complete rewrite or complete breaking changes.
We might accept contributors when everything above have better & crytal-clears roadmap.

# License [[Top]](#nvcore)

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
