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

#include "nvm/sqlbuilder/def.h"
#include "nvm/sqlbuilder/policy/order_by.h"

namespace nvm::sqlbuilder {
struct OrderByClause {
  std::string field_name_;
  std::optional<std::string> table_alias;
  uint32_t level_;
  SortType sort_type_;
  bool define_sort_type_;

  explicit OrderByClause(const std::string& field_name, SortType sort,
                         bool define_sort_type, uint32_t level)
                  : field_name_(field_name),
                    table_alias(std::nullopt),
                    sort_type_(sort),
                    define_sort_type_(define_sort_type),
                    level_(level) {}

  explicit OrderByClause(const std::string& field_name,
                         const std::optional<std::string>& alias, SortType sort,
                         bool define_sort_type, uint32_t level)
                  : field_name_(field_name),
                    table_alias(alias),
                    sort_type_(sort),
                    define_sort_type_(define_sort_type),
                    level_(level) {}

  std::string BuildFieldname() const {
    return table_alias.has_value() ? table_alias.value() + "." + field_name_
                                   : field_name_;
  }

  std::string GenerateQuery() const {
    return BuildFieldname() +
           (define_sort_type_
                ? (sort_type_ == SortType::Ascending ? " ASC" : " DESC")
                : "");
  }
};

template <typename TParameterType>
class OrderByStatement {
 public:
  OrderByStatement() : parent_(nullptr), sorts_(), level_() {}

  explicit OrderByStatement(NvSelect<TParameterType>* parent, uint32_t level)
                  : parent_(parent), sorts_(), level_(level) {}

  OrderByStatement& ApplyFrom(
      const policy::OrderByPolicyParameter<TParameterType>& parameters);

  OrderByStatement& Asc(
      const std::string& field_name,
      const std::optional<std::string>& table_alias = std::nullopt,
      bool define_sort_type = true) {
    return By(field_name, table_alias, SortType::Ascending, define_sort_type);
  }

  OrderByStatement& Desc(
      const std::string& field_name,
      const std::optional<std::string>& table_alias = std::nullopt,
      bool define_sort_type = true) {
    return By(field_name, table_alias, SortType::Descending, define_sort_type);
  }

  OrderByStatement& By(
      const std::string& field_name,
      const std::optional<std::string>& table_alias = std::nullopt,
      const SortType sort_type = SortType::Ascending,
      bool define_sort_type = true) {
    sorts_.emplace_back(field_name, table_alias, sort_type, define_sort_type,
                        level_);

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

  NvSelect<TParameterType>& EndOrderByBlock() {
    if (!parent_)
      throw std::runtime_error(
          "null-reference to parent of NvSelect<TParameterType>");

    return *parent_;
  }

 private:
  NvSelect<TParameterType>* parent_;
  std::vector<OrderByClause> sorts_;
  uint32_t level_;
};

}  // namespace nvm::sqlbuilder