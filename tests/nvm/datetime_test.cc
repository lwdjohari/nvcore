#define CATCH_CONFIG_MAIN
#include "nvm/dates/datetime.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>

#include "catch2/catch_all.hpp"
#include "date/tz.h"

using namespace nvm;

TEST_CASE("datetime", "[datetime][local-test]") {
  auto local = nvm::dates::DateTime("Etc/UTC");
  auto local_comp = nvm::dates::DateTime(local);
  CAPTURE(local.TzTime()->get_local_time());
  CAPTURE(local_comp.TzTime()->get_local_time());

  std::cout << "Date1: " << local.ToIso8601() << " [" << local.TzName() << "]"
            << std::endl;
  std::cout << "Date2: " << local_comp.ToIso8601() << " ["
            << local_comp.TzName() << "]" << std::endl;
  std::cout << "D1_ptr: " << &(*local.TzTime())
            << " D2_ptr: " << &(*local_comp.TzTime()) << std::endl;
  std::cout << "D1_size: " << sizeof(local)
            << " D2_size: " << sizeof(local_comp) << std::endl;
  REQUIRE(local.TzTime()->get_local_time() ==
          local_comp.TzTime()->get_local_time());
}

TEST_CASE("datetime-print", "[datetime][local-test]") {
  auto local_time = nvm::dates::DateTime();

  // Print the local time
  std::cout << "Current local time: " << local_time << '\n';

  REQUIRE(local_time.TzTime() != nullptr);
}

TEST_CASE("datetime-utc-print", "[datetime][utc-test]") {
  auto local_time = nvm::dates::DateTime("Etc/UTC");

  // Print the local time
  std::cout << "Current UTC time: " << local_time << '\n';

  REQUIRE(local_time.TzTime() != nullptr);
}

TEST_CASE("datetime-test-add-duration", "[datetime][local-test]") {
  auto seconds = 7200;
  std::chrono::seconds duration(seconds);
  auto now = nvm::dates::DateTime();

  // Display the current local time
  std::cout << "Current local time: \n" << now << std::endl;

  auto next = now + duration;

  // Display the local time after adding the duration
  std::cout << "Local time after adding " << seconds << " seconds: \n"
            << next << std::endl;

  REQUIRE(next.TzTime() != nullptr);
}

TEST_CASE("datetime-test-subtract-duration", "[datetime][local-test]") {
  auto seconds = 7200;
  std::chrono::seconds duration(seconds);
  auto now = nvm::dates::DateTime();

  // Display the current local time
  std::cout << "Current local time: \n" << now << std::endl;

  auto next = now - duration;

  // Display the local time after subtract the duration
  std::cout << "Local time after subtract " << seconds << " seconds: \n"
            << next << std::endl;

  REQUIRE(next.TzTime() != nullptr);
}

TEST_CASE("datetime-date-subtract", "[datetime][local-test]") {
  auto seconds = 7200;
  std::chrono::seconds duration(seconds);
  auto now = nvm::dates::DateTime();

  // Display the current local time
  std::cout << "Current local time: \n" << now << std::endl;

  auto next = now + duration;

  // Display the local time after subtract the duration
  std::cout << "Local time after adding " << seconds << " seconds: \n"
            << next << std::endl;

  auto result = next - now;

  std::chrono::seconds diff =
      std::chrono::duration_cast<std::chrono::seconds>(result.value());

  std::cout << "Diff in second: \n" << diff.count() << std::endl;

  REQUIRE(duration == diff);
}

TEST_CASE("datetime-timezone-convert", "[datetime][local-test]") {
  auto jkt_time = dates::DateTime(2022, 7, 10, 13, 00, 00, 00, "Asia/Jakarta");
  auto answer_utc = dates::DateTime(2022, 7, 10, 06, 00, 00, 00, "Etc/UTC");
  auto answer_new_york =
      dates::DateTime(2022, 7, 10, 02, 00, 00, 00, "America/New_York");

  auto res_utc = jkt_time.ToTimezone("Etc/UTC");
  auto res_new_york = jkt_time.ToTimezone("America/New_York");

  CAPTURE(jkt_time);
  CAPTURE(res_utc);
  CAPTURE(res_new_york);

  std::cout << "TARGET" << std::endl;
  std::cout << "Jakarta : " << jkt_time << std::endl;
  std::cout << "UTC     : " << answer_utc << std::endl;
  std::cout << "New York: " << answer_new_york << std::endl;

  std::cout << "RESULT" << std::endl;
  std::cout << "UTC     : " << res_utc << std::endl;
  std::cout << "New York: " << res_new_york << std::endl;

  REQUIRE(res_new_york == answer_new_york);
  REQUIRE(res_utc == answer_utc);
}
