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

#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "nvm/sqlbuilder/def.h"
#include "nvm/sqlbuilder/group_by.h"
#include "nvm/sqlbuilder/order_by.h"
#include "nvm/sqlbuilder/where.h"
namespace nvm::sqlbuilder {

struct RecordKey {
  std::string table;
  std::string field;
  std::optional<std::string> table_alias;
  bool initialize;

  RecordKey()
                  : table(),
                    field(),
                    table_alias(std::nullopt),
                    initialize(false) {}

  explicit RecordKey(const std::string& table, const std::string& field,
                     const std::optional<std::string>& alias = std::nullopt)
                  : table(std::string(table)),
                    field(std::string(field)),
                    table_alias(std::optional<std::string>(alias)),
                    initialize(true) {}

  std::string BuildField() const {
    return table_alias.has_value() ? table_alias.value() + "." + field
                                   : table + "." + field;
  }

  std::string BuildTableName() const {
    return table_alias.has_value() ? table + " AS " + table_alias.value()
                                   : table;
  }
};

template <typename TParameterType = DefaultPostgresParamType>
class JoinDef {
 public:
  JoinDef(RecordKey&& left_table, RecordKey&& right_table, SqlJoinType join,
          uint32_t level, DatabaseDialect dialect)
                  : subquery_str_(),
                    subsquery_str_alias_(),
                    subquery_field_key_(),
                    subquery_obj_(),
                    left_table_(std::forward<RecordKey>(left_table)),
                    right_table_(std::forward<RecordKey>(right_table)),
                    join_type_(join),
                    join_mode_(JoinDefMode::RecordKeyBoth),
                    level_(level),
                    dialect_(dialect) {}

  JoinDef(RecordKey&& left_table, SqlJoinType join, const std::string& subquery,
          const std::string& subquery_field_key,
          const std::string& subquery_table_alias, SqlOperator op,
          uint32_t level, DatabaseDialect dialect)
                  : subquery_str_(std::string(subquery)),
                    subsquery_str_alias_(std::string(subquery_table_alias)),
                    subquery_field_key_(std::string(subquery_field_key)),
                    subquery_obj_(),
                    left_table_(std::forward<RecordKey>(left_table)),
                    right_table_(RecordKey()),
                    join_type_(join),
                    join_mode_(JoinDefMode::SubquerySelectString),
                    level_(level),
                    dialect_(dialect) {}

  JoinDef(RecordKey&& existing_table, NvSelect<TParameterType>&& subquery,
          SqlJoinType join, uint32_t level, DatabaseDialect dialect)
                  : subquery_str_(),
                    subsquery_str_alias_(),
                    subquery_field_key_(),
                    subquery_obj_(std::make_shared<NvSelect<TParameterType>>(
                        std::forward<NvSelect<TParameterType>>(subquery))),
                    left_table_(std::forward<RecordKey>(existing_table)),
                    right_table_(RecordKey()),
                    join_type_(join),
                    join_mode_(JoinDefMode::SubquerySelectObject),
                    level_(level),
                    dialect_(dialect) {}

  SqlJoinType JoinType() const {
    return join_type_;
  }

  JoinDefMode Mode() const {
    return join_mode_;
  }

  const RecordKey& LeftTable() const {
    return left_table_;
  }

  const RecordKey& RightTable() const {
    return right_table_;
  }

  NvSelect<TParameterType>& Subquery() {
    return *subquery_obj_;
  }

  bool IsHasSubqueryObject() {
    return subquery_obj_ != nullptr;
  }

  const std::string& SubqueryString() const {
    return subquery_str_;
  }

  const std::string& SubqueryAliasString() const {
    return subsquery_str_alias_;
  }

  std::string GenerateQuery(bool pretty_print = false) const {
    switch (join_mode_) {
      case JoinDefMode::RecordKeyBoth:
        return GenerateJoinRecordBoth(pretty_print);
        break;
      case JoinDefMode::SubquerySelectString:
        return GenerateJoinRecordSubqueryString();
        break;
      case JoinDefMode::SubquerySelectObject:
        return GenerateJoinRecordSubqueryObject();
        break;
      default:
        break;
    }

    return std::string();
  }

 private:
  std::string subquery_str_;
  std::string subsquery_str_alias_;
  std::string subquery_field_key_;
  mutable std::shared_ptr<NvSelect<TParameterType>> subquery_obj_;
  RecordKey left_table_;
  RecordKey right_table_;
  SqlJoinType join_type_;
  JoinDefMode join_mode_;
  SqlOperator sql_operator_;
  uint32_t level_;
  DatabaseDialect dialect_;

  std::string GenerateJoinRecordBoth(bool pretty_print) const {
    if (join_type_ == SqlJoinType::InnerJoin) {
      return GenerateInnerJoin(left_table_, right_table_, pretty_print);
    } else if (join_type_ == SqlJoinType::LeftJoin) {
      return GenerateLeftJoin(left_table_, right_table_, pretty_print);
    } else if (join_type_ == SqlJoinType::RightJoin) {
      return GenerateRightJoin(left_table_, right_table_);
    }

    return std::string();
  }

  std::string GenerateJoinRecordSubqueryString() const {
    if (join_type_ == SqlJoinType::InnerJoin) {
      return GenerateInnerJoin(left_table_, subquery_str_, subquery_field_key_,
                               subsquery_str_alias_, sql_operator_);
    } else if (join_type_ == SqlJoinType::LeftJoin) {
      return GenerateLeftJoin(left_table_, subquery_str_, subquery_field_key_,
                              subsquery_str_alias_, sql_operator_);
    } else if (join_type_ == SqlJoinType::RightJoin) {
      return GenerateRightJoin(left_table_, subquery_str_, subquery_field_key_,
                               subsquery_str_alias_, sql_operator_);
    }

    return std::string();
  }

  std::string GenerateJoinRecordSubqueryObject() const;

  std::string GenerateLeftJoin(const RecordKey& left_key,
                               const RecordKey& right_key,
                               bool pretty_print) const {
    std::ostringstream join;
    // clang-format off
    join << 
        (pretty_print? GenerateIndentation(level_) : "") << 
        (pretty_print? "LEFT JOIN\n" : "LEFT JOIN ") << 
        (pretty_print? GenerateIndentation(level_+1) : "") <<
        right_key.BuildTableName() <<  
        (pretty_print? "\n" + GenerateIndentation(level_+1) + "ON\n" : " ON ") << 
        (pretty_print? GenerateIndentation(level_+2) : "") << 
        left_key.BuildField() << " = " << right_key.BuildField();
    // clang-format on

    return join.str();
  }

  std::string GenerateLeftJoin(const RecordKey& right_table,
                               const std::string& left_table,
                               const std::string& left_table_field_key,
                               const std::string& left_table_alias,
                               SqlOperator op = SqlOperator::kEqual) const {
    std::ostringstream join;

    // clang-format off
    join << 
        "LEFT JOIN " << 
        "(" << left_table << ")" << 
         (!left_table_alias.empty()? 
          " AS " + left_table_alias : "") << 
        " ON " << 
        (!left_table_alias.empty()? 
        left_table_alias + "." + left_table_field_key  : left_table_field_key) <<
        SqlOperatorToString(op) <<
        right_table.BuildField();
    // clang-format on

    return join.str();
  }

  std::string GenerateRightJoin(const RecordKey& left_table,
                                const RecordKey& right_table) const {
    std::ostringstream join;
    // clang-format off
    join << 
        "RIGHT JOIN " << 
        right_table.BuildTableName() << 
        " ON " << 
        left_table.BuildField() << " = " << right_table.BuildField();
    // clang-format on

    return join.str();
  }

  std::string GenerateRightJoin(const RecordKey& left_table,
                                const std::string& right_table,
                                const std::string& right_table_field_key,
                                const std::string& right_table_alias,
                                SqlOperator op = SqlOperator::kEqual) const {
    std::ostringstream join;
    // clang-format off
    join << 
        "RIGHT JOIN " << 
        "(" << right_table << ")" << 
         (!right_table_alias.empty()? 
          " AS " + right_table_alias : "") << 
        " ON " << 
        left_table.BuildField() <<
        SqlOperatorToString(op) << 
        (!right_table_alias.empty()? 
          right_table_alias + "." + right_table_field_key : right_table_field_key);

    // clang-format on

    return join.str();
  }

  std::string GenerateInnerJoin(const RecordKey& existing_select,
                                const RecordKey& join_on_table,
                                bool pretty_print) const {
    std::ostringstream join;
    // clang-format off
    join << 
        (pretty_print? GenerateIndentation(level_) : "") << 
        (pretty_print? "INNER JOIN\n" : "INNER JOIN ") << 
        (pretty_print? GenerateIndentation(level_+1) : "") <<
        join_on_table.BuildTableName() <<  
        (pretty_print? "\n" + GenerateIndentation(level_+1) + "ON\n" : " ON ") << 
        (pretty_print? GenerateIndentation(level_+2) : "") << 
        existing_select.BuildField() << " = " << join_on_table.BuildField();
    // clang-format on

    return join.str();
  }

  std::string GenerateInnerJoin(const RecordKey& existing_select,
                                const std::string& join_on_table,
                                const std::string& join_table_field_key,
                                const std::string& join_table_alias,
                                SqlOperator op = SqlOperator::kEqual) const {
    std::ostringstream join;
    // clang-format off
    join << 
        "INNER JOIN " << 
        "(" << join_on_table << ")" << 
         (!join_table_alias.empty()? 
          " AS " + join_table_alias : "") << 
        " ON " << 
        existing_select.BuildField() <<
        SqlOperatorToString(op) << 
        (!join_table_alias.empty()? 
          join_table_alias + "." +  join_table_field_key : join_table_field_key);
    // clang-format on
    return join.str();
  }
};

template <typename TParameterType = DefaultPostgresParamType>
class JoinStatement {
 private:
  NvSelect<TParameterType>& parent_;
  std::vector<JoinDef<TParameterType>> joins_;
  // std::unique_ptr<NvSelect<TParameterType>> subquery_;
  uint32_t current_parameter_index_;
  uint32_t level_;
  DatabaseDialect dialect_;

 public:
  explicit JoinStatement(NvSelect<TParameterType>& parent,
                         uint32_t parameter_index, uint32_t level,
                         DatabaseDialect dialect)
                  : parent_(parent),
                    joins_(),
                    // subquery_(nullptr),
                    current_parameter_index_(parameter_index),
                    level_(level),
                    dialect_(dialect) {}

  NvSelect<TParameterType>& EndJoinBlock() {
    // sync the current_parameter
    parent_.UpdateCurrentParamIndex(current_parameter_index_);
    return parent_;
  };

  std::string __GenerateSelectBlock(const NvSelect<TParameterType>& select);

  const std::vector<JoinDef<TParameterType>>& GetJoinClauses() const {
    return joins_;
  }

  bool Empty() const {
    return joins_.empty();
  }

  std::string GenerateQuery(bool prety_print = false) const {
    std::ostringstream query;
    bool is_first_element = true;
    for (const auto& clause : joins_) {
      if (!is_first_element)
        query << (prety_print ? "\n" : " ");
      query << clause.GenerateQuery(prety_print);
      is_first_element = false;
    }
    return query.str();
  }

  /// @brief Construct LEFT JOIN Statement
  /// @param left_table
  /// @param right_table
  /// @return
  JoinStatement& LeftJoin(RecordKey&& left_table, RecordKey&& right_table) {
    joins_.emplace_back(std::forward<RecordKey>(left_table),
                        std::forward<RecordKey>(right_table),
                        SqlJoinType::LeftJoin, level_, dialect_);
    return *this;
  }

  /// @brief Construct LEFT Join by query string
  /// @param right_table
  /// @param left_table
  /// @param left_table_field_key
  /// @param left_table_alias
  /// @param op
  /// @return
  JoinStatement& LeftJoin(RecordKey&& right_table,
                          const std::string& left_table,
                          const std::string& left_table_field_key,
                          const std::string& left_table_alias,
                          SqlOperator op = SqlOperator::kEqual) {
    // Const format
    // RecordKey&& left_table, SqlJoinType join,
    //       const std::string& subquery,
    //       const std::string& subquery_field_key,
    //       const std::optional<std::string>& subquery_table_alias,
    //       SqlOperator op = SqlOperator::kEqual

    joins_.emplace_back(std::forward<RecordKey>(right_table),
                        SqlJoinType::LeftJoin, left_table, left_table_field_key,
                        left_table_alias, op, level_, dialect_);
    return *this;
  }

  /// @brief Construct RIGHT JOIN statement
  /// @param left_table
  /// @param right_table
  /// @return
  JoinStatement& RightJoin(RecordKey&& left_table, RecordKey&& right_table) {
    joins_.emplace_back(std::forward<RecordKey>(left_table),
                        std::forward<RecordKey>(right_table),
                        SqlJoinType::RightJoin, level_);
    return *this;
  }

  /// @brief Construct RIGHT JOIN statement from query string
  /// @param left_table
  /// @param right_table
  /// @param right_table_field_key
  /// @param right_table_alias
  /// @param op
  /// @return
  JoinStatement& RightJoin(RecordKey&& left_table,
                           const std::string& right_table,
                           const std::string& right_table_field_key,
                           const std::optional<std::string>& right_table_alias,
                           SqlOperator op = SqlOperator::kEqual) {
    // Const format
    // RecordKey&& left_table, SqlJoinType join,
    //       const std::string& subquery,
    //       const std::string& subquery_field_key,
    //       const std::optional<std::string>& subquery_table_alias,
    //       SqlOperator op = SqlOperator::kEqual

    joins_.emplace_back(
        std::forward<RecordKey>(left_table), SqlJoinType::LeftJoin, right_table,
        right_table_field_key, right_table_alias, op, level_, dialect_);

    return *this;
  }

  /// @brief Construct INNER JOIN statement
  /// @param existing_select
  /// @param join_on_table
  /// @return
  JoinStatement& InnerJoin(RecordKey&& existing_select,
                           RecordKey&& join_on_table) {
    joins_.emplace_back(std::forward<RecordKey>(existing_select),
                        std::forward<RecordKey>(join_on_table),
                        SqlJoinType::InnerJoin, level_, dialect_);
    return *this;
  }

  /// @brief Construct INNER JOIN from query string
  /// @param existing_select
  /// @param join_on_table
  /// @param join_table_field_key
  /// @param join_table_alias
  /// @param op
  /// @return
  JoinStatement& InnerJoin(RecordKey& existing_select,
                           const std::string& join_on_table,
                           const std::string& join_table_field_key,
                           const std::optional<std::string>& join_table_alias,
                           SqlOperator op = SqlOperator::kEqual) {
    // Const format
    // RecordKey&& left_table, SqlJoinType join,
    //       const std::string& subquery,
    //       const std::string& subquery_field_key,
    //       const std::optional<std::string>& subquery_table_alias,
    //       SqlOperator op = SqlOperator::kEqual

    joins_.emplace_back(std::forward<RecordKey>(existing_select),
                        SqlJoinType::LeftJoin, join_on_table,
                        join_table_field_key, join_table_alias, op, level_,
                        dialect_);
    return *this;
  }

  // NvSelect<TParameterType>& JoinWithSubquery(SqlJoinType join_type) {
  //   if (!subquery_) {
  //     subquery_ = std::make_shared<NvSelect<TParameterType>>(
  //         parent_.GetCurrentParamIndex());
  //   }

  //   return subquery_;
  // }
};

struct FromTable {
  std::string table;
  std::optional<std::string> table_alias;

  FromTable() : table(), table_alias() {}

  explicit FromTable(const std::string& table,
                     const std::optional<std::string>& alias = std::nullopt)
                  : table(table), table_alias(alias) {}

  std::string BuildTableName() const {
    return table_alias.has_value() ? table + " AS " + table_alias.value()
                                   : table;
  }
};

template <typename TParameterType = DefaultPostgresParamType>
class FromTableStatement {
 private:
  NvSelect<TParameterType>* parent_;
  std::vector<FromTable> tables_;
  std::vector<NvSelect<TParameterType>> subqueries_;
  std::shared_ptr<std::vector<TParameterType>> parameter_values_;
  uint32_t level_;
  uint32_t current_parameter_index_;
  DatabaseDialect dialect_;

  std::string __GetTableAliasFromParent(
      const NvSelect<TParameterType>& select) const;

  uint32_t __GetCurrentParameterIndexFromParent(
      const NvSelect<TParameterType>& select) const;

  void __CreateNewSelectBlock(std::vector<NvSelect<TParameterType>>& selects,
                              uint32_t index, uint32_t level,
                              const std::string& table_alias);

  std::string __GenerateSelectQuery(const NvSelect<TParameterType>& selects,
                                    bool pretty_print) const;

 public:
  explicit FromTableStatement(
      std::shared_ptr<std::vector<TParameterType>> values,
      NvSelect<TParameterType>* parent, uint32_t parameter_index,
      uint32_t level, DatabaseDialect dialect)
                  : parent_(parent),
                    tables_(),
                    parameter_values_(values),
                    subqueries_(),
                    level_(uint32_t(level)),
                    current_parameter_index_(parameter_index),
                    dialect_(dialect) {}

  ~FromTableStatement() {}

  /// @brief Add table clause inside FROM statement
  /// @param table
  /// @return
  FromTableStatement& AddTable(FromTable&& table) {
    tables_.emplace_back(std::forward<FromTable>(table));
    return *this;
  }

  /// @brief Add table clause inside FROM statement
  /// @param table_name
  /// @param table_alias
  /// @return
  FromTableStatement& AddTable(
      const std::string& table_name,
      const std::optional<std::string>& table_alias = std::nullopt) {
    tables_.emplace_back(table_name, table_alias);
    return *this;
  }

  uint32_t GetCurrentParameterIndex() const {
    return current_parameter_index_;
  }

  void UpdateCurrentParamIndex(uint32_t param_index) {
    current_parameter_index_ = param_index;
  }

  /// @brief Construct SUBQUERY inside FROM Statement block
  /// @param table_alias
  /// @return
  NvSelect<TParameterType>& BeginSubquery(const std::string& table_alias);

  NvSelect<TParameterType>& Reset() {
    subqueries_.clear();
    tables_.clear();
    current_parameter_index_ = 0;
    return parent_;
  }

  bool Empty() const {
    return tables_.empty();
  }

  /// @brief End of FROM statement BLOCK. For SUBQUERY must call
  /// .EndSubqueryInsideFromBlock() to return back to FROM block stement.
  /// @return
  NvSelect<TParameterType>& EndFromTableBlock() {
    if (!parent_) {
      throw std::runtime_error("EndFromTableBlock() null-reference to parent_");
    }

    parent_->UpdateCurrentParamIndex(current_parameter_index_);
    return *parent_;
  }

  std::string GenerateQuery(bool pretty_print = false) const {
    std::ostringstream query;
    bool first_element = true;
    for (size_t i = 0; i < tables_.size(); ++i) {
      if (!first_element) {
        query << (pretty_print ? ",\n" : ", ");
      }
      query << (pretty_print ? GenerateIndentation(level_ + 1) : "")
            << tables_[i].BuildTableName();

      first_element = false;
    }

    if (!subqueries_.empty()) {
      for (auto& s : subqueries_) {
        auto alias = __GetTableAliasFromParent(s);
        if (!first_element)
          query << (pretty_print ? ",\n" : ", ");

        query << (pretty_print ? GenerateIndentation(level_ + 1) + "(\n" : " (")
              << __GenerateSelectQuery(s, pretty_print) << ")"
              << (alias.empty() ? "" : " AS " + alias);
      }
    }
    return query.str();
  }
};

enum class FieldDefMode {
  FieldRaw = 0,
  FieldWType = 1,
  FnStaticParameter = 2,
  FnParameterizedValues = 3
};

// cppcheck-suppress unknownMacro
NVM_ENUM_CLASS_DISPLAY_TRAIT(FieldDefMode)

template <typename TParameterType = DefaultPostgresParamType>
class FieldDef {
 private:
  std::string field_;
  std::optional<std::string> table_alias_;
  const std::vector<std::string> static_param_values_;
  std::shared_ptr<std::vector<TParameterType>> parameter_values_;
  const std::vector<TParameterType> fn_values_;
  std::string function_name_;
  std::string parameter_format_;
  bool enclose_field_name_;
  SqlAggregateFunction aggregate_fn_;
  std::optional<std::string> field_alias_;
  uint32_t start_parameter_index_;
  uint32_t current_parameter_index_;
  uint32_t level_;
  FieldDefMode mode_;
  DatabaseDialect dialect_;

  uint32_t ProcessFunctionParameterIndex(
      const int32_t& current_param_index, const std::string& parameter_format,
      const std::vector<TParameterType>& parameter_values,
      const std::vector<std::string>& static_param_values) {
    if (parameter_format.empty()) {
      return current_param_index;
    }

    size_t index_params = 0;
    uint32_t param_index = uint32_t(current_param_index);
    size_t size_params = parameter_values.size();

    for (char ch : parameter_format) {
      if (index_params < size_params && ch == 'v') {
        parameter_values_->push_back(fn_values_.at(index_params));
        param_index += 1;
        index_params += 1;
      }
    }

    return param_index;
  }

  std::string BuildField() const {
    std::ostringstream oss;

    // Translate to sql keyword
    if (aggregate_fn_ == SqlAggregateFunction::Distinct) {
      oss << AggregateFunctionToString(aggregate_fn_) << " ";
    } else if (aggregate_fn_ != SqlAggregateFunction::None) {
      oss << AggregateFunctionToString(aggregate_fn_) << "(";
    }

    // append alias if any
    if (table_alias_.has_value()) {
      oss << table_alias_.value() << ".";
    }

    oss << field_;

    // determine the function closure
    if (aggregate_fn_ == SqlAggregateFunction::Distinct) {
      oss << "";
    } else if (aggregate_fn_ != SqlAggregateFunction::None) {
      oss << ")";
    }

    // append new name alias if any
    if (field_alias_.has_value()) {
      oss << " AS " << field_alias_.value();
    }

    return oss.str();
  }

  std::string BuildFunctionWithDynamicParameters() const {
    uint32_t param_index = start_parameter_index_;
    size_t index_params = 0;
    size_t index_statics = 0;
    size_t size_params = fn_values_.size();
    size_t size_statics = static_param_values_.size();
    bool is_first_element = true;
    std::ostringstream fn_call;

    fn_call << function_name_ << "(";
    for (char ch : parameter_format_) {
      if ((index_statics < size_statics || index_params < size_params) &&
          (ch == 's' || ch == 'v')) {
        fn_call << (is_first_element ? "" : ", ");
      }

      if (ch == 's' && index_statics < size_statics) {
        fn_call << static_param_values_[index_statics];
        index_statics += 1;
        is_first_element = false;
      } else if (ch == 'v' && index_params < size_params) {
        fn_call << DetermineParameterFormat(dialect_, param_index);
        param_index += 1;
        index_params += 1;
        is_first_element = false;
      }
    }

    fn_call << ")"
            << (field_alias_.has_value() ? " AS " + field_alias_.value() : "");

    return fn_call.str();
  }

  std::string BuildFunctionWithStaticParameters() const {
    std::ostringstream fn_call;
    fn_call << function_name_ << "(";
    for (size_t i = 0; i < static_param_values_.size(); ++i) {
      if (i > 0)
        fn_call << ", ";
      fn_call << static_param_values_[i];
    }
    fn_call << ")"
            << (field_alias_.has_value() ? " AS " + field_alias_.value() : "");

    return fn_call.str();
  }

 public:
  /// @brief Normal Field defintion
  /// @param field
  /// @param table_alias
  /// @param enclose_field_name
  /// @param aggregate_fn
  /// @param field_alias
  /// @param level
  /// @param mode
  explicit FieldDef(
      DatabaseDialect dialect, const std::string& field,
      const std::optional<std::string>& table_alias = std::nullopt,
      bool enclose_field_name = false,
      SqlAggregateFunction aggregate_fn = SqlAggregateFunction::None,
      const std::optional<std::string>& field_alias = std::nullopt,
      uint32_t level = 0, FieldDefMode mode = FieldDefMode::FieldWType)
                  : field_(field),
                    table_alias_(table_alias),
                    static_param_values_(),
                    parameter_values_(nullptr),
                    fn_values_(),
                    function_name_(),
                    parameter_format_(),
                    enclose_field_name_(enclose_field_name),
                    aggregate_fn_(aggregate_fn),
                    field_alias_(field_alias),
                    start_parameter_index_(),
                    current_parameter_index_(),
                    level_(level),
                    mode_(mode),
                    dialect_(dialect) {}

  /// @brief Function call static definitions
  /// @param function_name
  /// @param static_param_values
  /// @param level
  /// @param alias
  explicit FieldDef(DatabaseDialect dialect, const std::string& function_name,
                    const std::vector<std::string>& static_param_values,
                    uint32_t level,
                    const std::optional<std::string>& alias = std::nullopt)
                  : field_(),
                    table_alias_(),
                    static_param_values_(static_param_values),
                    parameter_values_(nullptr),
                    fn_values_(),
                    function_name_(function_name),
                    parameter_format_(),
                    enclose_field_name_(),
                    aggregate_fn_(SqlAggregateFunction::None),
                    field_alias_(alias),
                    start_parameter_index_(),
                    current_parameter_index_(),
                    level_(level),
                    mode_(FieldDefMode::FnStaticParameter),
                    dialect_(dialect) {}

  /// @brief Function call for parameterized parameters definitions
  /// @param function_name
  /// @param parameter_format
  /// @param parameter_values
  /// @param static_param_values
  /// @param param_index
  /// @param level
  /// @param alias
  explicit FieldDef(
      DatabaseDialect dialect, const std::string& function_name,
      const std::string& parameter_format,
      std::shared_ptr<std::vector<TParameterType>> parameter_values,
      const std::vector<TParameterType>& fn_param_values,
      const std::vector<std::string>& static_param_values, uint32_t param_index,
      uint32_t level, const std::optional<std::string>& alias = std::nullopt)
                  : field_(),
                    table_alias_(),
                    static_param_values_(static_param_values),
                    parameter_values_(parameter_values),
                    fn_values_(fn_param_values),
                    function_name_(function_name),
                    parameter_format_(parameter_format),
                    enclose_field_name_(),
                    aggregate_fn_(SqlAggregateFunction::None),
                    field_alias_(alias),
                    start_parameter_index_(param_index),
                    current_parameter_index_(ProcessFunctionParameterIndex(
                        param_index, parameter_format, fn_values_,
                        static_param_values_)),
                    level_(level),
                    mode_(FieldDefMode::FnParameterizedValues),
                    dialect_(dialect) {}

  FieldDefMode Mode() const {
    return mode_;
  }

  uint32_t GetCurrentParameterIndex() const {
    return current_parameter_index_;
  }

  std::string Field() const {
    return field_;
  }

  std::optional<std::string> TableAlias() const {
    return table_alias_;
  }

  std::optional<std::string> FieldAlias() const {
    return field_alias_;
  }

  bool EncloseFieldName() const {
    return enclose_field_name_;
  }

  SqlAggregateFunction AggregateFunction() const {
    return aggregate_fn_;
  }

  std::string FunctionName() const {
    return function_name_;
  }

  const std::vector<std::string>& StaticParameterValues() {
    return static_param_values_;
  }

  std::shared_ptr<std::vector<TParameterType>> Values() const {
    return parameter_values_;
  }

  std::string GenerateQuery() const {
    switch (mode_) {
      case FieldDefMode::FieldRaw:
        return BuildField();
      case FieldDefMode::FieldWType:
        return BuildField();
      case FieldDefMode::FnStaticParameter:
        return BuildFunctionWithStaticParameters();
      case FieldDefMode::FnParameterizedValues:
        return BuildFunctionWithDynamicParameters();
      default:
        return std::string();
    }
  }

  std::string AggregateFunctionToString(SqlAggregateFunction fn) const {
    switch (fn) {
      case SqlAggregateFunction::Distinct:
        return "DISTINCT";
      case SqlAggregateFunction::Count:
        return "COUNT";
      case SqlAggregateFunction::Avg:
        return "AVG";
      case SqlAggregateFunction::Sum:
        return "SUM";
      case SqlAggregateFunction::ToUpper:
        return "TO_UPPER";
      case SqlAggregateFunction::ToLower:
        return "TO_LOWER";
      default:
        return "";
    }
  }
};

/// @brief Fluent SQL Select Builder. TParameterType is typedef of
/// std::variant<supported_data_type_by_cpp_for_db>. You can customize to your
/// supported c++ data type for any database based on the db connector that
/// you are use.
/// @tparam TParameterType DefaultPostgresParamType.
template <typename TParameterType>
class NvSelect final {
 private:
  uint32_t current_param_index_;
  uint32_t level_;
  std::vector<JoinStatement<TParameterType>> join_blocks_;
  std::shared_ptr<FromTableStatement<TParameterType>> from_table_;
  std::vector<FieldDef<TParameterType>> fields_;
  std::shared_ptr<std::vector<TParameterType>> parameter_values_;
  FromTableStatement<TParameterType>* subquery_from_parent_;
  std::string table_alias_;
  std::shared_ptr<WhereStatement<TParameterType>> where_;
  std::shared_ptr<OrderByStatement<TParameterType>> order_by_;
  std::shared_ptr<GroupByStatement<TParameterType>> group_by_;
  WhereStatement<TParameterType>* subquery_where_parent_;
  DatabaseDialect dialect_;

 public:
  /// @brief Construct NvSelect with parameter index start from 1.
  explicit NvSelect(DatabaseDialect dialect = DatabaseDialect::PostgreSQL)
                  : current_param_index_(1),
                    level_(0),
                    join_blocks_(),
                    from_table_(nullptr),
                    fields_(),
                    parameter_values_(
                        std::make_shared<std::vector<TParameterType>>()),
                    subquery_from_parent_(nullptr),
                    table_alias_(),
                    where_(nullptr),
                    order_by_(nullptr),
                    group_by_(nullptr),
                    subquery_where_parent_(nullptr),
                    dialect_(dialect) {}

  /// @brief Construct NvSelect with parameter as specified.
  /// @param current_param_index start of parameter index, must be 1 based for
  /// the start.
  explicit NvSelect(uint32_t current_param_index,
                    DatabaseDialect dialect = DatabaseDialect::PostgreSQL)
                  : current_param_index_(current_param_index),
                    level_(0),
                    join_blocks_(),
                    from_table_(nullptr),
                    fields_(),
                    parameter_values_(
                        std::make_shared<std::vector<TParameterType>>()),
                    subquery_from_parent_(nullptr),
                    table_alias_(),
                    where_(nullptr),
                    order_by_(nullptr),
                    group_by_(nullptr),
                    subquery_where_parent_(nullptr),
                    dialect_(dialect) {}

  /// @brief DO NOT USE THIS DIRECTLY, SUBQUERY USE THIS CONST
  /// @param current_param_index
  /// @param level
  explicit NvSelect(std::shared_ptr<std::vector<TParameterType>> values,
                    uint32_t current_param_index, uint32_t level,
                    DatabaseDialect dialect)
                  : current_param_index_(current_param_index),
                    level_(level),
                    join_blocks_(),
                    from_table_(nullptr),
                    fields_(),
                    parameter_values_(values),
                    subquery_from_parent_(nullptr),
                    table_alias_(),
                    where_(nullptr),
                    order_by_(nullptr),
                    group_by_(nullptr),
                    subquery_where_parent_(nullptr),
                    dialect_(dialect) {}

  /// @brief DO NOT USE DIRECTLY, SUBQUERY FROM NESTED FROM STATEMENT USE THIS
  /// CONST
  /// @param current_param_index
  /// @param level
  /// @param from_obj
  /// @param table_alias
  explicit NvSelect(std::shared_ptr<std::vector<TParameterType>> values,
                    uint32_t current_param_index, uint32_t level,
                    FromTableStatement<TParameterType>* from_obj,
                    const std::string& table_alias, DatabaseDialect dialect)
                  : current_param_index_(current_param_index),
                    level_(level),
                    join_blocks_(),
                    from_table_(nullptr),
                    fields_(),
                    parameter_values_(values),
                    subquery_from_parent_(from_obj),
                    table_alias_(std::string(table_alias)),
                    where_(nullptr),
                    order_by_(nullptr),
                    group_by_(nullptr),
                    subquery_where_parent_(nullptr),
                    dialect_(dialect) {}

  /// @brief DO NOT USE DIRECTLY, SUBQUERY FROM NESTED WHERE STATEMENT USE
  /// THIS CONST
  /// @param current_param_index
  /// @param level
  /// @param from_obj
  /// @param table_alias
  explicit NvSelect(std::shared_ptr<std::vector<TParameterType>> values,
                    WhereStatement<TParameterType>* where_obj,
                    uint32_t current_param_index, uint32_t level,
                    const std::string& table_alias, DatabaseDialect dialect)
                  : current_param_index_(current_param_index),
                    level_(level),
                    join_blocks_(),
                    from_table_(nullptr),
                    fields_(),
                    parameter_values_(values),
                    subquery_from_parent_(nullptr),
                    table_alias_(std::string(table_alias)),
                    where_(nullptr),
                    order_by_(nullptr),
                    group_by_(nullptr),
                    subquery_where_parent_(where_obj),
                    dialect_(dialect) {}

  ~NvSelect() {}

  DatabaseDialect Dialect() const {
    return dialect_;
  }

  uint32_t GetCurrentParamIndex() const {
    return current_param_index_;
  }

  void UpdateCurrentParamIndex(uint32_t current_param_index) {
    current_param_index_ = current_param_index;
  }

  std::string TableAlias() const {
    return table_alias_;
  }

  uint32_t GetBlockLevel() const {
    return level_;
  }

  /// @brief Define select field/column
  /// @tparam T type in std::variant of TParameterType
  /// @param field
  /// @return
  template <typename T>
  NvSelect& Field(const std::string& field) {
    return Field<T>(field, std::nullopt, std::nullopt,
                    SqlAggregateFunction::None, false);
  }

  /// @brief Define select field/column
  /// @tparam T type in std::variant of TParameterType
  /// @param field
  /// @param table_alias
  /// @return
  template <typename T>
  NvSelect& Field(const std::string& field,
                  const std::optional<std::string>& table_alias) {
    return Field<T>(field, table_alias, std::nullopt,
                    SqlAggregateFunction::None, false);
  }

  /// @brief Define select field/column
  /// @tparam T type in std::variant of TParameterType
  /// @param field
  /// @param table_alias
  /// @param field_alias
  /// @return
  template <typename T>
  NvSelect& Field(const std::string& field,
                  const std::optional<std::string>& table_alias,
                  const std::optional<std::string>& field_alias) {
    return Field<T>(field, table_alias, field_alias, SqlAggregateFunction::None,
                    false);
  }

  /// @brief Define select field/column
  /// @tparam T type in std::variant of TParameterType
  /// @param field
  /// @param table_alias
  /// @param aggregate_fn
  /// @return
  template <typename T>
  NvSelect& Field(const std::string& field,
                  const std::optional<std::string>& table_alias,
                  SqlAggregateFunction aggregate_fn) {
    return Field<T>(field, table_alias, std::nullopt, aggregate_fn, false);
  }

  /// @brief Define select field/column
  /// @tparam T type in std::variant of TParameterType
  /// @param field
  /// @param table_alias
  /// @param field_alias
  /// @param aggregate_fn
  /// @param enclose_field_name
  /// @return
  template <typename T>
  NvSelect& Field(const std::string& field,
                  const std::optional<std::string>& table_alias,
                  const std::optional<std::string>& field_alias,
                  SqlAggregateFunction aggregate_fn, bool enclose_field_name) {
    fields_.emplace_back(dialect_, field, table_alias, enclose_field_name,
                         aggregate_fn, field_alias, level_,
                         FieldDefMode::FieldWType);
    return *this;
  }

  // Non template Field

  /// @brief Define select field/column.
  /// When using F the tuple row static type generation will be not available
  /// for this field.
  /// @param field
  /// @return
  NvSelect& F(const std::string& field) {
    return F(field, std::nullopt, std::nullopt, SqlAggregateFunction::None,
             false);
  }

  /// @brief Define select field/column.
  /// When using F the tuple row static type generation will be not available
  /// for this field.
  /// @param field
  /// @param table_alias
  /// @return
  NvSelect& F(const std::string& field,
              const std::optional<std::string>& table_alias) {
    return F(field, table_alias, std::nullopt, SqlAggregateFunction::None,
             false);
  }

  /// @brief Define select field/column.
  /// When using F the tuple row static type generation will be not available
  /// for this field.
  /// @param field
  /// @param table_alias
  /// @param field_alias
  /// @return
  NvSelect& F(const std::string& field,
              const std::optional<std::string>& table_alias,
              const std::optional<std::string>& field_alias) {
    return F(field, table_alias, field_alias, SqlAggregateFunction::None,
             false);
  }

  /// @brief Define select field/column.
  /// When using F the tuple row static type generation will be not available
  /// for this field.
  /// @param field
  /// @param table_alias
  /// @param aggregate_fn
  /// @return
  NvSelect& F(const std::string& field,
              const std::optional<std::string>& table_alias,
              SqlAggregateFunction aggregate_fn) {
    return F(field, table_alias, std::nullopt, aggregate_fn, false);
  }

  /// @brief Define select field/column.
  /// When using F the tuple row static type generation will be not available
  /// for this field.
  /// @param field
  /// @param table_alias
  /// @param field_alias
  /// @param aggregate_fn
  /// @param enclose_field_name
  /// @return
  NvSelect& F(const std::string& field,
              const std::optional<std::string>& table_alias,
              const std::optional<std::string>& field_alias,
              SqlAggregateFunction aggregate_fn, bool enclose_field_name) {
    fields_.emplace_back(dialect_, field, table_alias, enclose_field_name,
                         aggregate_fn, field_alias, level_,
                         FieldDefMode::FieldRaw);
    return *this;
  }

  /// @brief End of Subquery inside From statement [Match:
  /// From().BeginSubquery()]. When use on root level will thrown std::runtime
  /// error.
  /// @return
  FromTableStatement<TParameterType>& EndSubqueryInsideFrom() {
    if (!subquery_from_parent_)
      throw std::runtime_error(
          "Call this only from .From().AddSubquery().EndFromSubquery()");

    // std::cout << "Subquery End:" << current_param_index_ << std::endl;
    // if (subquery_from_parent_->GetCurrentParameterIndex() !=
    //     current_param_index_) {
    subquery_from_parent_->UpdateCurrentParamIndex(current_param_index_);
    // }
    return *subquery_from_parent_;

    // return from_table_;
  }

  WhereStatement<TParameterType>& EndSubqueryInsideWhereCondition() {
    if (!subquery_where_parent_)
      throw std::runtime_error(
          "Call this only from .Where().AddSubquery().EndFromSubquery()");

    subquery_where_parent_->UpdateCurrentParameterIndex(current_param_index_);
    return *subquery_where_parent_;
  }

  /// @brief Construct SQL FROM Statement block
  /// @return
  FromTableStatement<TParameterType>& From() {
    try {
      if (!from_table_) {
        // explicit
        // FromTableStatement(std::shared_ptr<std::vector<TParameterType>>
        // values, NvSelect<TParameterType>* parent,
        //                       uint32_t parameter_index, uint32_t level)
        //           : parent_(parent),
        //             tables_(),
        //             parameter_values_(values),
        //             subqueries_(),
        //             level_(uint32_t(level)),
        //             current_parameter_index_(parameter_index) {}

        from_table_ = std::make_shared<FromTableStatement<TParameterType>>(
            parameter_values_, this, current_param_index_, level_, dialect_);
      }

      // if (from_table_->GetCurrentParameterIndex() != current_param_index_)
      // {
      //   from_table_->UpdateCurrentParamIndex(current_param_index_);
      // }

      return *from_table_;
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
      throw e;
    }
  }

  /// @brief Construct SQL WHERE Statement block
  /// @return
  WhereStatement<TParameterType>& Where() {
    // TARGET CONST
    // explicit WhereStatement(std::shared_ptr<std::vector<TParamaterType>>,
    // parameter_values, NvSelect<TParamaterType>* parent,
    //                       uint32_t current_param_index, uint32_t level)
    //               : parent_(parent),
    //                 values_(parameter_values),
    //                 conditions_(),
    //                 level_(level),
    //                 current_param_index_(current_param_index) {}

    if (!where_) {
      where_ = std::make_shared<WhereStatement<TParameterType>>(
          parameter_values_, this, current_param_index_, level_, dialect_);
      // std::cout << "Where INIT:" << current_param_index_ <<std::endl;
    }

    if (where_->GetCurrentParameterIndex() != current_param_index_) {
      where_->UpdateCurrentParameterIndex(current_param_index_);
    }

    return *where_;
  }

  // /// @brief End of SELECT Statement
  // /// @return
  // NvSelect& EndBlock() {
  //   return *this;
  // }

  /// @brief Construct Join Statement Block
  /// @return
  JoinStatement<TParameterType>& Join() {
    try {
      join_blocks_.emplace_back(*this, current_param_index_, level_, dialect_);
      return join_blocks_.back();
    } catch (const std::exception& e) {
      std::cout << "Create JOINBLOCK_FAILED [" << level_ << "]: " << e.what()
                << std::endl;
      throw e;
    }
  }

  /// @brief Construct SQL ORDER BY Statement block
  /// @return
  OrderByStatement<TParameterType>& OrderBy() {
    if (!order_by_) {
      order_by_ =
          std::make_shared<OrderByStatement<TParameterType>>(this, level_);
    }

    return *order_by_;
  }

  /// @brief Construct SQL GROUP BY Statement block
  /// @return
  GroupByStatement<TParameterType>& GroupBy() {
    if (!group_by_) {
      group_by_ = std::make_shared<GroupByStatement<TParameterType>>(
          this, current_param_index_, level_);
    }

    return *group_by_;
  }

  /// @brief Static function call in SELECT field.
  /// For SQL string make sure you have sanitize the input value,
  /// passing sql string inside this function will bypass the driver or db
  /// level sanitazion process.
  /// @example
  /// ```cxx
  /// // Execute LOWER(u.username) AS username
  /// select.Fn("LOWER",{"u.username"},"username");
  /// ```
  /// @param fn_name
  /// @param param_values
  /// @param field_alias
  /// @return
  NvSelect& Fn(const std::string& fn_name,
               const std::vector<std::string>& param_values,
               const std::optional<std::string>& field_alias = std::nullopt) {
    // TARGET
    // explicit FieldDef(const std::string& function_name,
    //                   const std::vector<std::string>& static_param_values,
    //                   uint32_t level,
    //                   const std::optional<std::string>& alias = std::nullopt)

    fields_.emplace_back(fn_name, param_values, level_, field_alias);

    // No need to update current_parameter_index
    return *this;
  }

  /// @brief Adavance parameterized function call. For Nested function called
  /// still in WIP.
  /// @param fn_name function name to execute
  /// @param parameter_list_format %v : TParameterType parameter value, %s :
  /// static value. Implementations will automatically set the first parameter
  /// format found in parameter_list_format. "%"
  /// @param param_values
  /// @param static_param_values
  /// @param alias
  /// @return
  NvSelect& Fn(const std::string& fn_name,
               const std::string& parameter_list_format,
               const std::vector<TParameterType>& param_values,
               const std::vector<std::string>& static_param_values =
                   std::vector<std::string>(),
               std::optional<std::string> alias = std::nullopt) {
    // TARGET
    // explicit FieldDef(
    // const std::string& function_name, const std::string& parameter_format,
    // std::shared_ptr<std::vector<TParameterType>> parameter_values,
    // const std::vector<TParameterType>& fn_param_values,
    // const std::vector<std::string>& static_param_values, uint32_t
    // param_index, uint32_t level, const std::optional<std::string>& alias =
    // std::nullopt)

    fields_.emplace_back(dialect_, fn_name, parameter_list_format,
                         parameter_values_, param_values, static_param_values,
                         current_param_index_, level_, alias);

    // sync the current param index
    auto f = fields_.back();
    current_param_index_ = f.GetCurrentParameterIndex();

    return *this;
  }

  template <typename T>
  NvSelect& Fn(const std::string& fn_name,
               const std::vector<std::string>& param_values,
               const std::optional<std::string>& field_alias = std::nullopt) {
    // TARGET
    // explicit FieldDef(const std::string& function_name,
    //                   const std::vector<std::string>& static_param_values,
    //                   uint32_t level,
    //                   const std::optional<std::string>& alias = std::nullopt)

    fields_.emplace_back(dialect_, fn_name, param_values, level_, field_alias);

    // No need to update current_parameter_index
    return *this;
  }

  template <typename T>
  NvSelect& Fn(const std::string& fn_name,
               const std::string& parameter_list_format,
               const std::vector<TParameterType>& param_values,
               const std::vector<std::string>& static_param_values =
                   std::vector<std::string>(),
               std::optional<std::string> alias = std::nullopt) {
    // TARGET
    // explicit FieldDef(
    // const std::string& function_name, const std::string& parameter_format,
    // std::shared_ptr<std::vector<TParameterType>> parameter_values,
    // const std::vector<TParameterType>& fn_param_values,
    // const std::vector<std::string>& static_param_values, uint32_t
    // param_index, uint32_t level, const std::optional<std::string>& alias =
    // std::nullopt)

    fields_.emplace_back(dialect_, fn_name, parameter_list_format,
                         parameter_values_, param_values, static_param_values,
                         current_param_index_, level_, alias);

    // sync the current param index
    auto f = fields_.back();
    current_param_index_ = f.GetCurrentParameterIndex();

    return *this;
  }

  /// @brief Build and Generate SQL string in this NvSelect object
  /// @param pretty_print
  /// @return
  std::string GenerateQuery(bool pretty_print = false) const {
    std::ostringstream query;

    // SELECT
    query << (pretty_print ? GenerateIndentation(level_) + "SELECT \n"
                           : "SELECT ");
    bool first_element = true;
    for (const auto& field : fields_) {
      if (!first_element)
        query << (pretty_print ? ",\n" : ", ");
      query << (pretty_print ? GenerateIndentation(level_ + 1) : "")
            << field.GenerateQuery();
      first_element = false;
    }

    // FROM
    if (from_table_ != nullptr && !from_table_->Empty()) {
      query << (pretty_print ? "\n" + GenerateIndentation(level_) + "FROM \n"
                             : " FROM ")
            << from_table_->GenerateQuery(pretty_print);
    }

    // JOIN
    if (!join_blocks_.empty()) {
      query << (pretty_print ? "\n" : " ");
      for (const auto& join_block : join_blocks_) {
        query << join_block.GenerateQuery(pretty_print);
      }
    }

    // WHERE
    if (where_ != nullptr) {
      query << (pretty_print ? "\n" + GenerateIndentation(level_) + "WHERE"
                             : " WHERE ");
      query << (pretty_print ? GenerateIndentation(level_ + 1) : "")
            << where_->GenerateQuery(pretty_print, false);
    }

    // GROUP BY
    if (group_by_ != nullptr) {
      query << (pretty_print ? GenerateIndentation(level_) + "\nGROUP BY \n"
                             : " GROUP BY ");
      query << (pretty_print ? GenerateIndentation(level_ + 1) : "")
            << group_by_->GenerateQuery(pretty_print);
    }

    // HAVING BY

    // ORDER BY
    if (order_by_ != nullptr) {
      query << (pretty_print ? GenerateIndentation(level_) + "\nORDER BY \n"
                             : " ORDER BY ");
      query << (pretty_print ? GenerateIndentation(level_ + 1) : "")
            << order_by_->GenerateQuery(pretty_print);
    }

    // LIMIT

    return query.str();
  }

  /// @brief Get parameter values, all values has been packed with order based
  /// on the parameter index
  /// @return
  std::shared_ptr<std::vector<TParameterType>> Values() const {
    return parameter_values_;
  }

  template <typename... FieldTypes>
  auto GenerateTuplesHolder() {
    return std::vector<std::tuple<FieldTypes...>>();
  }
};

// Late Complete Declare

template <typename TParameterType>
std::string JoinDef<TParameterType>::GenerateJoinRecordSubqueryObject() const {
  if (!subquery_obj_)
    return std::string();
  return subquery_obj_->GenerateQuery();
};

template <typename TParameterType>
std::string JoinStatement<TParameterType>::__GenerateSelectBlock(
    const NvSelect<TParameterType>& select) {
  return select.GenerateQuery();
}

template <typename TParameterType>
uint32_t
FromTableStatement<TParameterType>::__GetCurrentParameterIndexFromParent(
    const NvSelect<TParameterType>& select) const {
  return select.GetCurrentParamIndex();
}

template <typename TParameterType>
NvSelect<TParameterType>& FromTableStatement<TParameterType>::BeginSubquery(
    const std::string& table_alias) {
  uint32_t index;
  index = current_parameter_index_;

  __CreateNewSelectBlock(subqueries_, index, level_ + 1, table_alias);
  return subqueries_.back();
}

template <typename TParameterType>
void FromTableStatement<TParameterType>::__CreateNewSelectBlock(
    std::vector<NvSelect<TParameterType>>& selects, uint32_t index,
    uint32_t level, const std::string& table_alias) {
  selects.emplace_back(parameter_values_, index, level, this, table_alias,
                       dialect_);
}

template <typename TParameterType>
std::string FromTableStatement<TParameterType>::__GenerateSelectQuery(
    const NvSelect<TParameterType>& select, bool pretty_print) const {
  return select.GenerateQuery(pretty_print);
}

template <typename TParameterType>
std::string Condition<TParameterType>::__GenerateQueryFromSubquery(
    bool pretty_print) const {
  if (!subquery_)
    return std::string();
  return subquery_->GenerateQuery(pretty_print);
}

template <typename TParameterType>
std::string FromTableStatement<TParameterType>::__GetTableAliasFromParent(
    const NvSelect<TParameterType>& select) const {
  return select.TableAlias();
}

}  // namespace nvm::sqlbuilder