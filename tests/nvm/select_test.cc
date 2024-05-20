#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>

#include "catch2/catch_all.hpp"
#include "nvm/containers/nv_select.h"
#include "nvm/strings/utility.h"
using namespace nvm;

TEST_CASE("select-join-complex-1", "[validator][normal-case]") {
  // using Validator = nvm::containers::validations::Validator;
  using DefaultPostgresParamType = nvm::containers::DefaultPostgresParamType;
  using NvSelect = nvm::containers::NvSelect<DefaultPostgresParamType>;
  using SqlOperator = nvm::containers::SqlOperator;
  using RecordKey = nvm::containers::RecordKey;
  using SqlAggregateFn = nvm::containers::SqlAggregateFunction;
  auto select = std::make_unique<NvSelect>(1);

  // clang-format off
  std::cout << "NvSelect size:" << sizeof(*select) << std::endl;
  std::cout << "NvSelect ptr-size:" << sizeof(select) << std::endl;
  
  (*select)
    .Field<int32_t>("equipment_id","e")
    .Field<int32_t>("company_id","c")
    .Field<int32_t>("service_id","s")
    .Field<int32_t>("unit_type_id","e")
    .Field<int32_t>("unit_class_id","e")
    .Field<int32_t>("unit_subclass_id","e")
    .Field<std::string>("reg_id","e")
    .Field<std::string>("code","e", "equipment_code")
    .Field<std::string>("name","c","company_name")
    .Field<std::string>("name","s","service_name")
    .Field<std::string>("code","ut","unit_type_code")
    .Field<std::string>("name","ut","unit_type_name")
    .Field<std::string>("code","uc","unit_class_code")
    .Field<std::string>("name","uc","unit_class_name")
    .Field<std::string>("code","us","unit_sub_class_code")
    .Field<std::string>("name","us", "unit_subclass_name")
    .Field<int16_t>("flags","e")
    .Field<int16_t>("status","e")
    .Field<std::string>("username","ua","add_username")
    .Field<std::string>("username","um", "mod_username")
    .From()
      .AddTable("equipment","e")
      .BeginSubquery("ad")
        .Field<int32_t>("equipment_id","ad_e")
        .Field<int32_t>("group_id","ad_g")
        .From()
        .AddTable("equipment","ad_e")
        .EndFromTableBlock()
        .Join()
          .InnerJoin(
            RecordKey("equipment","group_id","ad_e"),
            RecordKey("group","group_id","ad_g"))
        .EndJoinBlock()
        .Where()
          .BeginClause()
            .AddCondition<int16_t>("ad_g.status",SqlOperator::kEqual,1)
          .EndClauseBlock()
        .EndWhereBlock()
      .EndSubqueryInsideFrom()
    .EndFromTableBlock()
    .Join()
      .InnerJoin(
        RecordKey("equipment","equipment_type_id","e"),
        RecordKey("equipment_type","equipment_type_id","ut"))
      .InnerJoin(
        RecordKey("equipment","equipment_class_id","e"),
        RecordKey("equipment_class","equipment_class_id","uc"))
      .InnerJoin(
        RecordKey("equipment","equipment_sub_class_id","e"),
        RecordKey("equipment_sub_class","equipment_sub_class_id","us"))
      .InnerJoin(
        RecordKey("equipment","service_id","e"),
        RecordKey("services","service_id","s"))
      .InnerJoin(
        RecordKey("services","company_id","s"),
        RecordKey("company","company_id","c"))
      .LeftJoin(
        RecordKey("equipment","add_by","e"),
        RecordKey("users","user_id","ua"))
      .LeftJoin(
        RecordKey("equipment","mod_by","e"),
        RecordKey("users","user_id","um"))
      .InnerJoin(
        RecordKey("equipment","equipment_id","e"),
        RecordKey("ad","equipment_id","ad"))
    .EndJoinBlock()
    .Where()
      .BeginClause()
        .AddCondition("equipment_type_name",SqlOperator::kLike, "Dozer")
        .And()
        .AddCondition("s.service_code",SqlOperator::kEqual, "MS")
      .EndClauseBlock()
      .EndWhereBlock()
    .GroupBy()
      .Field("company_id","e")
      .Field("service_name")
    .EndGroupByBlock()
    .OrderBy()
      .OrderByAsc("company_name")
      .OrderByAsc("service_name")
      .OrderByAsc("unit_type_code")
      .OrderByAsc("unit_type_class_code")
    .EndOrderByBlock()
    .EndBlock();

  // clang-format on

  auto output = select->GenerateQuery();
  std::cout << select->GenerateQuery(true) << std::endl;

  std::string result =
      "SELECT DISTINCT e.equipment_id, c.company_id, s.service_id, "
      "e.unit_type_id, e.unit_class_id, e.unit_subclass_id, e.reg_id, e.code "
      "AS equipment_code, c.name AS company_name, s.name AS service_name, "
      "ut.code AS unit_type_code, ut.name AS unit_type_name, uc.code AS "
      "unit_class_code, uc.name AS unit_class_name, us.code AS "
      "unit_sub_class_code, us.name AS unit_subclass_name, e.flags, e.status, "
      "ua.username AS add_username, um.username AS mod_username FROM equipment "
      "AS e INNER JOIN equipment_type AS ut ON e.equipment_type_id = "
      "ut.equipment_type_id INNER JOIN equipment_class AS uc ON "
      "e.equipment_class_id = uc.equipment_class_id INNER JOIN "
      "equipment_sub_class AS us ON e.equipment_sub_class_id = "
      "us.equipment_sub_class_id INNER JOIN services AS s ON e.service_id = "
      "s.service_id INNER JOIN company AS c ON s.company_id = c.company_id "
      "LEFT JOIN users AS ua ON e.add_by = ua.user_id LEFT JOIN users AS um ON "
      "e.mod_by = um.user_id ";

  REQUIRE(true == true);
}

TEST_CASE("select-test-complex-2", "[validator][normal-case]") {
  using DefaultPostgresParamType = nvm::containers::DefaultPostgresParamType;
  using NvSelect = nvm::containers::NvSelect<DefaultPostgresParamType>;
  using SqlOperator = nvm::containers::SqlOperator;
  using RecordKey = nvm::containers::RecordKey;
  using SqlAggregateFn = nvm::containers::SqlAggregateFunction;
  NvSelect s(1);

  // clang-format off
  s.Field<int32_t>("transaction_id", "w")
      .Field<std::string>("transaction_code", "w")
      .Field<std::chrono::system_clock::time_point>("date", "w", "shift_date")
      .Field<std::chrono::system_clock::time_point>("date_in", "w")
      .Field<std::chrono::system_clock::time_point>("date_out", "w")
      .Field<int32_t>("shift_id", "w")
      .Field<std::string>("unit_code", "w")
      .Field<std::string>("vendor_code", "w")
      .Field<std::string>("model", "e")
      .Field<std::string>("unit_type", "e")
      .Field<double>("gross_weight", "w")
      .Field<double>("tarre_weight", "w")
      .Field<double>("nett_weight", "w")
      .Field<std::string>("remarks", "w")
      .From()
        .AddTable("weightbridge_transaction", "w")
        .EndFromTableBlock()
      .Join()
        .InnerJoin(
          RecordKey("weightbridge_transaction","unit_code","w"),
          RecordKey("equipment","equipment_code","e"))
      .EndJoinBlock()
      .Where()
        .BeginClause()
          .AddCondition<std::string>("shift_date",SqlOperator::kEqual,"2024-05-20T00:00:00")
        .EndClauseBlock()
      .EndWhereBlock()
      .OrderBy()
        .OrderByAsc("shift_date")
        .OrderByAsc("date_in")
      .EndOrderByBlock()
      .EndBlock();

  // clang-format on

  std::cout << s.GenerateQuery(true) << std::endl;
}
TEST_CASE("select-test-grouby-count", "[validator][normal-case]") {
  // using Validator = nvm::containers::validations::Validator;
  using DefaultPostgresParamType = nvm::containers::DefaultPostgresParamType;
  using NvSelect = nvm::containers::NvSelect<DefaultPostgresParamType>;
  using SqlOperator = nvm::containers::SqlOperator;
  using RecordKey = nvm::containers::RecordKey;
  using SqlAggregateFn = nvm::containers::SqlAggregateFunction;

  NvSelect select;
  // clang-format off
  select
    .Field<int32_t>("equipment_id","e")
    .Field<int32_t>("company_id","c")
    .Field<int32_t>("service_id","e")
    .Field<int32_t>("unit_type_id","e")
    .Field<int32_t>("unit_class_id","e")
    .Field<int32_t>("*",std::nullopt,SqlAggregateFn::Count)
    .From()
      .AddTable("equipment","e")
    .EndFromTableBlock()
    .Join()
      .InnerJoin(
        RecordKey("equipment","service_id","e"),
        RecordKey("service","service_id","s"))
      .InnerJoin(
        RecordKey("service","company_id","s"),
        RecordKey("company","company_id","c"))
    .EndJoinBlock()
    .Where()
      .BeginClause()
        .AddConditionIn<int32_t>("c.company_id",{1,2,3})
      .EndClauseBlock()
    .EndWhereBlock()
    .GroupBy()
      .Field("equipment_id","e")
      .Field("company_id","c")
      .Field("service_id","e")
      .Field("unit_type_id","e")
      .Field("unit_class_id","e")
    .EndGroupByBlock()
    .OrderBy()
      .OrderByAsc("company_id","c")
      .OrderByAsc("unit_type_id","e")
      .OrderByAsc("unit_class_id","e")
      .OrderByAsc("equipment_id","e")
    .EndOrderByBlock()
    .EndBlock();
    
  //   std::cout << select.GenerateQuery() << std::endl;

  // clang-format off

     std::cout << select.GenerateQuery(true) << std::endl;

  REQUIRE(true == true);
}