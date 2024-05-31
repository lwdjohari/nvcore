#pragma once

#include "nvm/sqlbuilder/def.h"

namespace nvm::sqlbuilder {
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
}  // namespace nvm::sqlbuilder
