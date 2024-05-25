/*
 *  Copyright (c) 2024 Linggawasistha Djohari
 * <linggawasistha.djohari@outlook.com> Licensed to Linggawasistha Djohari under
 * one or more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 *
 *  Linggawasistha Djohari licenses this file to you under the Apache License,
 *  Version 2.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef NVM_CORE_CONTAINERS_V2_RECORD_DEF_H
#define NVM_CORE_CONTAINERS_V2_RECORD_DEF_H

#include <nvm/macro.h>

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace nvm::sqlbuilder {

// Define DefaultPostgresParamType to support comprehensive PostgreSQL
// data types
using DefaultPostgresParamType =
    std::variant<int, long long, float, double, std::string, bool,
                 std::chrono::system_clock::time_point, std::vector<int>>;

// Define DefaultOracleParamType to support comprehensive Oracle data types
using DefaultOracleParamType = std::variant<
    int,          // Oracle NUMBER, INTEGER
    long long,    // Oracle NUMBER (large integer)
    float,        // Oracle FLOAT
    double,       // Oracle DOUBLE PRECISION
    std::string,  // Oracle VARCHAR2, CHAR, CLOB, NCHAR, NVARCHAR2, NCLOB,
                  // BFILE, XMLType, ROWID, UROWID
    bool,         // Oracle BOOLEAN
    std::chrono::system_clock::time_point,  // Oracle DATE, TIMESTAMP, TIMESTAMP
                                            // WITH TIME ZONE, TIMESTAMP WITH
                                            // LOCAL TIME ZONE
    std::vector<unsigned char>              // Oracle RAW, BLOB
    >;

// using DefaultPostgresParamType =
//     std::variant<int, long long, float, double, std::string, bool,
//                  std::chrono::system_clock::time_point, std::vector<int>,
//                  std::vector<long long>, std::vector<float>,
//                  std::vector<double>, std::vector<std::string>,
//                  std::vector<bool>,
//                  std::vector<std::chrono::system_clock::time_point>>;

enum class DatabaseDialect { PostgreSQL = 1, Oracle = 2 };

// cppcheck-suppress unknownMacro
NVM_ENUM_CLASS_DISPLAY_TRAIT(DatabaseDialect)

struct RecordTable {
  std::string name;
  std::optional<std::string> alias;

  explicit RecordTable(const std::string& name,
                       std::optional<std::string> alias = std::nullopt)
                  : name(std::string(name)), alias(alias) {}
};

enum class FieldPinMode : uint16_t { None = 0, Beginning = 1, End = 2 };

NVM_ENUM_CLASS_DISPLAY_TRAIT(FieldPinMode)

enum class SortType : uint16_t { Ascending = 0, Descending = 1 };

NVM_ENUM_CLASS_DISPLAY_TRAIT(SortType)

enum class SqlOperator {
  kEqual,
  kNotEqual,
  kLess,
  kLessOrEqual,
  kGreater,
  kGreaterOrEqual,
  kLike,
  kBetween,
  kIn
};

NVM_ENUM_CLASS_DISPLAY_TRAIT(SqlOperator)

enum class LogicOperator { kAnd, kOr };

NVM_ENUM_CLASS_DISPLAY_TRAIT(LogicOperator)

enum class SqlAggregateFunction {
  None = 0,
  Distinct = 1,
  Count = 2,
  Avg = 3,
  Sum = 4,
  ToUpper = 5,
  ToLower = 6,
  ToIso8601DateTime = 7,
  ToIso8601Date = 8,
  ToIso8601Time = 9
};

NVM_ENUM_CLASS_DISPLAY_TRAIT(SqlAggregateFunction)

enum class SqlJoinType {
  None = 0,
  InnerJoin = 1,
  LeftJoin = 2,
  RightJoin = 4,
};

NVM_ENUM_CLASS_DISPLAY_TRAIT(SqlJoinType)

enum class JoinDefMode {
  RecordKeyBoth = 0,
  SubquerySelectString = 1,
  SubqueryRawString = 2,
  SubquerySelectObject = 3
};

NVM_ENUM_CLASS_DISPLAY_TRAIT(JoinDefMode)

// Forward declaration WhereStatement
template <typename TParameterType = DefaultPostgresParamType>
class WhereStatement;

// Forward declaration Order By
template <typename TParameterType = DefaultPostgresParamType>
class OrderByStatement;

// Forward declaration Group By
template <typename TParameterType = DefaultPostgresParamType>
class GroupByStatement;

// Forward declaration HavingByStatement
template <typename TParameterType = DefaultPostgresParamType>
class HavingByStatement;

// Forward declaration for NvSelect
template <typename TParameterType = DefaultPostgresParamType>
class NvSelect;

inline std::string DetermineParameterFormat(const DatabaseDialect& dialect,
                                            const uint32_t parameter_index) {
  std::ostringstream ss;
  switch (dialect) {
    case DatabaseDialect::PostgreSQL:
      ss << "$" << parameter_index;
      break;
    case DatabaseDialect::Oracle:
      ss << ":" << parameter_index;
      break;
    default:
      break;
  }

  return ss.str();
}

inline std::string SqlOperatorToString(SqlOperator op) {
  switch (op) {
    case SqlOperator::kEqual:
      return "=";
    case SqlOperator::kNotEqual:
      return "!=";
    case SqlOperator::kLess:
      return "<";
    case SqlOperator::kLessOrEqual:
      return "<=";
    case SqlOperator::kGreater:
      return ">";
    case SqlOperator::kGreaterOrEqual:
      return ">=";
    case SqlOperator::kLike:
      return "LIKE";
    case SqlOperator::kBetween:
      return "BETWEEN";
    case SqlOperator::kIn:
      return "IN";
    default:
      return "";
  }
}

inline std::string LogicOperatorToString(LogicOperator logic) {
  switch (logic) {
    case LogicOperator::kAnd:
      return " AND ";
    case LogicOperator::kOr:
      return " OR ";
    default:
      return "";
  }
}

inline std::string GenerateIndentation(uint32_t level, char indent_char = ' ',
                                       uint32_t number_per_print = 2) {
  if (level == 0)
    return std::string();
  int32_t n = number_per_print * level;
  std::ostringstream oss;
  for (int i = 0; i < n; ++i) {
    oss << indent_char;
  }
  return oss.str();
}

template <typename TParameterType = DefaultPostgresParamType>
class ParameterParser {
 public:
  explicit ParameterParser(
      std::shared_ptr<std::vector<TParameterType>> parameter_values)
                  : parameter_values_(parameter_values) {}

  virtual ~ParameterParser() = default;

  template <typename TParameterDestType>
  std::shared_ptr<std::vector<TParameterDestType>> Parse() {
    return ParseImpl<TParameterDestType>();
  }

  virtual std::string GetAllParameterValuesAsString() const {
    std::ostringstream oss;
    for (const auto& value : *parameter_values_) {
      std::visit([this, &oss](const auto& val) { AppendValue(oss, val); },
                 value);
      oss << std::endl;
    }
    return oss.str();
  }

 protected:
  std::shared_ptr<std::vector<TParameterType>> parameter_values_;

  virtual std::shared_ptr<void> ParseImplInternal() const = 0;

 private:
  template <typename TParameterDestType>
  std::shared_ptr<std::vector<TParameterDestType>> ParseImpl() const {
    return std::static_pointer_cast<std::vector<TParameterDestType>>(
        ParseImplInternal());
  }

  template <typename T>
  void AppendValue(std::ostringstream& oss, const T& value) const {
    if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>) {
      std::time_t time = std::chrono::system_clock::to_time_t(value);
      oss << std::put_time(std::localtime(&time), "%F %T");
    } else if constexpr (is_vector<T>::value) {
      AppendVector(oss, value);
    } else {
      oss << value;
    }
  }

  template <typename T>
  void AppendVectorValue(std::ostringstream& oss, const T& value) const {
    if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>) {
      std::time_t time = std::chrono::system_clock::to_time_t(value);
      oss << std::put_time(std::localtime(&time), "%F %T");
    } else {
      oss << value;
    }
  }

  template <typename T>
  void AppendVector(std::ostringstream& oss, const std::vector<T>& vec) const {
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
      AppendVectorValue(oss, vec[i]);
      if (i < vec.size() - 1) {
        oss << ", ";
      }
    }
    oss << "]";
  }

  void AppendVector(std::ostringstream& oss,
                    const std::vector<bool>& vec) const {
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
      oss << (vec[i] ? "true" : "false");
      if (i < vec.size() - 1) {
        oss << ", ";
      }
    }
    oss << "]";
  }

  // Helper trait to check if a type is a vector
  template <typename T>
  struct is_vector : std::false_type {};

  template <typename T>
  struct is_vector<std::vector<T>> : std::true_type {};
};

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

}  // namespace nvm::sqlbuilder

#endif