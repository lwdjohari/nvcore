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

#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "nvm/sqlbuilder/def.h"
namespace nvm::sqlbuilder {

template <typename TParameterType = DefaultPostgresParamType>
class RecordInsert {
 private:
  std::string table_name_;
  std::ostringstream columns_;
  std::ostringstream sstr_;
  std::ostringstream returning_clause_;
  uint32_t current_param_index_;
  std::shared_ptr<std::vector<TParameterType>> values_;
  std::set<std::string> column_names_;

 public:
  explicit RecordInsert(
      const std::string& table_name, uint32_t start_param_index = 1,
      std::shared_ptr<std::vector<TParameterType>> parameter_values =
          std::make_shared<std::vector<TParameterType>>())
                  : table_name_(table_name),
                    current_param_index_(start_param_index),
                    values_(parameter_values) {}

  template <typename T>
  RecordInsert<TParameterType>& AddValue(const std::string& column_name,
                                         const T& value) {
    if (!columns_.str().empty()) {
      columns_ << ", ";
      sstr_ << ", ";
    }
    columns_ << column_name;
    sstr_ << "$" << current_param_index_++;
    values_->push_back(value);
    column_names_.insert(column_name);
    return *this;
  }

  RecordInsert<TParameterType>& AddReturning(const std::string& column_name) {
    if (!returning_clause_.str().empty()) {
      returning_clause_ << ", ";
    }
    returning_clause_ << column_name;
    return *this;
  }

  std::string ToString() const {
    std::ostringstream ss;
    ss << "INSERT INTO " << table_name_ << " (" << columns_.str()
       << ") VALUES (" << sstr_.str() << ")";
    if (!returning_clause_.str().empty()) {
      ss << " RETURNING " << returning_clause_.str();
    }
    return ss.str();
  }

  std::shared_ptr<std::vector<TParameterType>> Values() const {
    return values_;
  }

  std::set<std::string> Columns() const {
    return column_names_;
  }
};

template <typename TParameterType = DefaultPostgresParamType>
class RecordUpdate {
 private:
  std::string table_name_;
  std::ostringstream set_clause_;
  std::ostringstream where_clause_;
  std::ostringstream returning_clause_;
  uint32_t current_param_index_;
  std::shared_ptr<std::vector<TParameterType>> values_;
  std::set<std::string> column_names_;

 public:
  explicit RecordUpdate(
      const std::string& table_name, uint32_t start_param_index = 1,
      std::shared_ptr<std::vector<TParameterType>> parameter_values =
          std::make_shared<std::vector<TParameterType>>())
                  : table_name_(table_name),
                    current_param_index_(start_param_index),
                    values_(parameter_values) {}

  template <typename T>
  RecordUpdate<TParameterType>& SetValue(const std::string& column_name,
                                         const T& value) {
    if (!set_clause_.str().empty()) {
      set_clause_ << ", ";
    }
    set_clause_ << column_name << " = $" << current_param_index_++;
    values_->push_back(value);
    column_names_.insert(column_name);
    return *this;
  }

  template <typename T>
  RecordUpdate<TParameterType>& AddCondition(const std::string& field_name,
                                             SqlOperator op, const T& value) {
    if (!where_clause_.str().empty()) {
      where_clause_ << " AND ";
    }

    where_clause_ << field_name << " " << SqlOperatorToString(op) << " $"
                  << current_param_index_++;
    values_->push_back(value);
    return *this;
  }

  RecordUpdate<TParameterType>& AddReturning(const std::string& column_name) {
    if (!returning_clause_.str().empty()) {
      returning_clause_ << ", ";
    }

    returning_clause_ << column_name;
    return *this;
  }

  std::string ToString() const {
    std::ostringstream ss;
    ss << "UPDATE " << table_name_ << " SET " << set_clause_.str();

    if (!where_clause_.str().empty()) {
      ss << " WHERE " << where_clause_.str();
    }

    if (!returning_clause_.str().empty()) {
      ss << " RETURNING " << returning_clause_.str();
    }

    return ss.str();
  }

  std::shared_ptr<std::vector<TParameterType>> Values() const {
    return values_;
  }

  std::set<std::string> Columns() const {
    return column_names_;
  }
};

template <typename TParameterType = DefaultPostgresParamType>
class RecordDelete {
 private:
  std::string table_name_;
  std::ostringstream where_clause_;
  std::ostringstream returning_clause_;
  uint32_t current_param_index_;
  std::shared_ptr<std::vector<TParameterType>> values_;
  std::set<std::string> column_names_;

 public:
  explicit RecordDelete(
      const std::string& table_name, uint32_t start_param_index = 1,
      std::shared_ptr<std::vector<TParameterType>> parameter_values =
          std::make_shared<std::vector<TParameterType>>())
                  : table_name_(table_name),
                    current_param_index_(start_param_index),
                    values_(parameter_values) {}

  template <typename T>
  RecordDelete<TParameterType>& AddCondition(const std::string& field_name,
                                             SqlOperator op, const T& value) {
    if (!where_clause_.str().empty()) {
      where_clause_ << " AND ";
    }

    where_clause_ << field_name << " " << SqlOperatorToString(op) << " $"
                  << current_param_index_++;
    values_->push_back(value);
    column_names_.insert(field_name);

    return *this;
  }

  RecordDelete<TParameterType>& AddReturning(const std::string& column_name) {
    if (!returning_clause_.str().empty()) {
      returning_clause_ << ", ";
    }

    returning_clause_ << column_name;
    return *this;
  }

  std::string ToString() const {
    std::ostringstream ss;
    ss << "DELETE FROM " << table_name_;

    if (!where_clause_.str().empty()) {
      ss << " WHERE " << where_clause_.str();
    }

    if (!returning_clause_.str().empty()) {
      ss << " RETURNING " << returning_clause_.str();
    }

    return ss.str();
  }

  std::shared_ptr<std::vector<TParameterType>> Values() const {
    return values_;
  }

  std::set<std::string> Columns() const {
    return column_names_;
  }
};

}  // namespace nvm::sqlbuilder
