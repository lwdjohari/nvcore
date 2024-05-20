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

#include <nvm/macro.h>

#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>

#include "nvm/containers/record_def.h"

namespace nvm::containers {

enum class GroupByMode {
  Field = 1,
  FunctionCall = 2,
  RawString = 3,
};

NVM_ENUM_CLASS_DISPLAY_TRAIT(GroupByMode)

class GroupByClause {
 private:
  std::string field_name_;
  std::optional<std::string> table_alias_;
  uint32_t start_parameter_index_;
  uint32_t parameter_index_;
  uint32_t level_;
  GroupByMode mode_;

  uint32_t ProcessNextParameterIndex(GroupByMode mode,
                                     uint32_t start_parameter_index) {
    return start_parameter_index;
  }

 public:
  explicit GroupByClause(const std::string& field_name,
                         const std::optional<std::string>& alias,
                         GroupByMode mode, uint32_t parameter_index,
                         uint32_t level)
                  : field_name_(field_name),
                    table_alias_(alias),
                    start_parameter_index_(parameter_index),
                    parameter_index_(
                        ProcessNextParameterIndex(mode, parameter_index)),
                    level_(level),
                    mode_(mode) {}

  uint32_t NextParameterIndex() const {
    return parameter_index_;
  }

  std::string FieldName() const {
    return field_name_;
  }

  std::optional<std::string> TableAlias() const {
    return table_alias_;
  }

  std::string BuildFieldname() const {
    return table_alias_.has_value() ? table_alias_.value() + "." + field_name_
                                    : field_name_;
  }

  std::string GenerateQuery() const {
    return BuildFieldname();
  }
};

template <typename TParameterType>
class GroupByStatement {
 public:
  GroupByStatement() : parent_(nullptr), sorts_(), level_(), param_index_(1) {}

  explicit GroupByStatement(NvSelect<TParameterType>* parent,
                            uint32_t parameter_index, uint32_t level)
                  : parent_(parent),
                    sorts_(),
                    level_(level),
                    param_index_(parameter_index) {}

  GroupByStatement& Field(
      const std::string& field_name,
      const std::optional<std::string>& table_alias = std::nullopt) {
    sorts_.emplace_back(field_name, table_alias, GroupByMode::Field,
                        param_index_, level_);

    auto back = sorts_.back();
    param_index_ = back.NextParameterIndex();
    return *this;
  }

  std::string GenerateQuery(bool pretty_print = false) const {
    std::ostringstream query;
    bool is_first_element = true;

    for (auto& s : sorts_) {
      if (!is_first_element)
        query << ", ";

      query << s.GenerateQuery();
      is_first_element = false;
    }

    return query.str();
  }

  uint32_t CurrentParameterIndex() const {
    return param_index_;
  }

  NvSelect<TParameterType>& EndGroupByBlock() {
    if (!parent_)
      throw std::runtime_error(
          "null-reference to parent of NvSelect<TParameterType>");

    return *parent_;
  }

 private:
  NvSelect<TParameterType>* parent_;
  std::vector<GroupByClause> sorts_;
  uint32_t level_;
  uint32_t param_index_;
};

}  // namespace nvm::containers