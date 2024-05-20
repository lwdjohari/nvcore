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

enum class ConditionMode {
  Comparator = 0,
  LogicalOperator = 1,
  StartGroup = 2,
  EndGroup = 3
};

NVM_ENUM_CLASS_DISPLAY_TRAIT(ConditionMode)

class Condition {
 private:
  std::string field_name;
  SqlOperator operation;
  uint32_t value_size_;
  uint32_t start_index_;
  uint32_t param_index_;
  uint32_t level_;
  LogicOperator logic_operator_;
  ConditionMode mode_;

  uint32_t Process(uint32_t start_index) {
    uint32_t index = start_index;
    if (operation == SqlOperator::kBetween && value_size_ == 2) {
      index += 2;
    } else if (operation == SqlOperator::kIn) {
      for (size_t i = 0; i < value_size_; ++i) {
        index += 1;
      }
    } else {
      index += 1;
    }
    return index;
  }

 public:
  Condition(std::string field_name, SqlOperator op, uint32_t value_size,
            uint32_t param_index, uint32_t level)
                  : field_name(std::move(field_name)),
                    operation(op),
                    value_size_(value_size),
                    start_index_(param_index),
                    param_index_(Process(param_index)),
                    level_(level),
                    logic_operator_(),
                    mode_(ConditionMode::Comparator) {}

  Condition(LogicOperator op, ConditionMode mode, uint32_t level)
                  : field_name(),
                    operation(),
                    value_size_(),
                    start_index_(),
                    param_index_(),
                    level_(level),
                    logic_operator_(op),
                    mode_(mode) {}

  uint32_t StartParameterIndex() const {
    return start_index_;
  }

  uint32_t NextParameterIndex() const {
    return param_index_;
  }

  std::string GenerateQuery() const {
    std::ostringstream ss;
    auto index = start_index_;
    if (mode_ == ConditionMode::StartGroup) {
      ss << "(";
    } else if (mode_ == ConditionMode::LogicalOperator) {
      ss << LogicOperatorToString(logic_operator_);
    }

    if (mode_ == ConditionMode::Comparator) {
      ss << field_name << " " << SqlOperatorToString(operation) << " ";
      if (operation == SqlOperator::kBetween && value_size_ == 2) {
        ss << "$" << index << " AND $" << (index + 1);
        // index += 2;
      } else if (operation == SqlOperator::kIn) {
        ss << "($";
        for (size_t i = 0; i < value_size_; ++i) {
          ss << index++ << (i < value_size_ - 1 ? ", $" : "");
        }
        ss << ")";
      } else {
        ss << "$" << index++;
      }
    } else if (mode_ == ConditionMode::EndGroup) {
      ss << ")";
    }

    return ss.str();
  }
};

template <typename TParamaterType = DefaultPostgresParamType>
class RecordClause {
 private:
  WhereStatement<TParamaterType>& parent_;
  std::vector<Condition> conditions_;
  //   std::vector<DefaultPostgresParamType> parameters_;
  uint32_t current_param_index_;
  bool is_first_condition_ = true;
  std::shared_ptr<std::vector<TParamaterType>> values_;
  uint32_t level_;

 public:
  explicit RecordClause(
      WhereStatement<TParamaterType>& parent, uint32_t start_param_index,
      std::shared_ptr<std::vector<TParamaterType>> parameter_values,
      uint32_t level)
                  : parent_(parent),
                    current_param_index_(start_param_index),
                    values_(parameter_values),
                    level_(level) {}

  template <typename T>
  RecordClause<TParamaterType>& AddCondition(const std::string& field_name,
                                             SqlOperator op, const T& values) {
    conditions_.emplace_back(field_name, op, 1, current_param_index_, level_);
    const auto cond = conditions_.back();
    current_param_index_ = cond.NextParameterIndex();
    values_->push_back(values);
    return *this;
  }

  template <typename T>
  RecordClause<TParamaterType>& AddConditionBetween(
      const std::string& field_name, const T& value1, const T& value2) {
    values_->push_back(value1);
    values_->push_back(value2);
    conditions_.emplace_back(field_name, SqlOperator::kBetween, 2,
                             current_param_index_, level_);
    const auto cond = conditions_.back();
    current_param_index_ = cond.NextParameterIndex();
    return *this;
  }

  template <typename T>
  RecordClause<TParamaterType>& AddConditionIn(const std::string& field_name,
                                               const std::vector<T>& values) {
    conditions_.emplace_back(field_name, SqlOperator::kIn, values.size(),
                             current_param_index_, level_);
    const auto cond = conditions_.back();
    current_param_index_ = cond.NextParameterIndex();
    for (auto& value : values) {
      values_->push_back(value);
    }

    return *this;
  }

  RecordClause<TParamaterType>& And() {
    conditions_.emplace_back(LogicOperator::kAnd,
                             ConditionMode::LogicalOperator, level_);
    return *this;
  }

  RecordClause<TParamaterType>& Or() {
    conditions_.emplace_back(LogicOperator::kOr, ConditionMode::LogicalOperator,
                             level_);
    return *this;
  }

  RecordClause<TParamaterType>& StartGroup() {
    conditions_.emplace_back(LogicOperator::kOr, ConditionMode::StartGroup,
                             level_);
    return *this;
  }

  RecordClause<TParamaterType>& EndGroup() {
    conditions_.emplace_back(LogicOperator::kOr, ConditionMode::EndGroup,
                             level_);
    return *this;
  }

  std::string GenerateQuery(bool pretty_print = false) const {
    std::ostringstream query;
    for (auto& c : conditions_) {
      query << c.GenerateQuery();
    }

    return query.str();
  }

  WhereStatement<TParamaterType>& EndClauseBlock() {
    // should set the parent reflecting the current parameter index and
    // propagating until root
    // no recursive all pointing by reference values
    return parent_;
  }

  //   const std::vector<DefaultPostgresParamType>& GetParameters() const {
  //   return parameters_;
  //   }

  uint32_t GetCurrentParamIndex() const {
    return current_param_index_;
  }
};

template <typename TParamaterType>
class WhereStatement {
 private:
  NvSelect<TParamaterType>* parent_;
  std::shared_ptr<std::vector<TParamaterType>> values_;
  std::shared_ptr<RecordClause<TParamaterType>> clause_;
  uint32_t level_;

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
      if (i < vec.size() - 1)
        oss << ", ";
    }
    oss << "]";
  }

  static void AppendVector(std::ostringstream& oss,
                           const std::vector<bool>& vec) {
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
      oss << (vec[i] ? "true" : "false");
      if (i < vec.size() - 1)
        oss << ", ";
    }
    oss << "]";
  }

 public:
  // WhereStatement<TParamaterType>& parent, uint32_t start_param_index,
  //     std::shared_ptr<std::vector<TParamaterType>> parameter_values,
  //     uint32_t level)
  //     : parent_(parent),
  //       current_param_index_(start_param_index),
  //       values_(parameter_values),
  //       level_(level) {}

  WhereStatement()
                  : parent_(nullptr),
                    values_(std::make_shared<std::vector<TParamaterType>>()),
                    clause_(std::make_shared<RecordClause<TParamaterType>>(
                        *this, 1, values_, 0)),
                    level_() {}

  explicit WhereStatement(NvSelect<TParamaterType>* parent,
                          uint32_t current_param_index, uint32_t level)
                  : parent_(parent),
                    values_(std::make_shared<std::vector<TParamaterType>>()),
                    clause_(std::make_shared<RecordClause<TParamaterType>>(
                        *this, current_param_index, values_, level_)),
                    level_(level) {}
  ~WhereStatement() {}

  std::shared_ptr<std::vector<TParamaterType>> Values() {
    return values_;
  }

  RecordClause<TParamaterType>& BeginClause() {
    return *clause_;
  };

  NvSelect<TParamaterType>& EndWhereBlock() {
    return *parent_;
  }

  std::string GenerateQuery(bool pretty_print = false,
                            bool append_where_keyword = true) const {
    std::ostringstream where_clause;
    if (append_where_keyword) {
      where_clause << "WHERE ";
    }

    where_clause << clause_->GenerateQuery(pretty_print);
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