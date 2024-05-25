#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>

#include "catch2/catch_all.hpp"
#include "nvm/sqlbuilder/nv_select_builder.h"
#include "nvm/strings/utility.h"
using namespace nvm;

TEST_CASE("select-join-complex-1", "[validator][normal-case]") {
  // using Validator = nvm::sqlbuilder::validations::Validator;
  using DefaultPostgresParamType = nvm::sqlbuilder::DefaultPostgresParamType;
  using NvSelect = nvm::sqlbuilder::NvSelect<DefaultPostgresParamType>;
  using SqlOperator = nvm::sqlbuilder::SqlOperator;
  using RecordKey = nvm::sqlbuilder::RecordKey;
  using SqlAggregateFn = nvm::sqlbuilder::SqlAggregateFunction;
  using ParameterParser =
      nvm::sqlbuilder::PostgresDefaultParameterParser<DefaultPostgresParamType>;
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
          .AddCondition<int16_t>("ad_g.status",SqlOperator::kEqual,1)
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
        .AddCondition("equipment_type_name",SqlOperator::kLike, "Dozer")
        .And()
        .AddCondition("s.service_code",SqlOperator::kEqual, "MS")
      .EndWhereBlock()
    .GroupBy()
      .Field("company_id","e")
      .Field("service_name")
    .EndGroupByBlock()
    .OrderBy()
      .Asc("company_name")
      .Asc("service_name")
      .Asc("unit_type_code")
      .Asc("unit_type_class_code")
    .EndOrderByBlock();

  // clang-format on

  auto output = select->GenerateQuery();
  std::cout << select->GenerateQuery(true) << std::endl;

  ParameterParser parser(select->Values());
  std::cout << "\nPARAMETER VALUES:" << std::endl;
  std::cout << parser.GetAllParameterValuesAsString() << std::endl;

  REQUIRE(true == true);
}

TEST_CASE("select-join-complex-2", "[validator][normal-case]") {
  // using Validator = nvm::sqlbuilder::validations::Validator;
  using DefaultPostgresParamType = nvm::sqlbuilder::DefaultPostgresParamType;
  using NvSelect = nvm::sqlbuilder::NvSelect<DefaultPostgresParamType>;
  using SqlOperator = nvm::sqlbuilder::SqlOperator;
  using RecordKey = nvm::sqlbuilder::RecordKey;
  using SqlAggregateFn = nvm::sqlbuilder::SqlAggregateFunction;
  using ParameterParser =
      nvm::sqlbuilder::PostgresDefaultParameterParser<DefaultPostgresParamType>;

  std::string service_code("RS");
  std::string date_format = "DD-MON-YYYY";
  auto equipment_types = std::vector<std::string>({"HMT", "HLB"});

  auto select = std::make_unique<NvSelect>(1);

  // clang-format off
  
  (*select)
    .Field<int32_t>("equipment_id","e")
    .Field<int32_t>("company_id","c")
    .Field<int32_t>("service_id","s")
    .Field<int32_t>("unit_type_id","e")
    .Field<int32_t>("unit_class_id","e")
    .Field<int32_t>("unit_subclass_id","e")
    .Field<std::string>("reg_id","e")
    .Field<std::string>("code","e", "equipment_code")
    .Fn<std::string>("UPPER",{"c.name"},"company_name")
    // TO_CHAR(e.entry_date,'DD-MON-YYYY')
    .Fn<std::string>("TO_CHAR", "%s %v",{date_format},{"e.entry_date"}, "entry_date")
    // TO_CHAR(e.termination_date,'DD-MON-YYYY')
    .Fn("TO_CHAR", "%s %v",{date_format},{"e.termination_date"}, "termination_date")
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
        .AddConditionIn("unit_type_code",equipment_types)
        .And()
        .AddCondition("s.service_code",SqlOperator::kEqual, service_code)
      .EndWhereBlock()
    .OrderBy()
      .Asc("company_name")
      .Asc("service_name")
      .Asc("unit_type_code")
      .Asc("unit_type_class_code")
    .EndOrderByBlock();

  // clang-format on

  std::cout << select->GenerateQuery(true) << std::endl;

  ParameterParser parser(select->Values());
  std::cout << "\nPARAMETER VALUES:" << std::endl;
  std::cout << parser.GetAllParameterValuesAsString() << std::endl;

  REQUIRE(true == true);
}

TEST_CASE("select-test-complex-2", "[validator][normal-case]") {
  using DefaultPostgresParamType = nvm::sqlbuilder::DefaultPostgresParamType;
  using NvSelect = nvm::sqlbuilder::NvSelect<DefaultPostgresParamType>;
  using SqlOperator = nvm::sqlbuilder::SqlOperator;
  using RecordKey = nvm::sqlbuilder::RecordKey;
  using SqlAggregateFn = nvm::sqlbuilder::SqlAggregateFunction;
  using ParameterParser =
      nvm::sqlbuilder::PostgresDefaultParameterParser<DefaultPostgresParamType>;

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
        .AddCondition<std::string>("shift_date",SqlOperator::kEqual,"2024-05-20T00:00:00")
        .And()
        .AddSubquery("w.vendor_code",SqlOperator::kIn,"")
          .Field<std::string>("vendor_code")
          .From()
            .AddTable("vendor")
          .EndFromTableBlock()
          .Where()
            .AddConditionIn<std::string>("vendor_code",{"MP","GP","SC"})
          .EndWhereBlock()
        .EndSubqueryInsideWhereCondition()
      .EndWhereBlock()
      .OrderBy()
        .Asc("shift_date")
        .Asc("date_in")
      .EndOrderByBlock();

  // clang-format on

  std::cout << s.GenerateQuery(true) << std::endl;
  ParameterParser parser(s.Values());
  std::cout << "\nPARAMETER VALUES:" << std::endl;
  std::cout << parser.GetAllParameterValuesAsString() << std::endl;
}
TEST_CASE("select-test-groupby-count", "[validator][normal-case]") {
  // using Validator = nvm::sqlbuilder::validations::Validator;
  using DefaultPostgresParamType = nvm::sqlbuilder::DefaultPostgresParamType;
  using NvSelect = nvm::sqlbuilder::NvSelect<DefaultPostgresParamType>;
  using SqlOperator = nvm::sqlbuilder::SqlOperator;
  using RecordKey = nvm::sqlbuilder::RecordKey;
  using SqlAggregateFn = nvm::sqlbuilder::SqlAggregateFunction;
  using ParameterParser =
      nvm::sqlbuilder::PostgresDefaultParameterParser<DefaultPostgresParamType>;

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
      .AddConditionIn<int32_t>("c.company_id",{1,2,3})
    .EndWhereBlock()
    .GroupBy()
      .Field("equipment_id","e")
      .Field("company_id","c")
      .Field("service_id","e")
      .Field("unit_type_id","e")
      .Field("unit_class_id","e")
    .EndGroupByBlock()
    .OrderBy()
      .Asc("company_id","c")
      .Asc("unit_type_id","e")
      .Asc("unit_class_id","e")
      .Asc("equipment_id","e")
    .EndOrderByBlock();
    
  //   std::cout << select.GenerateQuery() << std::endl;

  // clang-format on

  std::cout << select.GenerateQuery(true) << std::endl;
  ParameterParser parser(select.Values());
  std::cout << "\nPARAMETER VALUES:" << std::endl;
  std::cout << parser.GetAllParameterValuesAsString() << std::endl;

  REQUIRE(true == true);
}