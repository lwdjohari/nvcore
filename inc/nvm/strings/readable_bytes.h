#pragma once

#include <iostream>
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
    return { 0, "Bytes" };
  }

  static const char* units[] = {"Bytes", "KB", "MB", "GB", "TB"};

  double size = bytes;
  int unit_index = 0;

  while (size >= 1024) {
    size /= 1024;
  }

  return { size, units[unit_index] };
}

}  // namespace strings
}  // namespace nvm
