#pragma once

#include "nvm/macro.h"
#include "nvm/sqlbuilder/def.h"

namespace nvm::sqlbuilder {
enum class LimitOffsetMode { None = 0, LimitOffset = 1, Limit = 2 };

// cppcheck-suppress unknownMacro
NVM_ENUM_CLASS_DISPLAY_TRAIT(LimitOffsetMode)

template <typename TParamType = DefaultPostgresParamType>
class LimitOffsetStatement {
 public:
  explicit LimitOffsetStatement(
      NvSelect<TParamType>* parent,
      std::shared_ptr<std::vector<TParamType>> parameter_values,
      uint32_t param_index, uint32_t level, DatabaseDialect dialect)
                  : parent_(parent),
                    parameter_values_(parameter_values),
                    offset_(),
                    limit_(),
                    current_param_index_(param_index),
                    level_(level),
                    dialect_(dialect),
                    mode_() {}

  LimitOffsetMode Mode() const {
    return mode_;
  }

  uint32_t GetCurrentParameterIndex() const {
    return current_param_index_;
  }

  LimitOffsetStatement& Limit(const int32_t& limit) {
    limit_ = int32_t(limit);
    mode_ = LimitOffsetMode::Limit;

    return *this;
  }

  LimitOffsetStatement& LimitOffset(const int32_t& limit,
                                    const int64_t& offset) {
    limit_ = int32_t(limit);
    offset_ = int64_t(offset);
    mode_ = LimitOffsetMode::LimitOffset;

    return *this;
  }

  std::string GenerateQuery() const {
    std::ostringstream ss;

    switch (dialect_) {
      case DatabaseDialect::PostgreSQL:
        GeneratePostgresStatement(ss);
        break;

      case DatabaseDialect::Oracle:
        GenerateOracleStatement(ss);
        break;

      default:
        break;
    }

    return ss.str();
  }

  NvSelect<TParamType>& EndLimitOffsetBlock() {
    parent_->UpdateCurrentParamIndex(current_param_index_);
    return *parent_;
  }

 private:
  NvSelect<TParamType>* parent_;
  std::shared_ptr<std::vector<TParamType>> parameter_values_;
  int64_t offset_;
  int32_t limit_;
  uint32_t current_param_index_;
  uint32_t level_;
  DatabaseDialect dialect_;
  LimitOffsetMode mode_;

  void GenerateOracleStatement(std::ostringstream& ss) {
    //LIMIT =  FETCH FIRST 5 ROWS ONLY;
    ss << "OFFSET 20 ROWS FETCH NEXT 10 ROWS ONLY";
  }

  void GeneratePostgresStatement(std::ostringstream& ss) {
    ss << "LIMIT " << "OFFSET ";
  }
};
}  // namespace nvm::sqlbuilder
