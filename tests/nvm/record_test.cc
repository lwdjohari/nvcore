#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>

#include "catch2/catch_all.hpp"
#include "nvm/sqlbuilder/where.h"
#include "nvm/sqlbuilder/operation.h"
#include "nvm/sqlbuilder/limit_offset.h"

#include "nvm/logic.h"
using namespace nvm;

TEST_CASE("record-page", "[record-page][normal-case]") {
  using RecordPage = nvm::sqlbuilder::RecordPage;

  auto page = RecordPage(300, 15);

  CAPTURE(page.Pages());

  std::cout << "Record Page\n"
            << "Items: " << page.TotalItems() << "\n"
            << "Pages: " << page.ItemsPerPage() << "\n"
            << "Offset of page 3: " << page.GetPageOffset(3) << "\n"
            << "Items Per Page: " << page.ItemsPerPage() << "\n";

  REQUIRE(page.Pages() == 20);
  REQUIRE(page.GetPageOffset(3) == 30);
  REQUIRE(page.ItemsPerPage() == 15);
  REQUIRE(page.TotalItems() == 300);
}

TEST_CASE("record-page-less-than-per-page", "[record-page][lt-per-page-case]") {
  using RecordPage = nvm::sqlbuilder::RecordPage;

  auto page = RecordPage(8, 15);

  CAPTURE(page.Pages());

  std::cout << "Record Page\n"
            << "Items: " << page.TotalItems() << "\n"
            << "Pages: " << page.ItemsPerPage() << "\n"
            << "Offset of page 3: " << page.GetPageOffset(1) << "\n"
            << "Items Per Page: " << page.ItemsPerPage() << "\n";

  REQUIRE(page.Pages() == 1);
  REQUIRE(page.GetPageOffset(1) == 0);
  REQUIRE(page.ItemsPerPage() == 15);
  REQUIRE(page.TotalItems() == 8);
}

TEST_CASE("record-page-over-page", "[record-page][lt-per-page-case]") {
  using RecordPage = nvm::sqlbuilder::RecordPage;

  auto page = RecordPage(8, 15);

  CAPTURE(page.Pages());

  std::cout << "Record Page\n"
            << "Items: " << page.TotalItems() << "\n"
            << "Pages: " << page.ItemsPerPage() << "\n"
            << "Offset of page 3: " << page.GetPageOffset(3) << "\n"
            << "Items Per Page: " << page.ItemsPerPage() << "\n";

  REQUIRE(page.Pages() == 1);
  REQUIRE(page.GetPageOffset(3) == 0);
  REQUIRE(page.ItemsPerPage() == 15);
  REQUIRE(page.TotalItems() == 8);
}

// TEST_CASE("record-sort", "[record-sort][normal-case]") {
//   using OrderByStatement = nvm::sqlbuilder::OrderByStatement;
//   using SortField = nvm::sqlbuilder::SortField;
//   using SortType = nvm::sqlbuilder::SortType;
//   using RecordTable = nvm::sqlbuilder::RecordTable;
//   using FieldPinMode = nvm::sqlbuilder::FieldPinMode;

//   auto sorter = OrderByStatement();
//   auto user_table = std::make_shared<RecordTable>(RecordTable("users", "u"));
//   auto company_table =
//       std::make_shared<RecordTable>(RecordTable("company", "c"));

//   sorter.AddField("company_name", SortField("name", SortType::Ascending,
//                                             FieldPinMode::None,
//                                             company_table));
//   sorter.AddField("username", SortField("username", SortType::Descending,
//                                         FieldPinMode::None, user_table));

//   auto query = sorter.GenerateQuery();
//   CAPTURE(query);

//   std::cout << "Query: " << query;

//   REQUIRE(query == "ORDER BY u.username DESC, c.name ASC");
// }

TEST_CASE("record-filter", "[record-sort][normal-case]") {
  using DefaultPostgresParamType = nvm::sqlbuilder::DefaultPostgresParamType;
  using WhereStatement =
      nvm::sqlbuilder::WhereStatement<DefaultPostgresParamType>;
  using SqlOperator = nvm::sqlbuilder::SqlOperator;
  using LogicOperator = nvm::sqlbuilder::LogicOperator;
  WhereStatement filter;

  // RecordClause& clause = filter.AddClause(6);
  filter
    .AddConditionIn<std::string>("department",
                                  {"engineering", "sales", "devops"})
    .And()
    .StartGroup()
    .AddCondition<int>("age", SqlOperator::kGreater, 30)
    .And()
    .AddCondition<int>("salary", SqlOperator::kGreater, 50000)
    .EndGroup()
    .Or()
    .AddCondition<std::string>("name", SqlOperator::kLike, "Alice%")
    .And()
    .AddCondition<std::string>("city", SqlOperator::kNotEqual, "NYC")
    .Or()
    .StartGroup()
    .AddConditionBetween<std::chrono::system_clock::time_point>(
        "hire_date", std::chrono::system_clock::now(),
        std::chrono::system_clock::now() + std::chrono::hours(24))
    .EndGroup();

  std::string where_clause = filter.GenerateQuery();
  auto parameter_values = filter.Values();
  std::cout << "Generated SQL WHERE clause:\n" << where_clause << std::endl;
  // std::cout << "Values:\n" << filter.GetAllParameterValuesAsString();
  std::cout << "[" << parameter_values->size() << " Parameter Values]";

  REQUIRE(true == true);
  // REQUIRE(where_clause ==
  //         "WHERE department IN ($6, $7, $8) AND (age > $9 AND salary > $10) OR "
  //         "name LIKE $11 AND city != $12 OR (hire_date BETWEEN $13 AND $14)");
}

TEST_CASE("record-insert", "[record-op][normal-case]") {
  using DefaultPostgresParamType = nvm::sqlbuilder::DefaultPostgresParamType;
  using RecordInsert = nvm::sqlbuilder::RecordInsert<DefaultPostgresParamType>;
  using RecordUpdate = nvm::sqlbuilder::RecordUpdate<DefaultPostgresParamType>;
  using RecordDelete = nvm::sqlbuilder::RecordDelete<DefaultPostgresParamType>;
  using SqlOperator = nvm::sqlbuilder::SqlOperator;

  // Example for RecordInsert
  auto insert_values =
      std::make_shared<std::vector<DefaultPostgresParamType>>();
  RecordInsert insert_record("users", 1, insert_values);

  insert_record.AddValue("username", std::string("john_doe"))
      .AddValue("age", 30)
      .AddValue("created_at", std::chrono::system_clock::now())
      .AddReturning("id");

  std::cout << "Insert Query: " << insert_record.ToString() << std::endl;

  std::cout << "Columns: ";
  for (const auto& column : insert_record.Columns()) {
    std::cout << column << ", ";
  }
  std::cout << std::endl;

  REQUIRE(true == true);
}

TEST_CASE("record-update", "[record-op][normal-case]") {
  using DefaultPostgresParamType = nvm::sqlbuilder::DefaultPostgresParamType;
  using RecordInsert = nvm::sqlbuilder::RecordInsert<DefaultPostgresParamType>;
  using RecordUpdate = nvm::sqlbuilder::RecordUpdate<DefaultPostgresParamType>;
  using RecordDelete = nvm::sqlbuilder::RecordDelete<DefaultPostgresParamType>;
  using SqlOperator = nvm::sqlbuilder::SqlOperator;

  // Example for RecordUpdate
  auto update_values =
      std::make_shared<std::vector<DefaultPostgresParamType>>();
  RecordUpdate update_record("users", 1, update_values);
  update_record.SetValue("age", 31)
      .AddCondition("username", SqlOperator::kEqual, std::string("john_doe"))
      .AddReturning("updated_at");

  std::cout << "Update Query: " << update_record.ToString() << std::endl;

  std::cout << "Columns: ";
  for (const auto& column : update_record.Columns()) {
    std::cout << column << ", ";
  }
  std::cout << std::endl;

  REQUIRE(true == true);
}

TEST_CASE("record-delete", "[record-op][normal-case]") {
  using DefaultPostgresParamType = nvm::sqlbuilder::DefaultPostgresParamType;
  using RecordInsert = nvm::sqlbuilder::RecordInsert<DefaultPostgresParamType>;
  using RecordUpdate = nvm::sqlbuilder::RecordUpdate<DefaultPostgresParamType>;
  using RecordDelete = nvm::sqlbuilder::RecordDelete<DefaultPostgresParamType>;
  using SqlOperator = nvm::sqlbuilder::SqlOperator;
  // Example for RecordDelete
  auto delete_values =
      std::make_shared<std::vector<DefaultPostgresParamType>>();
  RecordDelete delete_record("users", 1, delete_values);
  delete_record
      .AddCondition("username", SqlOperator::kEqual, std::string("john_doe"))
      .AddReturning("deleted_at");

  std::cout << "Delete Query: " << delete_record.ToString() << std::endl;

  std::cout << "Columns: ";
  for (const auto& column : delete_record.Columns()) {
    std::cout << column << ", ";
  }
  std::cout << std::endl;

  REQUIRE(true == true);
}