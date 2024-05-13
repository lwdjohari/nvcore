/*
 *  Copyright (c) 2020 Linggawasistha Djohari
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

#ifndef NVM_CORE_STRINGS_V2_UTILITY_H
#define NVM_CORE_STRINGS_V2_UTILITY_H

#include <algorithm>
#include <sstream>
#include <string>

namespace nvm {
namespace strings {
namespace utility {

/// @brief
/// @param s
/// @param t
/// @return
inline int GetLevenshteinDistance(const std::string &s, const std::string &t) {
  if (s.empty()) return t.size();
  if (t.empty()) return s.size();

  size_t n = s.size();
  size_t m = t.size();

  if (n > m) {
    // Swap the strings to consume less memory
    return GetLevenshteinDistance(t, s);
  }

  std::vector<int> p(n + 1);

  // Initialize the first row
  for (size_t i = 0; i <= n; i++) {
    p[i] = static_cast<int>(i);
  }

  int upper;
  int upper_left;  // cppcheck-suppress variableScope
  int cost;

  for (size_t j = 1; j <= m; j++) {
    upper_left = p[0];
    p[0] = static_cast<int>(j);

    for (size_t i = 1; i <= n; i++) {
      upper = p[i];
      cost = (s[i - 1] == t[j - 1]) ? 0 : 1;
      p[i] = std::min({p[i - 1] + 1, p[i] + 1, upper_left + cost});
      upper_left = upper;
    }
  }

  return p[n];
};

/// @brief
/// @param str
/// @return
inline std::string Trim(const std::string &str) {
  // Find the first non-whitespace character
  const auto start = str.find_first_not_of(" \t\r\n");

  // If the string is all whitespace, return an empty string
  if (start == std::string::npos) {
    return std::move("");
  }

  // Find the last non-whitespace character
  const auto end = str.find_last_not_of(" \t\r\n");

  // Extract the trimmed string using a stringstream
  std::stringstream ss;
  ss << str.substr(start, end - start + 1);

  return std::move(ss.str());
};

/// @brief
/// @param c
/// @return
inline bool IsWhitespaceChar(char c) {
  return std::isspace(static_cast<unsigned char>(c));
};

/// @brief
/// @param str
/// @return
inline bool IsWhitespaceString(const std::string &str) {
  return str.find_first_not_of(" \t\n\v\f\r") == std::string::npos;
};

/// @brief
/// @param str
/// @return
inline std::string ToUpper(const std::string &str) {
  std::string newstr = std::string(str);
  std::transform(newstr.begin(), newstr.end(), newstr.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return std::move(newstr);
};

/// @brief
/// @param str
/// @return
inline std::string ToLower(const std::string &str) {
  std::string newstr = std::string(str);
  std::transform(newstr.begin(), newstr.end(), newstr.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return std::move(newstr);
};

}  // namespace utility
}  // namespace strings
}  // namespace nvm

#endif  // NVM_CORE_STRINGS_V2_UTILITY_H
