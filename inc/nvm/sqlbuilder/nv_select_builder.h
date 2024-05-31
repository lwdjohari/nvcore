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
#include "nvm/sqlbuilder/fields.h"
#include "nvm/sqlbuilder/from.h"
#include "nvm/sqlbuilder/group_by.h"
#include "nvm/sqlbuilder/join.h"
#include "nvm/sqlbuilder/limit_offset_statement.h"
#include "nvm/sqlbuilder/order_by.h"
#include "nvm/sqlbuilder/where.h"

namespace nvm::sqlbuilder {

/// @brief Fluent SQL Select Builder. TParameterType is typedef of
/// std::variant<supported_data_type_by_cpp_for_db>. You can customize to your
/// supported c++ data type for any database based on the db connector that
/// you are use.
/// @tparam TParameterType DefaultPostgresParamType.
template <typename TParameterType>
class NvSelect final : public NvSelectBasic {
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
  std::shared_ptr<LimitOffsetStatement<TParameterType>> limit_offset_;
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
                    limit_offset_(nullptr),
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
                    limit_offset_(nullptr),
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
                    limit_offset_(nullptr),
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
                    limit_offset_(nullptr),
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
                    limit_offset_(nullptr),
                    dialect_(dialect) {}

  ~NvSelect() {}

  DatabaseDialect Dialect() const override {
    return dialect_;
  }

  uint32_t GetCurrentParamIndex() const override {
    return current_param_index_;
  }

  void UpdateCurrentParamIndex(uint32_t current_param_index) override {
    current_param_index_ = current_param_index;
  }

  std::string TableAlias() const override {
    return table_alias_;
  }

  uint32_t GetBlockLevel() const override {
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

  LimitOffsetStatement<TParameterType>& LimitOffset() {
    if (!limit_offset_) {
      // explicit LimitOffsetStatement(
      //     NvSelect<TParamType>* parent,
      //     std::shared_ptr<std::vector<TParamType>> parameter_values,
      //     uint32_t param_index, uint32_t level, DatabaseDialect dialect)

      limit_offset_ = std::make_shared<LimitOffsetStatement<TParameterType>>(
          this, parameter_values_, current_param_index_, level_, dialect_);
    }

    return *limit_offset_;
  }

  /// @brief Build and Generate SQL string in this NvSelect object
  /// @param pretty_print
  /// @return
  std::string GenerateQuery(bool pretty_print = false) const override {
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