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

#include <cctype>
#include <functional>
#include <iostream>
#include <optional>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include "nvm/macro.h"
#include "nvm/types/type_utility.h"

namespace nvm::containers::validations {

enum class ValidationOperator {
  kEqual,
  kNotEqual,
  kLess,
  kLessOrEqual,
  kGreater,
  kGreaterOrEqual
};

// cppcheck-suppress unknownMacro
NVM_ENUM_CLASS_DISPLAY_TRAIT(ValidationOperator)

enum class ValidationConstraint {
  // clang-format off
  Strict,                       /// Must have value and passed the validation rule, otherwise will produce validation error
  ValidOnNull,                  /// If Null will be resulting validation success,
  ValidOnNullOrEmpty,           /// If Null or Empty will be resulting validation success.
  ValidOnNullEmptyOrWhiteSpace  /// If Null or Empty or Whitespace will be resulting validation success.
  // clang-format on
};

NVM_ENUM_CLASS_DISPLAY_TRAIT(ValidationConstraint)

struct ValidationError {
  std::string key;
  std::optional<int> error_code;
  std::string message;

  ValidationError(std::string key, std::optional<int> error_code,
                  std::string message) noexcept
      : key(std::move(key)),
        error_code(error_code),
        message(std::move(message)) {}
};

class ValidationResult {
 public:
  bool is_valid = true;
  std::vector<ValidationError> errors;

  void AddError(const std::string& key, std::optional<int> error_code,
                const std::string& message) noexcept {
    is_valid = false;
    try {
      errors.emplace_back(key, error_code, message);
    } catch (...) {
      // If emplace_back throws, we swallow the exception to ensure no
      // exceptions are propagated.
    }
  }

  std::string GetErrorAsString() const {
    std::stringstream state;
    for (const auto& error : errors) {
      state << "Error in " << error.key << ": " << error.message
            << " (Error code: " << error.error_code.value_or(0) << ")" << "\n";
    }
    return state.str();
  }
};

template <typename T>
class Validator {
 public:
  Validator(const std::string& key, const T& value,
            std::queue<std::function<void()>>& tasks, ValidationResult& result,
            bool must_be_valid) noexcept
      : key_(key),
        value_(value),
        tasks_(tasks),
        result_(result),
        must_be_valid_(must_be_valid) {}

  Validator& IsNotNull(
      std::optional<int> error_code = std::nullopt,
      const std::string& message = "Value must not be null") noexcept {
    NVM_ASSERT_VALID_NULLABLE_TYPE(T)
    tasks_.emplace([this, error_code, message]() noexcept {
      bool is_empty = true;
      if (must_be_valid_) {
        if constexpr (std::is_same_v<T, std::optional<T>>) {
          is_empty = !value_.has_value();
        } else if constexpr (nvm::types::utility::is_smart_ptr_v<T>()) {
          is_empty = !value_;
        }

        if (is_empty) {
          result_.AddError(key_, error_code, message);
        }
      }
    });
    return *this;
  }

  Validator& IsNotEmpty(
      std::optional<int> error_code = std::nullopt,
      const std::string& message = "Value must not be empty") noexcept {
    NVM_ASSERT_VALID_NULLABLE_OR_EMPTY_TYPE(T)
    tasks_.emplace([this, error_code, message]() noexcept {
      bool is_empty = true;
      if (must_be_valid_) {
        if constexpr (nvm::types::utility::is_string_and_string_view_v<T>()) {
          auto is_empty = value_.empty();

        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_optional_v<T>()) {
          is_empty = !value_.has_value() || value_->empty();
        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_smart_ptr_v<T>()) {
          is_empty = !value_ || value_->empty();
        } else if constexpr (std::is_pointer_v<T>) {
          is_empty = !value_;
        }

        if (is_empty) {
          result_.AddError(key_, error_code, message);
        }
      }
    });
    return *this;
  }

  Validator& IsNotEmptyNullOrWhiteSpace(
      std::optional<int> error_code = std::nullopt,
      const std::string& message =
          "Value must not be empty, null, or whitespace") noexcept {
    NVM_ASSERT_VALID_NULLABLE_OR_EMPTY_TYPE(T)
    tasks_.emplace([this, error_code, message]() noexcept {
      if (must_be_valid_) {
        bool is_empty_null_or_whitespace = false;
        if constexpr (nvm::types::utility::is_string_and_string_view_v<T>()) {
          is_empty_null_or_whitespace =
              value_.empty() ||
              std::all_of(value_.begin(), value_.end(), ::isspace);
        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_optional_v<T>()) {
          is_empty_null_or_whitespace =
              !value_.has_value() || value_->empty() ||
              std::all_of(value_->begin(), value_->end(), ::isspace);
        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_smart_ptr_v<T>()) {
          is_empty_null_or_whitespace =
              !value_ || value_->empty() ||
              std::all_of(value_->begin(), value_->end(), ::isspace);
        } else if constexpr (nvm::types::utility::is_smart_ptr_v<T>) {
          is_empty_null_or_whitespace = !value_;
        }

        if (is_empty_null_or_whitespace) {
          result_.AddError(key_, error_code, message);
        }
      }
    });
    return *this;
  }

  Validator& IsLength(
      size_t min, size_t max, std::optional<int> error_code = std::nullopt,
      const std::string& message = "Value length is out of range") noexcept {
    NVM_ASSERT_VALID_STRING_SPTR_TYPE(T)
    if constexpr (nvm::types::utility::is_string_and_string_view_v<T>()) {
      tasks_.emplace([this, min, max, error_code, message]() noexcept {
        if (must_be_valid_ &&
            (value_.length() < min || value_.length() > max)) {
          result_.AddError(key_, error_code, message);
        }
      });
    } else if constexpr (nvm::types::utility::
                             is_string_and_string_view_optional_v<T>()) {
      tasks_.emplace([this, min, max, error_code, message]() noexcept {
        if (must_be_valid_ && value_.has_value() &&
            (value_->length() < min || value_->length() > max)) {
          result_.AddError(key_, error_code, message);
        }
      });
    } else if constexpr (nvm::types::utility::
                             is_string_and_string_view_smart_ptr_v<T>()) {
      tasks_.emplace([this, min, max, error_code, message]() noexcept {
        if (must_be_valid_ &&
            (!value_ || (value_->length() < min || value_->length() > max))) {
          result_.AddError(key_, error_code, message);
        }
      });
    }
    return *this;
  }

  Validator& IsEqual(const T& other,
                     std::optional<int> error_code = std::nullopt,
                     const std::string& message =
                         "Value does not match the condition") noexcept {
    NVM_ASSERT_VALID_LOGICAL_EQ_NEQ_TYPE(T)
    return Is(ValidationOperator::kEqual, other, error_code, message);
  }

  Validator& IsNotEqual(const T& other,
                        std::optional<int> error_code = std::nullopt,
                        const std::string& message =
                            "Value does not match the condition") noexcept {
    NVM_ASSERT_VALID_LOGICAL_EQ_NEQ_TYPE(T)
    return Is(ValidationOperator::kNotEqual, other, error_code, message);
  }

  Validator& IsLessThan(const T& other,
                        std::optional<int> error_code = std::nullopt,
                        const std::string& message =
                            "Value does not match the condition") noexcept {
    NVM_ASSERT_VALID_LOGICAL_LT_GT_TYPE(T)
    return Is(ValidationOperator::kLess, other, error_code, message);
  }

  Validator& IsLessThanEqual(
      const T& other, std::optional<int> error_code = std::nullopt,
      const std::string& message =
          "Value does not match the condition") noexcept {
    NVM_ASSERT_VALID_LOGICAL_LTE_GTE_TYPE(T)
    return Is(ValidationOperator::kLess, other, error_code, message);
  }

  Validator& IsGreaterThan(const T& other,
                           std::optional<int> error_code = std::nullopt,
                           const std::string& message =
                               "Value does not match the condition") noexcept {
    NVM_ASSERT_VALID_LOGICAL_LT_GT_TYPE(T)
    return Is(ValidationOperator::kGreater, other, error_code, message);
  }

  Validator& IsGreaterThanEqual(
      const T& other, std::optional<int> error_code = std::nullopt,
      const std::string& message =
          "Value does not match the condition") noexcept {
    NVM_ASSERT_VALID_LOGICAL_LTE_GTE_TYPE(T)
    return Is(ValidationOperator::kGreaterOrEqual, other, error_code, message);
  }

  /// @brief TODO make it to support smart pointer that wrapping arithmatic type
  /// @param min
  /// @param max
  /// @param error_code
  /// @param message
  /// @return
  Validator& IsBetween(
      const T& min, const T& max, std::optional<int> error_code = std::nullopt,
      const std::string& message =
          "Value is not between the specified range") noexcept {
    NVM_ASSERT_VALID_LOGICAL_LT_GT_TYPE(T)
    tasks_.emplace([this, min, max, error_code, message]() noexcept {
      if (must_be_valid_ && (value_ < min || value_ > max)) {
        result_.AddError(key_, error_code, message);
      }
    });
    return *this;
  }

  Validator& IsMust(
      std::function<bool(const T&)> lambda,
      std::optional<int> error_code = std::nullopt,
      const std::string& message = "Custom validation failed") noexcept {
    tasks_.emplace([this, lambda, error_code, message]() {
      if (must_be_valid_ && !lambda(value_)) {
        result_.AddError(key_, error_code, message);
      }
    });
    return *this;
  }

  Validator& IsAlphanumeric(
      const std::vector<char>& special_chars = {},
      std::optional<int> error_code = std::nullopt,
      const std::string& message = "Value must be alphanumeric") noexcept {
    NVM_ASSERT_VALID_STRING_SPTR_TYPE(T)

    tasks_.emplace([this, special_chars, error_code, message]() noexcept {
      if (must_be_valid_) {
        bool is_alphanumeric = true;
        auto is_valid_char = [&special_chars](char c) {
          return std::isalnum(c) ||
                 std::find(special_chars.begin(), special_chars.end(), c) !=
                     special_chars.end();
        };
        if constexpr (nvm::types::utility::is_string_and_string_view_v<T>()) {
          is_alphanumeric =
              std::all_of(value_.begin(), value_.end(), is_valid_char);
        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_optional_v<T>()) {
          is_alphanumeric =
              value_.has_value() &&
              std::all_of(value_->begin(), value_->end(), is_valid_char);
        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_smart_ptr_v<T>()) {
          is_alphanumeric =
              value_ != nullptr &&
              std::all_of(value_->begin(), value_->end(), is_valid_char);
        }
        if (!is_alphanumeric) {
          result_.AddError(key_, error_code, message);
        }
      }
    });
    return *this;
  }

  Validator& IsAlphabet(
      const std::vector<char>& special_chars = {},
      std::optional<int> error_code = std::nullopt,
      const std::string& message = "Value must be alphabetic") noexcept {
    NVM_ASSERT_VALID_STRING_SPTR_TYPE(T)

    tasks_.emplace([this, special_chars, error_code, message]() noexcept {
      if (must_be_valid_) {
        bool is_alphabet = true;

        auto is_valid_char = [&special_chars](char c) {
          return std::isalpha(c) ||
                 std::find(special_chars.begin(), special_chars.end(), c) !=
                     special_chars.end();
        };

        if constexpr (nvm::types::utility::is_string_and_string_view_v<T>()) {
          is_alphabet =
              std::all_of(value_.begin(), value_.end(), is_valid_char);
        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_optional_v<T>()) {
          is_alphabet =
              value_.has_value() &&
              std::all_of(value_->begin(), value_->end(), is_valid_char);
        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_smart_ptr_v<T>()) {
          is_alphabet =
              value_ != nullptr &&
              std::all_of(value_->begin(), value_->end(), is_valid_char);
        }
        if (!is_alphabet) {
          result_.AddError(key_, error_code, message);
        }
      }
    });
    return *this;
  }

  Validator& IsEmail(const std::vector<char>& allowed_special_chars = {'.',
                                                                       '_'},
                     std::optional<int> error_code = std::nullopt,
                     const std::string& message =
                         "Value must be a valid email address") noexcept {
    NVM_ASSERT_VALID_STRING_SPTR_TYPE(T)

    tasks_.emplace([this, allowed_special_chars, error_code,
                    message]() noexcept {
      if (must_be_valid_) {
        bool is_email = true;
        std::string special_chars_pattern;
        for (char c : allowed_special_chars) {
          special_chars_pattern += "\\" + std::string(1, c);
        }

        const std::regex pattern(R"((^[a-zA-Z0-9))" + special_chars_pattern +
                                 R"(]+)@([\w\-\.]+)\.([a-zA-Z]{2,6}$))");

        //  const std::regex pattern(R"((^[\w)" + special_chars_pattern +
        //  R"(]+)@([\w\-\.]+)\.([a-zA-Z]{2,6}$))");
        if constexpr (nvm::types::utility::is_string_and_string_view_v<T>()) {
          is_email = std::regex_match(value_, pattern);
        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_optional_v<T>()) {
          is_email = value_.has_value() && std::regex_match(*value_, pattern);
        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_smart_ptr_v<T>()) {
          is_email = value_ != nullptr && std::regex_match(*value_, pattern);
        }

        if (!is_email) {
          result_.AddError(key_, error_code, message);
        }
      }
    });
    return *this;
  }

  Validator& IsNumericString(
      std::optional<int> error_code = std::nullopt,
      const std::string& message = "Value must be a numeric string") noexcept {
    NVM_ASSERT_VALID_STRING_SPTR_TYPE(T)

    tasks_.emplace([this, error_code, message]() noexcept {
      if (must_be_valid_) {
        bool is_numeric = true;
        auto is_valid_char = [](char c) {
          return std::isdigit(c) || c == '.' || c == '-' || c == '+';
        };
        if constexpr (nvm::types::utility::is_string_and_string_view_v<T>()) {
          is_numeric = std::all_of(value_.begin(), value_.end(), is_valid_char);
        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_optional_v<T>()) {
          is_numeric =
              value_.has_value() &&
              std::all_of(value_->begin(), value_->end(), is_valid_char);
        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_smart_ptr_v<T>()) {
          is_numeric =
              value_ != nullptr &&
              std::all_of(value_->begin(), value_->end(), is_valid_char);
        }
        if (!is_numeric) {
          result_.AddError(key_, error_code, message);
        }
      }
    });
    return *this;
  }

  Validator& IsRegexString(
      const std::regex& pattern, std::optional<int> error_code = std::nullopt,
      const std::string& message =
          "Value must match the specified pattern") noexcept {
    NVM_ASSERT_VALID_STRING_TYPE(T)
    tasks_.emplace([this, pattern, error_code, message]() noexcept {
      if (must_be_valid_) {
        bool is_match = true;
        if constexpr (nvm::types::utility::is_string_and_string_view_v<T>()) {
          is_match = std::regex_match(value_, pattern);
        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_optional_v<T>()) {
          is_match = value_.has_value() && std::regex_match(*value_, pattern);
        } else if constexpr (nvm::types::utility::
                                 is_string_and_string_view_smart_ptr_v<T>()) {
          is_match = is_match =
              value_ != nullptr && std::regex_match(*value_, pattern);
        }
        if (!is_match) {
          result_.AddError(key_, error_code, message);
        }
      }
    });
    return *this;
  }

 private:
  std::string key_;
  const T& value_;
  std::queue<std::function<void()>>& tasks_;
  ValidationResult& result_;
  bool must_be_valid_;

  Validator& Is(ValidationOperator op, const T& other,
                std::optional<int> error_code = std::nullopt,
                const std::string& message =
                    "Value does not match the condition") noexcept {
    tasks_.emplace([this, op, other, error_code, message]() noexcept {
      if (must_be_valid_) {
        bool condition = false;
        switch (op) {
          case ValidationOperator::kEqual:

            condition = value_ == other;
            break;
          case ValidationOperator::kNotEqual:

            condition = value_ != other;
            break;
          case ValidationOperator::kLess:

            condition = value_ < other;
            break;
          case ValidationOperator::kLessOrEqual:

            condition = value_ <= other;
            break;
          case ValidationOperator::kGreater:

            condition = value_ > other;
            break;
          case ValidationOperator::kGreaterOrEqual:

            condition = value_ >= other;
            break;
        }
        if (!condition) {
          result_.AddError(key_, error_code, message);
        }
      }
    });
    return *this;
  }
};

/// @brief NVM Fluent Validator with integrated multi chained validating process
/// & validation result support with list of error code and custom message. This
/// class designed to be fluent template based validator with general commonly
/// validation cases and flexible custom lambda support.
class NvValidator {
 public:
  NvValidator() noexcept = default;

  template <typename TVal>
  Validator<TVal> Validate(const std::string& key, const TVal& value,
                           bool must_be_valid = true,
                           std::optional<int> error_code = std::nullopt,
                           const std::string& message = "Invalid value") {
    if (!keys_.insert(key).second) {
      throw std::runtime_error("Duplicate key: " + key);
    }
    return Validator<TVal>(key, value, tasks_, result_, must_be_valid);
  }

  template <typename TVal>
  Validator<std::optional<TVal>> Validate(
      const std::string& key, const std::optional<TVal>& value,
      bool must_be_valid = true, std::optional<int> error_code = std::nullopt,
      const std::string& message = "Invalid value") {
    if (!keys_.insert(key).second) {
      throw std::runtime_error("Duplicate key: " + key);
    }
    return Validator<std::optional<TVal>>(key, value, tasks_, result_,
                                          must_be_valid);
  }

  const ValidationResult& ValidateAll() noexcept {
    while (!tasks_.empty()) {
      try {
        tasks_.front()();
      } catch (...) {
        // If a lambda throws an exception, we let it propagate.
      }
      tasks_.pop();
    }
    return result_;
  }

 private:
  std::queue<std::function<void()>> tasks_;
  ValidationResult result_;
  std::unordered_set<std::string> keys_;
};

}  // namespace nvm::containers::validations
