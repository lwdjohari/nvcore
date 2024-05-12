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

  auto next = now + dates::ToNanosecondDuration(duration);

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

  auto next = now - dates::ToNanosecondDuration(duration);

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

  auto next = now + dates::ToNanosecondDuration(duration);

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
  using DateTime = dates::DateTime;

  auto jkt_time = DateTime(2022, 7, 10, 13, 00, 00, 00, "Asia/Jakarta");
  auto answer_utc = DateTime(2022, 7, 10, 06, 00, 00, 00, "Etc/UTC");
  auto answer_new_york =
      DateTime(2022, 7, 10, 02, 00, 00, 00, "America/New_York");

  auto res_utc = jkt_time.ToTimezone("Etc/UTC");
  auto res_new_york = jkt_time.ToTimezone("America/New_York");

  CAPTURE(jkt_time);
  CAPTURE(answer_utc);
  CAPTURE(answer_new_york);
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

TEST_CASE("datetime-first-and-end-day-of-month", "[datetime][local-test]") {
  using DateTime = dates::DateTime;

  auto jkt_time = DateTime(2022, 7, 10, 13, 0, 0, 0, "Asia/Jakarta");
  auto ans_first = DateTime(2022, 7, 1, 0, 0, 0, 0, "Asia/Jakarta");
  auto ans_last = DateTime(2022, 7, 31, 23, 59, 59, 0, "Asia/Jakarta");

  auto first_day_of_month = jkt_time.GetStartOfMonth();
  auto last_day_of_month = jkt_time.GetEndOfMonth();

  CAPTURE(jkt_time);
  CAPTURE(ans_first);
  CAPTURE(ans_last);
  CAPTURE(first_day_of_month);
  CAPTURE(last_day_of_month);

  std::cout << "TARGET" << std::endl;
  std::cout << "DATE    : " << jkt_time << std::endl;
  std::cout << "FIRST   : " << ans_first << std::endl;
  std::cout << "LAST    : " << ans_last << std::endl;

  std::cout << "RESULT" << std::endl;
  std::cout << "FIRST   : " << first_day_of_month << std::endl;
  std::cout << "LAST    : " << last_day_of_month << std::endl;

  REQUIRE(first_day_of_month == ans_first);
  REQUIRE(last_day_of_month == ans_last);
}

TEST_CASE("datetime-format", "[datetime][local-test]") {
  using DateTime = dates::DateTime;

  auto jkt_time = DateTime(2022, 7, 10, 13, 0, 0, 0, "Asia/Jakarta");

  std::string format = "%d-%b-%y";
  auto res = jkt_time.ToString(format);

  CAPTURE(jkt_time);

  std::cout << "TARGET" << std::endl;
  std::cout << "FORMAT   : " << format << std::endl;
  std::cout << "DATE     : " << jkt_time << std::endl;

  std::cout << "RESULT" << std::endl;
  std::cout << res << std::endl;

  REQUIRE(res == "10-Jul-22");
}

TEST_CASE("datetime-add-100-years-seconds", "[datetime][local-test]") {
  using DateTime = dates::DateTime;
  using namespace std::chrono;

  auto jkt_time = DateTime(2022, 7, 10, 0, 0, 0, 0, "Asia/Singapore");
  auto ans = DateTime(2122, 07, 10, 0, 0, 0, 0, "Asia/Singapore");

  // Define duration types with int64_t to prevent overflow
  using seconds = duration<int64_t>;

  constexpr int64_t seconds_per_minute = 60;
  constexpr int64_t minutes_per_hour = 60;
  constexpr int64_t hours_per_day = 24;
  constexpr int64_t days_per_normal_year = 365;
  constexpr int64_t days_per_leap_year = 366;
  constexpr int64_t normal_years = 76;
  constexpr int64_t leap_years = 24;

  // Calculate total seconds in normal and leap years
  auto total_seconds =
      seconds(hours_per_day * minutes_per_hour * seconds_per_minute) *
      (days_per_normal_year * normal_years + days_per_leap_year * leap_years);

  auto res = jkt_time + dates::ToNanosecondDuration(total_seconds);
  CAPTURE(jkt_time);
  CAPTURE(ans);

  std::cout << "TARGET" << std::endl;
  std::cout << "START   : " << jkt_time << std::endl;
  std::cout << "END     : " << ans << std::endl;

  std::cout << "RESULT" << std::endl;
  std::cout << res << std::endl;

  REQUIRE(res == ans);
}

TEST_CASE("datetime-subtract-100-years-seconds", "[datetime][local-test]") {
  using DateTime = dates::DateTime;
  using namespace std::chrono;

  auto jkt_time = DateTime(2022, 7, 10, 0, 0, 0, 0, "Asia/Singapore");
  auto ans = DateTime(1922, 07, 11, 0, 0, 0, 0, "Asia/Singapore");

  // Define duration types with int64_t to prevent overflow
  using seconds = duration<int64_t>;

  constexpr int64_t seconds_per_minute = 60;
  constexpr int64_t minutes_per_hour = 60;
  constexpr int64_t hours_per_day = 24;
  constexpr int64_t days_per_normal_year = 365;
  constexpr int64_t days_per_leap_year = 366;
  constexpr int64_t normal_years = 76;
  constexpr int64_t leap_years = 24;

  // Calculate total seconds in normal and leap years
  auto total_seconds =
      seconds(hours_per_day * minutes_per_hour * seconds_per_minute) *
      (days_per_normal_year * normal_years + days_per_leap_year * leap_years);

  auto res = jkt_time - dates::ToNanosecondDuration(total_seconds);
  CAPTURE(jkt_time);
  CAPTURE(ans);

  std::cout << "TARGET" << std::endl;
  std::cout << "START   : " << jkt_time << std::endl;
  std::cout << "END     : " << ans << std::endl;

  std::cout << "RESULT" << std::endl;
  std::cout << res << std::endl;

  REQUIRE(res == ans);
}