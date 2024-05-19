#pragma once

#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "nvm/containers/record_def.h"

namespace nvm::containers {

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

enum class SqlJoinType {
  None = 0,
  InnerJoin = 1,
  LeftJoin = 2,
  RightJoin = 4,
};

enum class JoinDefMode {
  RecordKeyBoth = 0,
  SubquerySelectString = 1,
  SubqueryRawString = 2,
  SubquerySelectObject = 3
};

// Forward declration for SelectBlock
template <typename TParameterType = DefaultPostgresParamType>
class SelectBlock;

struct RecordKey {
  std::string table;
  std::string field;
  std::optional<std::string> table_alias;
  bool initialize;

  RecordKey()
      : table(), field(), table_alias(std::nullopt), initialize(false) {}

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

  JoinDef(RecordKey&& existing_table, SelectBlock<TParameterType>&& subquery,
          SqlJoinType join, uint32_t level)
      : subquery_str_(),
        subsquery_str_alias_(),
        subquery_field_key_(),
        subquery_obj_(std::make_shared<SelectBlock<TParameterType>>(
            std::forward<SelectBlock<TParameterType>>(subquery))),
        left_table_(std::forward<RecordKey>(existing_table)),
        right_table_(RecordKey()),
        join_type_(join),
        join_mode_(JoinDefMode::SubquerySelectObject),
        level_(level) {}

  SqlJoinType JoinType() const { return join_type_; }

  JoinDefMode Mode() const { return join_mode_; }

  const RecordKey& LeftTable() const { return left_table_; }

  const RecordKey& RightTable() const { return right_table_; }

  SelectBlock<TParameterType>& Subquery() { return *subquery_obj_; }

  bool IsHasSubqueryObject() { return subquery_obj_ != nullptr; }

  const std::string& SubqueryString() const { return subquery_str_; }

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
  mutable std::shared_ptr<SelectBlock<TParameterType>> subquery_obj_;
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
class JoinBlock {
 private:
  SelectBlock<TParameterType>& parent_;
  std::vector<JoinDef<TParameterType>> joins_;
  // std::unique_ptr<SelectBlock<TParameterType>> subquery_;
  uint32_t current_parameter_index_;
  uint32_t level_;

 public:
  explicit JoinBlock(SelectBlock<TParameterType>& parent, uint32_t level)
      : parent_(parent),
        joins_(),
        // subquery_(nullptr),
        current_parameter_index_(),
        level_(level) {}

  SelectBlock<TParameterType>& EndJoinBlock() {
    // sync the current_parameter

    return parent_;
  };

  std::string __GenerateSelectBlock(const SelectBlock<TParameterType>& select);

  const std::vector<JoinDef<TParameterType>>& GetJoinClauses() const {
    return joins_;
  }

  bool Empty() const { return joins_.empty(); }

  std::string GenerateQuery(bool prety_print = false) const {
    std::ostringstream query;
    for (const auto& clause : joins_) {
      query << clause.GenerateQuery(prety_print) << (prety_print ? "\n" : " ");
    }
    return query.str();
  }

  JoinBlock& LeftJoin(RecordKey&& left_table, RecordKey&& right_table) {
    joins_.emplace_back(std::forward<RecordKey>(left_table),
                        std::forward<RecordKey>(right_table),
                        SqlJoinType::LeftJoin, level_);
    return *this;
  }

  JoinBlock& LeftJoin(RecordKey&& right_table, const std::string& left_table,
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

  /// @brief
  /// @param left_table
  /// @param right_table
  /// @return
  JoinBlock& RightJoin(RecordKey&& left_table, RecordKey&& right_table) {
    joins_.emplace_back(std::forward<RecordKey>(left_table),
                        std::forward<RecordKey>(right_table),
                        SqlJoinType::RightJoin, level_);
    return *this;
  }

  JoinBlock& RightJoin(RecordKey&& left_table, const std::string& right_table,
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

  JoinBlock& InnerJoin(RecordKey&& existing_select, RecordKey&& join_on_table) {
    joins_.emplace_back(std::forward<RecordKey>(existing_select),
                        std::forward<RecordKey>(join_on_table),
                        SqlJoinType::InnerJoin, level_);
    return *this;
  }

  JoinBlock& InnerJoin(RecordKey& existing_select,
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

  // SelectBlock<TParameterType>& JoinWithSubquery(SqlJoinType join_type) {
  //   if (!subquery_) {
  //     subquery_ = std::make_shared<SelectBlock<TParameterType>>(
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
class FromTableBlock {
 private:
  SelectBlock<TParameterType>& parent_;
  std::vector<FromTable> tables_;
  std::vector<SelectBlock<TParameterType>> subqueries_;
  uint32_t level_;
  uint32_t last_current_index_returned_;

 public:
  explicit FromTableBlock(SelectBlock<TParameterType>& parent, uint32_t level)
      : parent_(parent),
        tables_(),
        subqueries_(),
        level_(uint32_t(level)),
        last_current_index_returned_() {}

  ~FromTableBlock() {}

  FromTableBlock& AddTable(FromTable&& table) {
    tables_.emplace_back(std::forward<FromTable>(table));
    return *this;
  }

  FromTableBlock& AddTable(
      const std::string& table_name,
      const std::optional<std::string>& table_alias = std::nullopt) {
    tables_.emplace_back(FromTable(table_name, table_alias));
    return *this;
  }

  uint32_t GetCurrentParameterIndex() const {
    return last_current_index_returned_;
  }

  uint32_t __GetCurrentParameterIndexFromParent(
      const SelectBlock<TParameterType>& select) const;

  void __CreateNewSelectBlock(std::vector<SelectBlock<TParameterType>>& selects,
                              uint32_t index, uint32_t level,
                              const std::string& table_alias);

  std::string __GenerateSelectQuery(const SelectBlock<TParameterType>& selects,
                                    bool pretty_print) const;

  SelectBlock<TParameterType>& AddSubquery(const std::string& table_alias);

  

  SelectBlock<TParameterType>& Reset() {
    subqueries_.clear();
    tables_.clear();
    last_current_index_returned_ = 0;
    return parent_;
  }

  bool Empty() const { return tables_.empty(); }

  SelectBlock<TParameterType>& EndFromTableBlock() {
    // std::cout << "LEVEL:" << level_ << std::endl;
    // std::cout << &parent_ << std::endl;
    return parent_;
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

// template <typename TParameterType = DefaultPostgresParamType>
// struct FieldDef {
//   std::string field;
//   std::optional<std::string> table_alias;
//   bool enclose_field_name;
//   SqlAggregateFunction aggregate_fn;
//   std::optional<std::string> field_alias;

//   explicit FieldDef(
//       const std::string& field,
//       const std::optional<std::string>& table_alias = std::nullopt,
//       bool enclose_field_name = false,
//       SqlAggregateFunction aggregate_fn = SqlAggregateFunction::None,
//       const std::optional<std::string>& field_alias = std::nullopt)
//       : field(field),
//         table_alias(table_alias),
//         enclose_field_name(enclose_field_name),
//         aggregate_fn(aggregate_fn),
//         field_alias(field_alias) {}

//   std::string BuildField() const {
//     std::ostringstream oss;
//     if (aggregate_fn != SqlAggregateFunction::None) {
//       oss << AggregateFunctionToString(aggregate_fn) << "(";
//     }
//     if (table_alias.has_value()) {
//       oss << table_alias.value() << ".";
//     }
//     oss << field;
//     if (aggregate_fn != SqlAggregateFunction::None) {
//       oss << ")";
//     }
//     if (field_alias.has_value()) {
//       oss << " AS " << field_alias.value();
//     }
//     return oss.str();
//   }

//   std::string AggregateFunctionToString(SqlAggregateFunction fn) const {
//     switch (fn) {
//       case SqlAggregateFunction::Count:
//         return "COUNT";
//       case SqlAggregateFunction::Avg:
//         return "AVG";
//       case SqlAggregateFunction::Sum:
//         return "SUM";
//       case SqlAggregateFunction::ToUpper:
//         return "TO_UPPER";
//       case SqlAggregateFunction::ToLower:
//         return "TO_LOWER";
//       default:
//         return "";
//     }
//   }
// };

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

template <typename TParameterType>
class SelectBlock {
 private:
  uint32_t current_param_index_;
  uint32_t level_;
  std::vector<JoinBlock<TParameterType>> join_blocks_;
  FromTableBlock<TParameterType> from_table_;
  std::vector<FieldDef<TParameterType>> fields_;
  std::vector<TParameterType> parameter_values_;
  FromTableBlock<TParameterType>* subquery_from_parent_;
  std::string table_alias_;

 public:
  explicit SelectBlock(uint32_t current_param_index)
      : current_param_index_(current_param_index),
        level_(0),
        join_blocks_(),
        from_table_(*this, level_),
        fields_(),
        parameter_values_(),
        subquery_from_parent_(nullptr),
        table_alias_() {}

  explicit SelectBlock(uint32_t current_param_index, uint32_t level)
      : current_param_index_(current_param_index),
        level_(level),
        join_blocks_(),
        from_table_(*this, level),
        fields_(),
        parameter_values_(),
        subquery_from_parent_(nullptr),
        table_alias_() {}

  explicit SelectBlock(uint32_t current_param_index, uint32_t level,
                       FromTableBlock<TParameterType>* from_obj,
                       const std::string& table_alias)
      : current_param_index_(current_param_index),
        level_(level),
        join_blocks_(),
        from_table_(*this, level),
        fields_(),
        parameter_values_(),
        subquery_from_parent_(from_obj),
        table_alias_(std::string(table_alias)) {}

  ~SelectBlock() {}

  uint32_t GetCurrentParamIndex() const { return current_param_index_; }

  void SetCurrentParamIndex(uint32_t current_param_index) {
    current_param_index_ = current_param_index;
  }

  uint32_t GetBlockLevel() const { return level_; }

  template <typename T>
  SelectBlock& Field(const std::string& field) {
    return Field<T>(field, std::nullopt, std::nullopt,
                    SqlAggregateFunction::None, false);
  }

  template <typename T>
  SelectBlock& Field(const std::string& field,
                     const std::optional<std::string>& table_alias) {
    return Field<T>(field, table_alias, std::nullopt,
                    SqlAggregateFunction::None, false);
  }

  template <typename T>
  SelectBlock& Field(const std::string& field,
                     const std::optional<std::string>& table_alias,
                     const std::optional<std::string>& field_alias) {
    return Field<T>(field, table_alias, field_alias, SqlAggregateFunction::None,
                    false);
  }

  template <typename T>
  SelectBlock& Field(const std::string& field,
                     const std::optional<std::string>& table_alias,
                     SqlAggregateFunction aggregate_fn) {
    return Field<T>(field, table_alias, std::nullopt, aggregate_fn, false);
  }

  template <typename T>
  SelectBlock& Field(const std::string& field,
                     const std::optional<std::string>& table_alias,
                     const std::optional<std::string>& field_alias,
                     SqlAggregateFunction aggregate_fn,
                     bool enclose_field_name) {
    fields_.emplace_back(
        FieldDef<TParameterType>(field, table_alias, enclose_field_name,
                                 aggregate_fn, field_alias, level_));
    return *this;
  }

  /// @brief End of Subquery inside From statement [Match:
  /// From().AddSubquery()]. When use on root level will thrown std::runtime
  /// error.
  /// @return
  FromTableBlock<TParameterType>& EndSubqueryInsideFrom() {
    if (!subquery_from_parent_)
      throw std::runtime_error("Call this only from .From().EndFromSubquery()");

    return *subquery_from_parent_;

    // return from_table_;
  }

  FromTableBlock<TParameterType>& From() {
    try {
      return from_table_;
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
      throw e;
    }
  }

  SelectBlock& EndBlock() { return *this; }

  JoinBlock<TParameterType>& CreateJoinBlock() {
    try {
      join_blocks_.emplace_back(*this, level_);
      return join_blocks_.back();
    } catch (const std::exception& e) {
      std::cout << "Create JOINBLOCK_FAILED [" << level_ << "]: " << e.what()
                << std::endl;
      throw e;
    }
  }

  // SelectBlock& FnCall(const std::string& fn_name,
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

  // SelectBlock& FnCall(const std::string& fn_name,
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
      if (!first_element) query << (pretty_print ? ", \n" : ", ");
      query << (pretty_print ? GenerateIndentation(level_ + 1) : "")
            << field.GenerateQuery();
      first_element = false;
    }

    // FROM
    if (!from_table_.Empty()) {
      query << (pretty_print ? "\n" + GenerateIndentation(level_) + "FROM \n"
                             : " FROM ")
            << from_table_.GenerateQuery(pretty_print);
    }

    // JOIN
    if (!join_blocks_.empty()) {
      query << (pretty_print ? "\n" : " ");
      for (const auto& join_block : join_blocks_) {
        query << join_block.GenerateQuery(pretty_print);
      }
    }

    // WHERE
    // GROUP BY
    // HAVING BY
    // ORDER BY
    // LIMIT

    // RESULT
    query << (pretty_print ? GenerateIndentation(level_) : "") << closed_bracket
          << (table_alias.empty() ? "" : " AS " + table_alias);
    return query.str();
  }

  std::vector<TParameterType> Values() const { return parameter_values_; }

  template <typename... FieldTypes>
  auto GenerateTuplesHolder() {
    return std::vector<std::tuple<FieldTypes...>>();
  }
};

// Late Complete Declare

template <typename TParameterType>
std::string JoinDef<TParameterType>::GenerateJoinRecordSubqueryObject() const {
  if (!subquery_obj_) return std::string();
  return subquery_obj_->GenerateQuery();
};

template <typename TParameterType>
std::string JoinBlock<TParameterType>::__GenerateSelectBlock(
    const SelectBlock<TParameterType>& select) {
  return select.GenerateQuery();
}

template <typename TParameterType>
uint32_t FromTableBlock<TParameterType>::__GetCurrentParameterIndexFromParent(
    const SelectBlock<TParameterType>& select) const {
  return select.GetCurrentParamIndex();
}

template <typename TParameterType>
SelectBlock<TParameterType>& FromTableBlock<TParameterType>::AddSubquery(
    const std::string& table_alias) {
  uint32_t index;
  if (subqueries_.empty()) {
    index = __GetCurrentParameterIndexFromParent(parent_);
  } else {
    auto last = &subqueries_.back();
    index = __GetCurrentParameterIndexFromParent(*last);
  }

  __CreateNewSelectBlock(subqueries_, index, level_ + 1, table_alias);
  return subqueries_.back();
}

template <typename TParameterType>
void FromTableBlock<TParameterType>::__CreateNewSelectBlock(
    std::vector<SelectBlock<TParameterType>>& selects, uint32_t index,
    uint32_t level, const std::string& table_alias) {
  selects.emplace_back(index, level, this, table_alias);
}

template <typename TParameterType>
std::string FromTableBlock<TParameterType>::__GenerateSelectQuery(
    const SelectBlock<TParameterType>& select, bool pretty_print) const {
  return select.GenerateQuery(pretty_print);
}

}  // namespace nvm::containers