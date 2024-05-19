#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>

#include "catch2/catch_all.hpp"
#include "nvm/containers/record_select.h"
#include "nvm/strings/utility.h"
using namespace nvm;

TEST_CASE("select-join-complex-1", "[validator][normal-case]") {
  // using Validator = nvm::containers::validations::Validator;
  using DefaultPostgresParamType = nvm::containers::DefaultPostgresParamType;
  using SelectBlock = nvm::containers::SelectBlock<DefaultPostgresParamType>;
  using SqlOperator = nvm::containers::SqlOperator;
  using RecordKey = nvm::containers::RecordKey;
  using SqlAggregateFn = nvm::containers::SqlAggregateFunction;
  SelectBlock select(1);
  // clang-format off

  select
    .Field<int32_t>("equipment_id","e",SqlAggregateFn::Distinct)
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
      .EndFromTableBlock()
    .CreateJoinBlock()
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
      .EndJoinBlock()
    .EndBlock();
  // clang-format on

  auto output = select.GenerateQuery();
  std::cout << select.GenerateQuery(true) << std::endl;

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

  REQUIRE(result == output);
}

TEST_CASE("select-test-complex", "[validator][normal-case]") {
  // using Validator = nvm::containers::validations::Validator;
  using DefaultPostgresParamType = nvm::containers::DefaultPostgresParamType;
  using SelectBlock = nvm::containers::SelectBlock<DefaultPostgresParamType>;
  using SqlOperator = nvm::containers::SqlOperator;
  using RecordKey = nvm::containers::RecordKey;
  using SqlAggregateFn = nvm::containers::SqlAggregateFunction;

  SelectBlock select(1);
  // clang-format off
  // select
  //   .Field<int32_t>("company_id","c", "company_id")
  //   .Field<std::string>("name","c","company_name")
  //   .Field<int32_t>("service_id","s", "service_id")
  //   .Field<std::string>("name","s","service_name")
  //   .Field<int32_t>("unit_type_id","e","unit_type_id")
  //   .Field<int32_t>("unit_class_id","e","unit_class_id")
  //   .Field<int32_t>("unit_subclass_id","e","unit_subclass_id")
  //   .Field<std::string>("code","ut","unit_type_code")
  //   .Field<std::string>("name","ut","unit_type_name")
  //   .Field<std::string>("code","uc","unit_class_code")
  //   .Field<std::string>("name","uc","unit_class_name")
  //   .Field<std::string>("code","us","unit_subclass_code")
  //   .Field<std::string>("name","us", "unit_subclass_name")
  //   .Field<int32_t>("qty","group_ec","qty")
  //   .From()
  //     .AddTable("equipment","e")
  //     .EndFromTableBlock()
  //   .CreateJoinBlock()
  //     .InnerJoinWithSubQuery(
  //       "ec","equipment_id","group_ec" 
  //       RecordKey("equipment","equipment_id","e"))
  //         .Field("equipment_id","ec","equipment_id")
  //         .Field("equipment_id","ec", SqlAggregateFn::Count, "qty")
  //         .From()
  //           .AddTable("equipment","ec")
  //           .EndFromTableBlock()
  //         .InnerJoin(
  //           RecordKey("equipment","equipment_id","ec"),
  //           RecordKey("services","service_id","sc"))
  //         .InnerJoin(
  //           RecordKey("services","service_id","sc"),
  //           RecordKey("company","company_id","cc"))
  //         .EndJoinBlock()
  //         .CreateWhereBlock()
  //           .ConditionIn<int32_t>("company_id","cc", companies)
  //           .EndWhereBlock()
  //         .CreateGroupBy()
  //           .Field("company_id","cc")
  //           .Field("service_id","sc")
  //           .Field("unit_type_id")
  //           .Field("unit_class_id")
  //           .Field("unit_subclass_id")
  //         .EndGroupByBlock()
  //         .CreateHavingBy()
  //           .Field<int32_t>("qty",SqlOperator::kGreater,10)
  //         .EndBlock()
  //       .EndSubquery()
  //     .InnerJoin(
  //       RecordKey("equipment","service_id","e"),
  //       RecordKey("services","service_id","s"))
  //     .InnerJoin(
  //       RecordKey("services","company_id","s"),
  //       RecordKey("company","company_id","c"))
  //     .InnerJoin(
  //       RecordKey("equipment","unit_type_id","e"),
  //       RecordKey("equipment_type","unit_type_id","ut"))
  //     .InnerJoin(
  //       RecordKey("equipment","unit_class_id","e"),
  //       RecordKey("equipment_class","unit_class_id","uc"))
  //     .InnerJoin(
  //       RecordKey("equipment","unit_subclass_id","e"),
  //       RecordKey("equipment_subclass","unit_subclass_id","us"))
  //     .InnerJoin(
  //       RecordKey("equipment","company_id","e"),
  //       RecordKey("company","company_id","c"))
  //     .InnerJoin(
  //       RecordKey("equipment","equipment_id","e"),
  //       GetEquipmentReadAccessData(user_id)) // Subquery to get group_id access to equipment data
  //     .EndJoinBlock()
  //   .EndBlock();

  //   std::cout << select.GenerateQuery() << std::endl;

  // clang-format off


  REQUIRE(true == true);
}