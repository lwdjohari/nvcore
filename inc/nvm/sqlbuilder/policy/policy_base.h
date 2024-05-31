#pragma once

#include <unordered_map>
#include "nvm/sqlbuilder/def.h"
#include "nvm/validators/rule.h"


namespace nvm::sqlbuilder::policy
{
    enum PolicyRuleMode {
  Automatic = 0,  // If no rules it allowed all,
  Allowed = 1,    // Can be use the parts
  Unallowed = 2,  // Parts will be stripped from final strip
  DefinitionOnly =
      3  // Just use as definition, so add parts just use the field def
};

NVM_ENUM_CLASS_DISPLAY_TRAIT(PolicyRuleMode)

using PolicyRule = validators::RuleGeneric<PolicyRuleMode, FieldPart>;
using PolicyRuleMap = std::unordered_map<std::string, PolicyRule>;
using PolicyPartMap = std::unordered_map<std::string, FieldPart>;
using RulePair = std::tuple<std::string, PolicyRule>;
using PairtPair = std::tuple<std::string, FieldPart>;

class PolicyBase {
 private:
  PolicyRuleMap rule_fields_;
  PolicyPartMap part_fields_;
  PolicyRuleMode mode_;

 public:
  PolicyBase()
                  : rule_fields_(),
                    part_fields_(),
                    mode_(PolicyRuleMode::Automatic){};

  virtual void Reset() {
    part_fields_.clear();
    rule_fields_.clear();
  }

  void ResetRules() {
    rule_fields_.clear();
  }

  void ResetParts() {
    part_fields_.clear();
  }

  bool IsRuleExist(const std::string& key) const {
    return rule_fields_.find(key) != rule_fields_.end();
  }

  bool IsPartExist(const std::string& key) const {
    return part_fields_.find(key) != part_fields_.end();
  }

  bool AddRule(const std::string& key, PolicyRule&& rule) {
    if (IsRuleExist(key))
      return false;

    rule_fields_.emplace(key, std::forward<PolicyRule>(rule));
    return true;
  }

  PolicyRuleMode RuleMode() const {
    return mode_;
  }

  void RuleMode(PolicyRuleMode rule) {
    mode_ = rule;
  }

  bool AddRules(const std::vector<RulePair>& rules) {
    if (rules.size() == 0)
      return false;

    size_t index = 0;
    for (auto& rule : rules) {
      auto key = std::string(std::get<0>(rule));
      auto part = PolicyRule(std::get<1>(rule));

      AddRule(std::move(key), std::move(part));
      index++;
    }

    return true;
  }

  bool RemoveRule(const std::string& key, PolicyRule&& rule) {
    if (!IsRuleExist(key))
      return false;

    rule_fields_.erase(key);
    return true;
  }

  /// @brief Add parts by reusing field from rules
  /// @param key
  /// @return
  bool AddPart(const std::string& key) {
    if (IsRuleExist(key) || IsPartExist(key))
      return false;

    auto rule = &rule_fields_.at(key);
    auto fp = FieldPart(rule->Value().value());

    part_fields_.emplace(key, fp);
    return true;
  }

  /// @brief Add parts by reusing field from rules
  /// @param keys
  void AddParts(const std::vector<std::string>& keys) {
    for (auto& key : keys) {
      AddPart(key);
    }
  }

  /// @brief Quick add parts by vector of FieldPartTuple.
  /// @param parts add field part quickly by using vector of tuple<std::string,
  /// std::string, std::string> with format of <key,name, alias>.
  void AddParts(const std::vector<PairtPair>& parts) {
    for (auto& tpart : parts) {
      auto key = std::string(std::get<0>(tpart));
      auto part = FieldPart(std::get<1>(tpart));

      AddPart(key, std::move(part));
    }
  }

  bool AddPart(const std::string& key, FieldPart&& part) {
    if (IsPartExist(key))
      return false;

    part_fields_.emplace(key, std::forward<FieldPart>(part));
    return true;
  }

  bool RemovePart(const std::string& key, PolicyRule&& rule) {
    if (!IsPartExist(key))
      return false;

    part_fields_.erase(key);
    return true;
  }

  const PolicyPartMap& Parts() const {
    return part_fields_;
  }

  const PolicyRuleMap& Rules() const {
    return rule_fields_;
  }

  static RulePair CreateRulePair(const std::string& key,
                                        FieldPart&& part,
                                        PolicyRuleMode rule_type) {
    return std::make_tuple<std::string, PolicyRule>(
        std::string(key),
        std::move<PolicyRule>(PolicyRule(std::forward<FieldPart>(part),
                                           PolicyRuleMode::Allowed)));
  }

  static PairtPair CreatePartPair(const std::string& key,
                                        const FieldPart& part) {
    return std::make_tuple<std::string, FieldPart>(std::string(key),
                                                   std::move(FieldPart(part)));
  }
};
} // namespace nvm::sqlbuilder::policy
