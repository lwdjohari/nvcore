#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>

#include "catch2/catch_all.hpp"
#include "nvm/logic.h"
#include "nvm/containers/record_page.h"

using namespace nvm;

TEST_CASE("record-page", "[record-page][normal-case]") {
  using RecordPage = nvm::containers::RecordPage;

  auto page = RecordPage(300,15);
  
  CAPTURE(page.Pages());

  std::cout << "Record Page\n" << "Items: " << page.TotalItems() << "\n"
  << "Pages: " << page.ItemsPerPage() << "\n"
  << "Offset of page 3: " << page.GetPageOffset(3) << "\n"
  << "Items Per Page: " << page.ItemsPerPage() << "\n";
  
  REQUIRE(page.Pages() == 20);
  REQUIRE(page.GetPageOffset(3) == 30);
  REQUIRE(page.ItemsPerPage() == 15);
  REQUIRE(page.TotalItems() == 300);
}

TEST_CASE("record-page-less-than-per-page", "[record-page][lt-per-page-case]") {
  using RecordPage = nvm::containers::RecordPage;

  auto page = RecordPage(8,15);
  
  CAPTURE(page.Pages());

  std::cout << "Record Page\n" << "Items: " << page.TotalItems() << "\n"
  << "Pages: " << page.ItemsPerPage() << "\n"
  << "Offset of page 3: " << page.GetPageOffset(1) << "\n"
  << "Items Per Page: " << page.ItemsPerPage() << "\n";
  
  REQUIRE(page.Pages() == 1);
  REQUIRE(page.GetPageOffset(1) == 0);
  REQUIRE(page.ItemsPerPage() == 15);
  REQUIRE(page.TotalItems() == 8);
}

TEST_CASE("record-page-over-page", "[record-page][lt-per-page-case]") {
  using RecordPage = nvm::containers::RecordPage;

  auto page = RecordPage(8,15);
  
  CAPTURE(page.Pages());

  std::cout << "Record Page\n" << "Items: " << page.TotalItems() << "\n"
  << "Pages: " << page.ItemsPerPage() << "\n"
  << "Offset of page 3: " << page.GetPageOffset(3) << "\n"
  << "Items Per Page: " << page.ItemsPerPage() << "\n";
  
  REQUIRE(page.Pages() == 1);
  REQUIRE(page.GetPageOffset(3) == 0);
  REQUIRE(page.ItemsPerPage() == 15);
  REQUIRE(page.TotalItems() == 8);
}


