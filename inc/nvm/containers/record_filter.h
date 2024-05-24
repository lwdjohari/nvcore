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
  EndGroup = 3,
  Subquery = 4
};

// cppcheck-suppress unknownMacro
NVM_ENUM_CLASS_DISPLAY_TRAIT(ConditionMode)

template <typename TParameterType = DefaultPostgresParamType>
class Condition {
 private:
  std::string field_name_;
  std::shared_ptr<std::vector<TParameterType>> values_;
  WhereStatement<TParameterType>* where_subquery_parent_;
  std::shared_ptr<NvSelect<TParameterType>> subquery_;
  SqlOperator operation;
  uint32_t value_size_;
  uint32_t start_index_;
  uint32_t param_index_;
  uint32_t level_;
  LogicOperator logic_operator_;
  ConditionMode mode_;
  std::string table_alias_;

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

  std::string __GenerateQueryFromSubquery(bool pretty_print) const;

 public:
  Condition(std::string field_name, SqlOperator op, uint32_t value_size,
            uint32_t param_index, uint32_t level)
                  : field_name_(std::move(field_name)),
                    values_(nullptr),
                    where_subquery_parent_(nullptr),
                    subquery_(nullptr),
                    operation(op),
                    value_size_(value_size),
                    start_index_(param_index),
                    param_index_(Process(param_index)),
                    level_(level),
                    logic_operator_(),
                    mode_(ConditionMode::Comparator),
                    table_alias_() {}

  Condition(LogicOperator op, ConditionMode mode, uint32_t level)
                  : field_name_(),
                    values_(nullptr),
                    where_subquery_parent_(nullptr),
                    subquery_(nullptr),
                    operation(),
                    value_size_(),
                    start_index_(),
                    param_index_(),
                    level_(level),
                    logic_operator_(op),
                    mode_(mode),
                    table_alias_() {}

  /// @brief Do not use this directly only inner code to instancing if Where
  /// subquery requested
  /// @param parameter_values
  /// @param parent
  /// @param subquery
  /// @param field_name
  /// @param field_alias
  /// @param subquery_name
  /// @param param_index
  /// @param level_
  Condition(std::shared_ptr<std::vector<TParameterType>> parameter_values,
            WhereStatement<TParameterType>* parent,
            const std::string& field_name, const std::string& subquery_name,
            SqlOperator op, uint32_t param_index, uint32_t level)
                  : field_name_(std::move(field_name)),
                    where_subquery_parent_(parent),
                    subquery_(std::make_shared<NvSelect<TParameterType>>(
                        parameter_values, parent, param_index, level + 1,
                        subquery_name)),
                    operation(op),
                    value_size_(),
                    start_index_(),
                    param_index_(),
                    level_(level),
                    logic_operator_(),
                    mode_(ConditionMode::Subquery),
                    table_alias_(subquery_name) {}

  // explicit NvSelect(std::shared_ptr<std::vector<TParameterType>> values,
  //                   uint32_t current_param_index, uint32_t level,
  //                   Where<TParameterType>* where_obj, const std::string&
  //                   table_alias,
  //                   )

  uint32_t StartParameterIndex() const {
    return start_index_;
  }

  uint32_t NextParameterIndex() const {
    return param_index_;
  }

  NvSelect<TParameterType>& Subquery() {
    return *subquery_;
  }

  std::string SubqueryTableAlias() const {
    return table_alias_;
  }

  std::string GenerateQuery(bool pretty_print) const {
    std::ostringstream ss;
    auto index = start_index_;
    if (mode_ == ConditionMode::StartGroup) {
      ss << "(";
    } else if (mode_ == ConditionMode::LogicalOperator) {
      ss << LogicOperatorToString(logic_operator_);
    }

    if (mode_ == ConditionMode::Subquery) {
      ss << (pretty_print ? "\n" + GenerateIndentation(level_) : "")
         << field_name_ << " " << SqlOperatorToString(operation)
         << (pretty_print ? " (\n" : " (");
      ss << __GenerateQueryFromSubquery(pretty_print) << ")"
         << (table_alias_.empty() ? "" : " AS " + table_alias_) << " ";
    } else if (mode_ == ConditionMode::Comparator) {
      ss << (pretty_print ? "\n" + GenerateIndentation(level_) : "")
         << field_name_ << " " << SqlOperatorToString(operation) << " ";
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

template <typename TParameterType>
class WhereStatement {
 private:
  NvSelect<TParameterType>* parent_;
  std::shared_ptr<std::vector<TParameterType>> values_;
  std::vector<Condition<TParameterType>> conditions_;
  uint32_t level_;
  uint32_t current_param_index_;

 public:
  WhereStatement()
                  : parent_(nullptr),
                    values_(std::make_shared<std::vector<TParameterType>>()),
                    conditions_(),
                    level_(),
                    current_param_index_(1) {}

  explicit WhereStatement(
      std::shared_ptr<std::vector<TParameterType>> parameter_values,
      NvSelect<TParameterType>* parent, uint32_t current_param_index,
      uint32_t level)
                  : parent_(parent),
                    values_(parameter_values),
                    conditions_(),
                    level_(level),
                    current_param_index_(current_param_index) {}
  ~WhereStatement() {}

  void UpdateCurrentParameterIndex(uint32_t parameter_index) {
    current_param_index_ = parameter_index;
  }

  int32_t GetCurrentParameterIndex() const {
    return current_param_index_;
  }

  std::shared_ptr<std::vector<TParameterType>> Values() {
    return values_;
  }

  NvSelect<TParameterType>& EndWhereBlock() {
    // std::cout << "EndWhere ParamIndex: " << current_param_index_ <<
    // std::endl;
    parent_->UpdateCurrentParamIndex(current_param_index_);
    return *parent_;
  }

  std::string GenerateQuery(bool pretty_print = false,
                            bool append_where_keyword = true) const {
    std::ostringstream where_clause;
    if (append_where_keyword) {
      where_clause << "WHERE ";
    }

    for (auto& c : conditions_) {
      where_clause << c.GenerateQuery(pretty_print);
    }

    return where_clause.str();
  }

  template <typename T>
  WhereStatement<TParameterType>& AddCondition(const std::string& field_name,
                                               SqlOperator op,
                                               const T& values) {
    conditions_.emplace_back(field_name, op, 1, current_param_index_,
                             level_ + 1);
    const auto cond = conditions_.back();
    current_param_index_ = cond.NextParameterIndex();
    values_->push_back(values);
    return *this;
  }

  template <typename T>
  WhereStatement<TParameterType>& AddConditionBetween(
      const std::string& field_name, const T& value1, const T& value2) {
    values_->push_back(value1);
    values_->push_back(value2);
    conditions_.emplace_back(field_name, SqlOperator::kBetween, 2,
                             current_param_index_, level_ + 1);
    const auto cond = conditions_.back();
    current_param_index_ = cond.NextParameterIndex();
    return *this;
  }

  template <typename T>
  WhereStatement<TParameterType>& AddConditionIn(const std::string& field_name,
                                                 const std::vector<T>& values) {
    conditions_.emplace_back(field_name, SqlOperator::kIn, values.size(),
                             current_param_index_, level_ + 1);
    const auto cond = conditions_.back();
    current_param_index_ = cond.NextParameterIndex();
    for (auto& value : values) {
      values_->push_back(value);
    }

    return *this;
  }

  WhereStatement<TParameterType>& And() {
    conditions_.emplace_back(LogicOperator::kAnd,
                             ConditionMode::LogicalOperator, level_);
    return *this;
  }

  WhereStatement<TParameterType>& Or() {
    conditions_.emplace_back(LogicOperator::kOr, ConditionMode::LogicalOperator,
                             level_);
    return *this;
  }

  WhereStatement<TParameterType>& StartGroup() {
    conditions_.emplace_back(LogicOperator::kOr, ConditionMode::StartGroup,
                             level_);
    return *this;
  }

  WhereStatement<TParameterType>& EndGroup() {
    conditions_.emplace_back(LogicOperator::kOr, ConditionMode::EndGroup,
                             level_);
    return *this;
  }

  NvSelect<TParameterType>& AddSubquery(const std::string& field_name,
                                        SqlOperator op,
                                        const std::string& subquery_name) {
    // Condition(std::shared_ptr<std::vector<TParameterType>> parameter_values,
    //         WhereStatement<TParameterType>* parent,
    //         const std::string& field_name,
    //         const std::string& subquery_name, uint32_t param_index,
    //         uint32_t level)
    conditions_.emplace_back(values_, this, field_name, subquery_name, op,
                             current_param_index_, level_ + 1);
    return conditions_.back().Subquery();
  }
};
}  // namespace nvm::containers