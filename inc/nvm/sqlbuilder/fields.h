#pragma once

#include "nvm/sqlbuilder/def.h"
namespace nvm::sqlbuilder {
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
}  // namespace nvm::sqlbuilder
