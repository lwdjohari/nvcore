#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>

#include "catch2/catch_all.hpp"
#include "nvm/sqlbuilder/policy/order_by.h"

using namespace nvm;

TEST_CASE("nvfilter-order_by_part", "[nvfilter][order-by]") {
  using OrderByParameter = nvm::sqlbuilder::policy::OrderByPolicyParameter<>;
  using PolicyRuleMode = nvm::sqlbuilder::policy::PolicyRuleMode;
  using FieldPart = nvm::sqlbuilder::FieldPart;
  using SortType = nvm::sqlbuilder::SortType;

  OrderByParameter filter;

  filter.AddRules({
      OrderByParameter::CreateRulePair(
          "company_name", FieldPart("company_name"), PolicyRuleMode::Allowed),
      OrderByParameter::CreateRulePair("equipment_name",
                                       FieldPart("equipment_name"),
                                       PolicyRuleMode::Allowed),
      OrderByParameter::CreateRulePair("equipment_type_name",
                                       FieldPart("equipment_type_name"),
                                       PolicyRuleMode::Allowed),
      OrderByParameter::CreateRulePair("brand", FieldPart("brand"),
                                       PolicyRuleMode::Allowed),
  });

  filter.RuleMode(PolicyRuleMode::Allowed);

  filter.AddParameters(
      {OrderByParameter::CreateParameterPair(
           "company_name", FieldPart("company_name"), SortType::Ascending),
       OrderByParameter::CreateParameterPair("equipment_name",
                                             FieldPart("equipment_name"),
                                             SortType::Ascending)});

  

  // REQUIRE(result.is_valid == true);
}
