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

#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "nvm/containers/record_def.h"

namespace nvm::containers {
    
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
enum class LogicOperator { kAnd, kOr };

std::string SqlOperatorToString(SqlOperator op) {
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

std::string LogicOperatorToString(LogicOperator logic) {
  switch (logic) {
    case LogicOperator::kAnd:
      return " AND ";
    case LogicOperator::kOr:
      return " OR ";
    default:
      return "";
  }
}

template <typename T>
class Condition {
 public:
  std::string field_name;
  SqlOperator operation;
  uint32_t value_size_;
  Condition(std::string field_name, SqlOperator op, uint32_t value_size = 1)
      : field_name(std::move(field_name)),
        operation(op),
        value_size_(value_size) {}

  std::string ToSql(uint32_t& start_param_index) const {
    std::ostringstream ss;
    ss << field_name << " " << SqlOperatorToString(operation) << " ";

    if (operation == SqlOperator::kBetween && value_size_ == 2) {
      ss << "$" << start_param_index << " AND $" << (start_param_index + 1);
      start_param_index += 2;
    } else if (operation == SqlOperator::kIn) {
      ss << "($";
      for (size_t i = 0; i < value_size_; ++i) {
        ss << start_param_index++ << (i < value_size_ - 1 ? ", $" : "");
      }
      ss << ")";
    } else {
      ss << "$" << start_param_index++;
    }
    return ss.str();
  }
};

template <typename TParamaterType = DefaultPostgresParamType>
class RecordClause {
 private:
  std::ostringstream clause_;
  //   std::vector<DefaultPostgresParamType> parameters_;
  uint32_t current_param_index_;
  bool is_first_condition_ = true;
  std::shared_ptr<std::vector<TParamaterType>> values_;

 public:
  explicit RecordClause(
      uint32_t start_param_index,
      std::shared_ptr<std::vector<TParamaterType>> parameter_values)
      : current_param_index_(start_param_index), values_(parameter_values) {}

  template <typename T>
  RecordClause<TParamaterType>& AddCondition(const std::string& field_name,
                                             SqlOperator op, const T& values) {
    clause_ << Condition<T>(field_name, op, 1).ToSql(current_param_index_);
    values_->push_back(values);
    return *this;
  }

  template <typename T>
  RecordClause<TParamaterType>& AddConditionBetween(
      const std::string& field_name, const T& value1, const T& value2) {
    values_->push_back(value1);
    values_->push_back(value2);
    clause_ << Condition<T>(field_name, SqlOperator::kBetween, 2)
                   .ToSql(current_param_index_);

    return *this;
  }

  template <typename T>
  RecordClause<TParamaterType>& AddConditionIn(const std::string& field_name,
                                               const std::vector<T>& values) {
    clause_ << Condition<T>(field_name, SqlOperator::kIn, values.size())
                   .ToSql(current_param_index_);
    for (auto& value : values) {
      values_->push_back(value);
    }

    return *this;
  }

  RecordClause<TParamaterType>& And() {
    clause_ << " AND ";
    return *this;
  }

  RecordClause<TParamaterType>& Or() {
    clause_ << " OR ";
    return *this;
  }

  RecordClause<TParamaterType>& StartGroup() {
    clause_ << "(";
    return *this;
  }

  RecordClause<TParamaterType>& EndGroup() {
    clause_ << ")";
    return *this;
  }

  std::string ToString() const { return clause_.str(); }

  //   const std::vector<DefaultPostgresParamType>& GetParameters() const { return parameters_;
  //   }

  uint32_t GetCurrentParamIndex() const { return current_param_index_; }
};

template <typename TParamaterType = DefaultPostgresParamType>
class RecordWhere {
 private:
  std::vector<std::unique_ptr<RecordClause<TParamaterType>>> clauses_;
  std::shared_ptr<std::vector<TParamaterType>> values_;

  template <typename T>
  static void AppendValue(std::ostringstream& oss, const T& value) {
    if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>) {
      std::time_t time = std::chrono::system_clock::to_time_t(value);
      oss << std::put_time(std::localtime(&time), "%F %T");
    } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                         std::is_same_v<T, std::vector<long long>> ||
                         std::is_same_v<T, std::vector<float>> ||
                         std::is_same_v<T, std::vector<double>> ||
                         std::is_same_v<T, std::vector<std::string>> ||
                         std::is_same_v<T, std::vector<bool>> ||
                         std::is_same_v<
                             T, std::vector<
                                    std::chrono::system_clock::time_point>>) {
      AppendVector(oss, value);
    } else {
      oss << value;
    }
  }

  template <typename T>
  static void AppendVectorValue(std::ostringstream& oss, const T& value) {
    if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>) {
      std::time_t time = std::chrono::system_clock::to_time_t(value);
      oss << std::put_time(std::localtime(&time), "%F %T");
    } else {
      oss << value;
    }
  }

  template <typename T>
  static void AppendVector(std::ostringstream& oss, const std::vector<T>& vec) {
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
      AppendVectorValue(oss, vec[i]);
      if (i < vec.size() - 1) oss << ", ";
    }
    oss << "]";
  }

  static void AppendVector(std::ostringstream& oss,
                           const std::vector<bool>& vec) {
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
      oss << (vec[i] ? "true" : "false");
      if (i < vec.size() - 1) oss << ", ";
    }
    oss << "]";
  }

 public:
  RecordWhere()
      : clauses_(), values_(std::make_shared<std::vector<TParamaterType>>()) {}
  ~RecordWhere() {}

  RecordClause<TParamaterType>& AddClause(uint32_t start_param_index = 1) {
    clauses_.emplace_back(std::make_unique<RecordClause<TParamaterType>>(
        start_param_index, values_));
    return *clauses_.back();
  }

  std::shared_ptr<std::vector<TParamaterType>> Values() { return values_; }

  std::string GenerateWhereClause(bool append_where_keyword = true) const {
    std::ostringstream where_clause;
    if (append_where_keyword) {
      where_clause << "WHERE ";
    }

    for (const auto& clause : clauses_) {
      where_clause << clause->ToString();
    }
    
    return where_clause.str();
  }

  std::string GetAllParameterValuesAsString() const {
    std::ostringstream oss;
    for (const auto& value : *values_) {
      std::visit([&oss](const auto& val) { AppendValue(oss, val); }, value);
      oss << std::endl;
    }
    return oss.str();
  }

  //   std::vector<DefaultPostgresParamType> GetAllParameters() const {
  //     std::vector<DefaultPostgresParamType> all_parameters;
  //     for (const auto& clause : clauses_) {
  //       const auto& params = clause->GetParameters();
  //       all_parameters.insert(all_parameters.end(), params.begin(),
  //       params.end());
  //     }
  //     return all_parameters;
  //   }
};
}  // namespace nvm::containers