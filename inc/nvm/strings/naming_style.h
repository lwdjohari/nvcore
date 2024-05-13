/*
 *  Copyright (c) 2022 Linggawasistha Djohari
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

#ifndef NVM_CORE_V2_NAMING_STYLE_H
#define NVM_CORE_V2_NAMING_STYLE_H

#include <iostream>
#include <string>

#include "nvm/macro.h"
#include "nvm/strings/utility.h"

namespace nvm {
namespace strings {
namespace naming_style {

enum class NamingStyleType {
  Unknown = 0,
  PascalCase = 1,
  CamelCase = 2,
  SnakeCase = 3,
};

// cppcheck-suppress unknownMacro
NVM_ENUM_CLASS_DISPLAY_TRAIT(NamingStyleType)

inline std::string FromPascalCaseToCamelCase(const std::string &input) {
  std::string result;
  bool first_char = true;
  bool ignore = true;

  for (char c : input) {
    if (c == '_') {
      continue;
    }

    if (isupper(c)) {
      if (first_char) {
        result += static_cast<char>(tolower(c));
      } else {
        result += c;
      }
    } else {
      result += c;
    }
    first_char = false;
  }
  return std::move(result);
}

inline std::string FromCamelCaseToPascalCase(const std::string &input) {
  std::string result;
  bool first_char = true;
  bool ignore = true;

  for (char c : input) {
    if (c == '_') {
      continue;
    }

    if (islower(c)) {
      if (first_char) {
        result += static_cast<char>(toupper(c));
      } else {
        result += c;
      }
    } else {
      result += c;
    }
    first_char = false;
  }
  return std::move(result);
}

inline std::string FromSnakeCaseToPascalCase(const std::string &input) {
  std::string result;
  bool capitalize_next_char = true;
  for (char c : input) {
    if (c == '_') {
      capitalize_next_char = true;
    } else {
      if (capitalize_next_char) {
        result += static_cast<char>(toupper(c));
        capitalize_next_char = false;
      } else {
        result += c;
      }
    }
  }
  return std::move(result);
}

inline std::string FromCamelCaseToSnakeCase(const std::string &input) {
  std::string result;
  for (char c : input) {
    if (isupper(c)) {
      result += '_';
      result += static_cast<char>(tolower(c));
    } else {
      result += c;
    }
  }
  return std::move(result);
}

inline NamingStyleType DetectNamingConvention(const std::string &str) {
  if (str.empty()) {
    return NamingStyleType::Unknown;
  }

  bool uppercase_found = false;
  bool lowercase_found = false;
  bool underscore_found = false;
  bool first_char = true;
  bool first_char_is_lower = false;
  // bool capatalizeNext = true;

  for (size_t i = 0; i < str.length(); ++i) {
    char c = str[i];

    if (isupper(c) != 0) {
      uppercase_found = true;
      if (first_char) {
        first_char_is_lower = false;
        first_char = false;
      }
    } else if (islower(c) != 0) {
      lowercase_found = true;
      if (first_char) {
        first_char_is_lower = true;
        first_char = false;
      }
    } else if (c == '_') {
      underscore_found = true;
    } else {
      // If a character is not uppercase, lowercase, or underscore, it's not a
      // valid naming convention
      return NamingStyleType::Unknown;
    }
  }

  if (underscore_found && !uppercase_found && lowercase_found) {
    // If only underscores are found, it's snake_case
    return NamingStyleType::SnakeCase;
  } else if (first_char_is_lower && uppercase_found && lowercase_found) {
    // If both uppercase and lowercase characters are found, it's camelCase
    return NamingStyleType::CamelCase;
  } else if (!first_char_is_lower && uppercase_found && lowercase_found) {
    // If first is uppercase characters are found , it's PascalCase
    return NamingStyleType::PascalCase;
  } else {
    // If none of the above conditions are met, it's not a valid naming
    // convention
    return NamingStyleType::Unknown;
  }
}

inline std::string ToSnakeCase(const std::string &str) {
  auto convention = DetectNamingConvention(str);

  if (convention == NamingStyleType::CamelCase) {
    return std::move(FromCamelCaseToSnakeCase(str));
  } else if (convention == NamingStyleType::PascalCase) {
    return std::move(FromCamelCaseToSnakeCase(FromPascalCaseToCamelCase(str)));
  } else {
    return str;
  }
}

inline std::string ToCamelCase(const std::string &str) {
  auto convention = DetectNamingConvention(str);

  if (convention == NamingStyleType::PascalCase) {
    return std::move(FromPascalCaseToCamelCase(str));
  } else if (convention == NamingStyleType::SnakeCase) {
    return std::move(FromPascalCaseToCamelCase(FromSnakeCaseToPascalCase(str)));
  } else {
    return str;
  }
}

inline std::string ToPascalCase(const std::string &str) {
  auto convention = DetectNamingConvention(str);

  if (convention == NamingStyleType::CamelCase) {
    return std::move(FromCamelCaseToPascalCase(str));
  } else if (convention == NamingStyleType::SnakeCase) {
    return std::move(FromSnakeCaseToPascalCase(str));
  } else {
    return str;
  }
}

inline std::string ConvertStyle(const NamingStyleType &destinationConvention,
                                const std::string &value) {
  if (destinationConvention == NamingStyleType::CamelCase) {
    return std::move(ToCamelCase(value));
  } else if (destinationConvention == NamingStyleType::PascalCase) {
    return std::move(ToPascalCase(value));
  } else if (destinationConvention == NamingStyleType::SnakeCase) {
    return std::move(ToSnakeCase(value));
  } else {
    return value;
  }
}

inline std::string ToString(const NamingStyleType &convention) {
  if (convention == NamingStyleType::CamelCase) {
    return "camelCase";
  } else if (convention == NamingStyleType::PascalCase) {
    return "pascalCase";
  } else if (convention == NamingStyleType::SnakeCase) {
    return "snakeCase";
  } else {
    return "mixCase";
  }
}

inline NamingStyleType ToEnum(const std::string &str) {
  if (utility::ToLower(str) == "camelcase") {
    return NamingStyleType::CamelCase;
  } else if (utility::ToLower(str) == "pascalcase") {
    return NamingStyleType::PascalCase;
  } else if (utility::ToLower(str) == "snakecase") {
    return NamingStyleType::SnakeCase;
  } else {
    return NamingStyleType::Unknown;
  }
}

}  // namespace naming_style
}  // namespace strings
}  // namespace nvm

#endif