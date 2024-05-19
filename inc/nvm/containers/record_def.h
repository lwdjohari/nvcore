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

#include <cstdint>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

namespace nvm::containers {

using DefaultPostgresParamType =
    std::variant<int, long long, float, double, std::string, bool,
                 std::chrono::system_clock::time_point, std::vector<int>>;

// using DefaultPostgresParamType =
//     std::variant<int, long long, float, double, std::string, bool,
//                  std::chrono::system_clock::time_point, std::vector<int>,
//                  std::vector<long long>, std::vector<float>,
//                  std::vector<double>, std::vector<std::string>,
//                  std::vector<bool>,
//                  std::vector<std::chrono::system_clock::time_point>>;

struct RecordTable {
  std::string name;
  std::optional<std::string> alias;

  explicit RecordTable(const std::string& name,
                       std::optional<std::string> alias = std::nullopt)
      : name(std::string(name)), alias(alias) {}
};

enum class FieldPinMode : uint16_t { None = 0, Beginning = 1, End = 2 };

// cppcheck-suppress unknownMacro
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
  if (level == 0) return std::string();
  int32_t n = number_per_print * level;
  std::ostringstream oss;
  for (int i = 0; i < n; ++i) {
    oss << indent_char;
  }
  return oss.str();
}

}  // namespace nvm::containers

#endif