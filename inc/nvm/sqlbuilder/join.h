#pragma once

#include "nvm/sqlbuilder/def.h"

namespace nvm::sqlbuilder {
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
}  // namespace nvm::sqlbuilder
