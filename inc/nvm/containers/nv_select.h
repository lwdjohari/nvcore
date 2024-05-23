#pragma once

#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "nvm/containers/record_def.h"
#include "nvm/containers/record_filter.h"
#include "nvm/containers/record_groupby.h"
#include "nvm/containers/record_orderby.h"
namespace nvm::containers {

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
          uint32_t level)
                  : subquery_str_(),
                    subsquery_str_alias_(),
                    subquery_field_key_(),
                    subquery_obj_(),
                    left_table_(std::forward<RecordKey>(left_table)),
                    right_table_(std::forward<RecordKey>(right_table)),
                    join_type_(join),
                    join_mode_(JoinDefMode::RecordKeyBoth),
                    level_(level) {}

  JoinDef(RecordKey&& left_table, SqlJoinType join, const std::string& subquery,
          const std::string& subquery_field_key,
          const std::string& subquery_table_alias, SqlOperator op,
          uint32_t level)
                  : subquery_str_(std::string(subquery)),
                    subsquery_str_alias_(std::string(subquery_table_alias)),
                    subquery_field_key_(std::string(subquery_field_key)),
                    subquery_obj_(),
                    left_table_(std::forward<RecordKey>(left_table)),
                    right_table_(RecordKey()),
                    join_type_(join),
                    join_mode_(JoinDefMode::SubquerySelectString),
                    level_(level) {}

  JoinDef(RecordKey&& existing_table, NvSelect<TParameterType>&& subquery,
          SqlJoinType join, uint32_t level)
                  : subquery_str_(),
                    subsquery_str_alias_(),
                    subquery_field_key_(),
                    subquery_obj_(std::make_shared<NvSelect<TParameterType>>(
                        std::forward<NvSelect<TParameterType>>(subquery))),
                    left_table_(std::forward<RecordKey>(existing_table)),
                    right_table_(RecordKey()),
                    join_type_(join),
                    join_mode_(JoinDefMode::SubquerySelectObject),
                    level_(level) {}

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

 public:
  explicit JoinStatement(NvSelect<TParameterType>& parent,
                         uint32_t parameter_index, uint32_t level)
                  : parent_(parent),
                    joins_(),
                    // subquery_(nullptr),
                    current_parameter_index_(parameter_index),
                    level_(level) {}

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
                        SqlJoinType::LeftJoin, level_);
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
                        left_table_alias, op, level_);
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

    joins_.emplace_back(std::forward<RecordKey>(left_table),
                        SqlJoinType::LeftJoin, right_table,
                        right_table_field_key, right_table_alias, op, level_);

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
                        SqlJoinType::InnerJoin, level_);
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
                        join_table_field_key, join_table_alias, op, level_);
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

 public:
  explicit FromTableStatement(
      std::shared_ptr<std::vector<TParameterType>> values,
      NvSelect<TParameterType>* parent, uint32_t parameter_index,
      uint32_t level)
                  : parent_(parent),
                    tables_(),
                    parameter_values_(values),
                    subqueries_(),
                    level_(uint32_t(level)),
                    current_parameter_index_(parameter_index) {}

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
    tables_.emplace_back(FromTable(table_name, table_alias));
    return *this;
  }

  uint32_t GetCurrentParameterIndex() const {
    return current_parameter_index_;
  }

  void UpdateCurrentParamIndex(uint32_t param_index){
    current_parameter_index_ = param_index;
  }

  uint32_t __GetCurrentParameterIndexFromParent(
      const NvSelect<TParameterType>& select) const;

  void __CreateNewSelectBlock(std::vector<NvSelect<TParameterType>>& selects,
                              uint32_t index, uint32_t level,
                              const std::string& table_alias);

  std::string __GenerateSelectQuery(const NvSelect<TParameterType>& selects,
                                    bool pretty_print) const;

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
        if (!first_element)
          query << (pretty_print ? ",\n" : ", ")
                << __GenerateSelectQuery(s, pretty_print);
      }
    }
    return query.str();
  }
};

template <typename TParameterType = DefaultPostgresParamType>
struct FieldDef {
  std::string field;
  std::optional<std::string> table_alias;
  bool enclose_field_name;
  SqlAggregateFunction aggregate_fn;
  std::optional<std::string> field_alias;
  uint32_t level_;
  explicit FieldDef(
      const std::string& field,
      const std::optional<std::string>& table_alias = std::nullopt,
      bool enclose_field_name = false,
      SqlAggregateFunction aggregate_fn = SqlAggregateFunction::None,
      const std::optional<std::string>& field_alias = std::nullopt,
      uint32_t level = 0)
                  : field(field),
                    table_alias(table_alias),
                    enclose_field_name(enclose_field_name),
                    aggregate_fn(aggregate_fn),
                    field_alias(field_alias),
                    level_(level) {}

  std::string GenerateQuery() const {
    std::ostringstream oss;

    // Translate to sql keyword
    if (aggregate_fn == SqlAggregateFunction::Distinct) {
      oss << AggregateFunctionToString(aggregate_fn) << " ";
    } else if (aggregate_fn != SqlAggregateFunction::None) {
      oss << AggregateFunctionToString(aggregate_fn) << "(";
    }

    // append alias if any
    if (table_alias.has_value()) {
      oss << table_alias.value() << ".";
    }

    oss << field;

    // determine the function closure
    if (aggregate_fn == SqlAggregateFunction::Distinct) {
      oss << "";
    } else if (aggregate_fn != SqlAggregateFunction::None) {
      oss << ")";
    }

    // append new name alias if any
    if (field_alias.has_value()) {
      oss << " AS " << field_alias.value();
    }

    return oss.str();
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
/// supported c++ data type for any database based on the db connector that you
/// are use.
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

 public:
  /// @brief Construct NvSelect with parameter index start from 1.
  NvSelect()
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
                    group_by_(nullptr) {}

  /// @brief Construct NvSelect with parameter as specified.
  /// @param current_param_index start of parameter index, must be 1 based for
  /// the start.
  explicit NvSelect(uint32_t current_param_index)
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
                    group_by_(nullptr) {}

  /// @brief DO NOT USE THIS DIRECTLY, SUBQUERY USE THIS CONST
  /// @param current_param_index
  /// @param level
  explicit NvSelect(std::shared_ptr<std::vector<TParameterType>> values,
                    uint32_t current_param_index, uint32_t level)
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
                    group_by_(nullptr) {}

  /// @brief DO NOT USE DIRECTLY, SUBQUERY FROM NESTED FROM STATEMENT USE THIS
  /// CONST
  /// @param current_param_index
  /// @param level
  /// @param from_obj
  /// @param table_alias
  explicit NvSelect(std::shared_ptr<std::vector<TParameterType>> values,
                    uint32_t current_param_index, uint32_t level,
                    FromTableStatement<TParameterType>* from_obj,
                    const std::string& table_alias)
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
                    group_by_(nullptr) {}

  ~NvSelect() {}

  uint32_t GetCurrentParamIndex() const {
    return current_param_index_;
  }

  void UpdateCurrentParamIndex(uint32_t current_param_index) {
    current_param_index_ = current_param_index;
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
    fields_.emplace_back(
        FieldDef<TParameterType>(field, table_alias, enclose_field_name,
                                 aggregate_fn, field_alias, level_));
    return *this;
  }

  /// @brief End of Subquery inside From statement [Match:
  /// From().BeginSubquery()]. When use on root level will thrown std::runtime
  /// error.
  /// @return
  FromTableStatement<TParameterType>& EndSubqueryInsideFrom() {
    if (!subquery_from_parent_)
      throw std::runtime_error("Call this only from .From().EndFromSubquery()");

    // std::cout << "Subquery End:" << current_param_index_ << std::endl;
    // if (subquery_from_parent_->GetCurrentParameterIndex() !=
    //     current_param_index_) {
      subquery_from_parent_->UpdateCurrentParamIndex(current_param_index_);
    // }
    return *subquery_from_parent_;

    // return from_table_;
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
            parameter_values_, this, current_param_index_, level_);
      }

      // if (from_table_->GetCurrentParameterIndex() != current_param_index_) {
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

    if (!where_){
      where_ = std::make_shared<WhereStatement<TParameterType>>(
          parameter_values_, this, current_param_index_, level_);
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
      join_blocks_.emplace_back(*this, current_param_index_, level_);
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

  // NvSelect& FnCall(const std::string& fn_name,
  //                     const std::vector<std::string>& param_values) {
  //   std::ostringstream fn_call;
  //   fn_call << fn_name << "(";
  //   for (size_t i = 0; i < param_values.size(); ++i) {
  //     if (i > 0) fn_call << ", ";
  //     fn_call << param_values[i];
  //   }
  //   fn_call << ")";
  //   elements_.emplace_back(fn_call.str());
  //   return *this;
  // }

  // NvSelect& FnCall(const std::string& fn_name,
  //                     const std::string& parameter_format,
  //                     const std::vector<TParameterType>& param_values) {
  //   std::ostringstream fn_call;
  //   fn_call << fn_name << "(";
  //   size_t param_index = 0;
  //   for (char ch : parameter_format) {
  //     if (ch == 's') {
  //       fn_call
  //           << param_values[param_index++];  // Assuming param_values contain
  //                                            // strings or can be converted
  //     } else if (ch == 'v') {
  //       fn_call << "$" << current_param_index_++;
  //       parameter_values_.emplace_back(param_values[param_index++]);
  //     } else {
  //       fn_call << ch;
  //     }
  //   }
  //   fn_call << ")";
  //   elements_.emplace_back(fn_call.str());
  //   return *this;
  // }

  /// @brief Build and Generate SQL string in this NvSelect object
  /// @param pretty_print
  /// @return
  std::string GenerateQuery(bool pretty_print = false) const {
    std::ostringstream query;

    std::string opening_bracket = subquery_from_parent_ != nullptr ? "(" : "";
    std::string closed_bracket = subquery_from_parent_ != nullptr ? ")" : "";
    std::string table_alias =
        subquery_from_parent_ != nullptr ? table_alias_ : "";
    // SELECT
    query << (pretty_print
                  ? GenerateIndentation(level_) + opening_bracket + "SELECT \n"
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
      query << (pretty_print ? "\n" + GenerateIndentation(level_) + "WHERE \n"
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

    // RESULT
    query << (pretty_print ? GenerateIndentation(level_) : "") << closed_bracket
          << (table_alias.empty() ? "" : " AS " + table_alias);
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
  selects.emplace_back(parameter_values_, index, level, this, table_alias);
}

template <typename TParameterType>
std::string FromTableStatement<TParameterType>::__GenerateSelectQuery(
    const NvSelect<TParameterType>& select, bool pretty_print) const {
  return select.GenerateQuery(pretty_print);
}

}  // namespace nvm::containers