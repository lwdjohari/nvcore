#pragma once

#include "nvm/sqlbuilder/policy/policy_base.h"

namespace nvm::sqlbuilder::policy {

using OrderByParameterPair = std::tuple<std::string, FieldPart, SortType>;

template <typename TParameterType = DefaultPostgresParamType>
class OrderByPolicyParameter final : public PolicyBase {
 private:
  std::unordered_map<std::string, SortType> sorts_;

 public:
  OrderByPolicyParameter() : PolicyBase() {}

  static OrderByParameterPair CreateParameterPair(const std::string& key,
                                                  const FieldPart& part,
                                                  SortType sort_type) {
    return std::make_tuple<std::string, FieldPart>(
        std::string(key), std::move(FieldPart(part)), sort_type);
  }

  void AddParameters(const std::vector<OrderByParameterPair>& parameters) {
    for (auto& param : parameters) {
      auto key = std::string(std::get<0>(param));
      auto part = FieldPart(std::get<1>(param));
      auto sort_type = std::get<2>(param);

      auto state = AddPart(std::string(key), std::move(part));
      if (state) {
        sorts_.emplace(std::move(key), sort_type);
      }
    }
  }

  void ResetParameters(){
    sorts_.clear();
    ResetParts();
  }

  virtual void Reset() override{
    sorts_.clear();
    PolicyBase::Reset();
  }
};

}  // namespace nvm::sqlbuilder::policy
