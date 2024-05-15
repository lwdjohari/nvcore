#ifndef NVM_CORE_CONTAINERS_V2_RECORD_DEF_H
#define NVM_CORE_CONTAINERS_V2_RECORD_DEF_H

#include <nvm/macro.h>

#include <cstdint>
#include <memory>
#include <optional>

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
}  // namespace nvm::containers

#endif