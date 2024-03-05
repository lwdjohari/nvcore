#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

namespace nvm {
namespace strings {

template <typename T>
std::pair<double, std::string> ConvertBytesToReadableSize(T bytes) {
  static_assert(
      std::is_same<T, size_t>::value || std::is_same<T, std::streamsize>::value,
      "Template parameter T must be either size_t or std::streamsize.");

  if (bytes == 0) {
    return {0, "Bytes"};
  }

  static const char* units[] = {"Bytes", "KB", "MB", "GB", "TB"};

  double size = bytes;
  int unit_index = 0;

  while (size >= 1024) {
    size /= 1024;
    unit_index++;
  }

  return {size, units[unit_index]};
}

template <typename T>
std::string ConvertBytesToReadableSizeString(T bytes, uint8_t precision = 2) {
  auto result = ConvertBytesToReadableSize(bytes);

  std::ostringstream out;
  out << std::fixed << std::setprecision(precision) << result.first << " "
      << result.second;
  return out.str();
};

}  // namespace strings
}  // namespace nvm
