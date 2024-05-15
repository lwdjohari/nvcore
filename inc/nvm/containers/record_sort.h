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

#ifndef NVM_CORE_CONTAINERS_V2_RECORD_SORT_H
#define NVM_CORE_CONTAINERS_V2_RECORD_SORT_H

#include <nvm/macro.h>

#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>

namespace nvm::containers {

struct RecordTable {
  std::string name;
  std::optional<std::string> alias;

  explicit RecordTable(const std::string& name,
                       std::optional<std::string> alias = std::nullopt)
      : name(std::string(name)), alias(alias) {}
};

enum class FieldPinMode : uint16_t { None = 0, Beginning = 1, End = 2 };

// cppcheck-suppress unknownMacro
NVM_ENUM_CLASS_DISPLAY_TRAIT(FieldPinMode)

enum class SortType : uint16_t { Ascending = 0, Descending = 1 };

NVM_ENUM_CLASS_DISPLAY_TRAIT(SortType)

struct SortField {
  std::shared_ptr<RecordTable> table_;
  std::string name;
  SortType sort_type;
  FieldPinMode pin_mode;

  explicit SortField(
      const std::string& field_name, SortType sort_type,
      const std::string& table_name,
      const std::optional<std::string>& table_alias = std::nullopt,
      FieldPinMode pin_mode = FieldPinMode::None)
      : table_(std::make_shared<RecordTable>(table_name, table_alias)),
        name(std::string(field_name)),
        sort_type(sort_type),
        pin_mode(pin_mode) {}

  explicit SortField(const std::string& field_name, SortType sort_type,
                     FieldPinMode pin_mode = FieldPinMode::None,
                     std::shared_ptr<RecordTable> table = nullptr)
      : table_(table),
        name(std::string(field_name)),
        sort_type(sort_type),
        pin_mode(pin_mode) {}
};

class RecordSort final {
 private:
  std::unordered_map<std::string, SortField> fields_;
  std::unordered_map<std::string, SortField> pinned_fields_;

 public:
  RecordSort() : fields_(), pinned_fields_(){};
  ~RecordSort(){};

  bool IsFieldExist(const std::string& key) const {
    return fields_.find(key) != fields_.end();
  }

  bool IsFieldExistOnPinned(const std::string& key) const {
    return pinned_fields_.find(key) != pinned_fields_.end();
  }

  bool AddField(const std::string& key, const SortField& field) {
    if (IsFieldExist(key) || IsFieldExistOnPinned(key)) return false;

    if (field.pin_mode != FieldPinMode::None) {
      pinned_fields_.emplace(key, field);
    }

    fields_.emplace(key, field);
    return true;
  }

  bool RemoveField(const std::string& key) {
    auto is_exist = IsFieldExist(key);
    auto is_exist_on_pinned = IsFieldExistOnPinned(key);

    if (!is_exist && !is_exist_on_pinned) {
      return false;
    }

    if (is_exist_on_pinned) {
      pinned_fields_.erase(key);
    }

    if (is_exist) {
      fields_.erase(key);
    }

    return true;
  }

  void Clear() {
    pinned_fields_.clear();
    fields_.clear();
  }

  bool IsEmpty() const { return fields_.empty(); }

  const std::unordered_map<std::string, SortField> Fields() const {
    return fields_;
  }

  const std::unordered_map<std::string, SortField> PinnedFields() const {
    return pinned_fields_;
  }

  std::string GenerateQuery() const {
    std::vector<std::string> start_pinned;
    std::vector<std::string> normal_fields;
    std::vector<std::string> end_pinned;

    if (fields_.empty() && pinned_fields_.empty()) {
      return std::string();
    }

    auto add_field = [](const std::pair<const std::string, SortField>& pair,
                        std::vector<std::string>& container) {
      const auto& field = pair.second;
      std::string sort_expression = field.table_->alias
                                        ? *(field.table_->alias) + "."
                                        : field.table_->name + ".";
      sort_expression += field.name;
      sort_expression +=
          (field.sort_type == SortType::Ascending ? " ASC" : " DESC");
      container.push_back(sort_expression);
    };

    for (const auto& pair : pinned_fields_) {
      if (pair.second.pin_mode == FieldPinMode::Beginning) {
        add_field(pair, start_pinned);
      } else {
        add_field(pair, end_pinned);
      }
    }

    for (const auto& pair : fields_) {
      add_field(pair, normal_fields);
    }

    std::vector<std::string> all_parts;
    all_parts.insert(all_parts.end(), start_pinned.begin(), start_pinned.end());
    all_parts.insert(all_parts.end(), normal_fields.begin(),
                     normal_fields.end());
    all_parts.insert(all_parts.end(), end_pinned.begin(), end_pinned.end());

    std::ostringstream query;
    query << "ORDER BY ";
    for (size_t i = 0; i < all_parts.size(); ++i) {
      if (i > 0) query << ", ";
      query << all_parts[i];
    }

    return query.str();
  }
};

}  // namespace nvm::containers
#endif