#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>
#include <set>

#include "nvm/containers/record_def.h"
namespace nvm::containers {


template <typename TParamaterType = DefaultPostgresParamType>
class RecordInsert {
 private:
  std::string table_name_;
  std::ostringstream columns_;
  std::ostringstream sstr_;
  std::ostringstream returning_clause_;
  uint32_t current_param_index_;
  std::shared_ptr<std::vector<TParamaterType>> values_;
  std::set<std::string> column_names_;

 public:
  explicit RecordInsert(
      const std::string& table_name,
      uint32_t start_param_index = 1,
      std::shared_ptr<std::vector<TParamaterType>> parameter_values = 
          std::make_shared<std::vector<TParamaterType>>())
      : table_name_(table_name),
        current_param_index_(start_param_index),
        values_(parameter_values) {}

  template <typename T>
  RecordInsert<TParamaterType>& AddValue(const std::string& column_name, const T& value) {
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

  RecordInsert<TParamaterType>& AddReturning(const std::string& column_name) {
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

  std::shared_ptr<std::vector<TParamaterType>> Values() const { return values_; }

  std::set<std::string> Columns() const { return column_names_; }
};

template <typename TParamaterType = DefaultPostgresParamType>
class RecordUpdate {
 private:
  std::string table_name_;
  std::ostringstream set_clause_;
  std::ostringstream where_clause_;
  std::ostringstream returning_clause_;
  uint32_t current_param_index_;
  std::shared_ptr<std::vector<TParamaterType>> values_;
  std::set<std::string> column_names_;

 public:
  explicit RecordUpdate(
      const std::string& table_name,
      uint32_t start_param_index = 1,
      std::shared_ptr<std::vector<TParamaterType>> parameter_values = 
          std::make_shared<std::vector<TParamaterType>>())
      : table_name_(table_name),
        current_param_index_(start_param_index),
        values_(parameter_values) {}

  template <typename T>
  RecordUpdate<TParamaterType>& SetValue(const std::string& column_name, const T& value) {
    if (!set_clause_.str().empty()) {
      set_clause_ << ", ";
    }
    set_clause_ << column_name << " = $" << current_param_index_++;
    values_->push_back(value);
    column_names_.insert(column_name);
    return *this;
  }

  template <typename T>
  RecordUpdate<TParamaterType>& AddCondition(const std::string& field_name,
                                             SqlOperator op, const T& value) {
    if (!where_clause_.str().empty()) {
      where_clause_ << " AND ";
    }
    where_clause_ << field_name << " " << SqlOperatorToString(op) << " $" << current_param_index_++;
    values_->push_back(value);
    return *this;
  }

  RecordUpdate<TParamaterType>& AddReturning(const std::string& column_name) {
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

  std::shared_ptr<std::vector<TParamaterType>> Values() const { return values_; }

  std::set<std::string> Columns() const { return column_names_; }
};

template <typename TParamaterType = DefaultPostgresParamType>
class RecordDelete {
 private:
  std::string table_name_;
  std::ostringstream where_clause_;
  std::ostringstream returning_clause_;
  uint32_t current_param_index_;
  std::shared_ptr<std::vector<TParamaterType>> values_;
  std::set<std::string> column_names_;

 public:
  explicit RecordDelete(
      const std::string& table_name,
      uint32_t start_param_index = 1,
      std::shared_ptr<std::vector<TParamaterType>> parameter_values = 
          std::make_shared<std::vector<TParamaterType>>())
      : table_name_(table_name),
        current_param_index_(start_param_index),
        values_(parameter_values) {}

  template <typename T>
  RecordDelete<TParamaterType>& AddCondition(const std::string& field_name,
                                             SqlOperator op, const T& value) {
    if (!where_clause_.str().empty()) {
      where_clause_ << " AND ";
    }
    where_clause_ << field_name << " " << SqlOperatorToString(op) << " $" << current_param_index_++;
    values_->push_back(value);
    column_names_.insert(field_name);
    return *this;
  }

  RecordDelete<TParamaterType>& AddReturning(const std::string& column_name) {
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

  std::shared_ptr<std::vector<TParamaterType>> Values() const { return values_; }

  std::set<std::string> Columns() const { return column_names_; }
};

}  // namespace nvm::containers
