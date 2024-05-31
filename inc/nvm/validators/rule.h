#pragma once

#include <tuple>
#include <optional>
#include <memory>


namespace nvm::validators {
template <typename TRuleType, typename TValueType>
class RuleGeneric {
 public:
  explicit RuleGeneric(TValueType&& value, TRuleType&& rule)
                  : rule_(std::make_shared<TRuleType>(
                        std::forward<TRuleType>(rule))),
                    value_(std::make_shared<TValueType>(
                        std::forward<TValueType>(value))){};
                        
bool IsHasRuleAndValue() const{
    return rule_ && value_;
}

std::optional<TRuleType> Rule() const{
    if(!rule_)
        return std::nullopt;

    return std::optional<TRuleType>(*rule_);
}

std::optional<TValueType> Value() const{
    if(!value_)
        return std::nullopt;

    return std::optional<TValueType>(*value_);
}

 private:
  std::shared_ptr<TRuleType> rule_;
  std::shared_ptr<TValueType> value_;

};
}  // namespace nvm::validators
