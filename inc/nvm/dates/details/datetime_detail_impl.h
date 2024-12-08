#ifndef NVM_CORE_DATETIME_V2_DATETIME_DETAILS_DATETIME_DETAIL_IMPL_H
#define NVM_CORE_DATETIME_V2_DATETIME_DETAILS_DATETIME_DETAIL_IMPL_H

#include <chrono>
#include <cstdint>
#include <ctime>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <utility>

#include "date/tz.h"
#include "nvm/dates/datetime_part.h"

namespace nvm {
namespace dates {
namespace details {


[[nodiscard]] inline auto ToTzTime(
    const std::chrono::system_clock::time_point& time) {
  auto z = date::current_zone();
  return date::make_zoned(z, time);
}

[[nodiscard]] inline auto ToTzTime(
    const date::time_zone* tz,
    const std::chrono::system_clock::time_point& time) {
  return date::make_zoned(tz, time);
}

[[nodiscard]] inline auto ToTzTime(
    const std::string& tz_name,
    const std::chrono::system_clock::time_point& time) {
  return date::make_zoned(tz_name, time);
}

[[nodiscard]] inline auto ToTzTime(int32_t year, uint8_t month, uint8_t day,
                                   uint8_t hour, uint8_t minutes,
                                   uint8_t second, uint16_t milisecond,
                                   const std::string& tz_name) {
  if (month == 0 || month > 12) {
    throw std::range_error("Valid month is between 1 ~ 12.");
  }

  if (day == 0 || day > 31) {
    throw std::range_error("Valid day is between 1 ~ 31.");
  }

  if (hour > 23) {
    throw std::range_error("Valid hour is between 0 ~ 23.");
  }

  if (minutes > 59) {
    throw std::range_error("Valid minutes is between 0 ~ 59.");
  }

  if (second > 59) {
    throw std::range_error("Valid second is between 0 ~ 59.");
  }

  // Create a date::year_month_day object
  date::year_month_day ymd{date::year{year}, date::month{month},
                           date::day{day}};

  // Create a time of day in nanoseconds
  auto tod = date::hh_mm_ss<std::chrono::nanoseconds>(
      std::chrono::hours(hour) + std::chrono::minutes(minutes) +
      std::chrono::seconds(second) +
      (std::chrono::milliseconds((milisecond > 999) ? 999 : milisecond) *
       1'000'000)  // Clamping & Convert milliseconds to nanoseconds
  );

  // Combine the date and time of day to create a local_time type
  auto local_time = date::local_days(ymd) + tod.to_duration();

  // Create a zoned_time using the provided timezone name
  try {
    auto zoned_time =
        date::zoned_time<std::chrono::nanoseconds>(tz_name, local_time);
    return zoned_time;
  } catch (const std::exception& e) {
    std::string msg = "Exception creating zoned_time: ";
    msg += e.what();
    throw std::runtime_error(msg);
  }
}

[[nodiscard]] inline auto ToTzTime(int32_t year, uint8_t month, uint8_t day,
                                   uint8_t hour, uint8_t minutes,
                                   uint8_t second, uint16_t milisecond) {
  return ToTzTime(year, month, day, hour, minutes, second, milisecond,
                  date::current_zone()->name());
}

[[nodiscard]] inline auto Now() {
  return ToTzTime(std::chrono::system_clock::now());
}

[[nodiscard]] inline auto UtcNow() {
  return ToTzTime("Etc/Utc", std::chrono::system_clock::now());
}

[[nodiscard]] inline DateTimePart GetDateTimePart(
    const date::zoned_time<std::chrono::nanoseconds, const date::time_zone*>&
        time) {
  // Get the local time_point and time_zone info
  auto local_time = time.get_local_time();
  auto tz_info = time.get_time_zone()->get_info(local_time);

  // Date components
  auto dp = date::floor<date::days>(local_time);
  auto time_of_day =
      date::make_time(local_time - dp);  // date::time_of_day<nanoseconds>
  auto ymd = date::year_month_day{dp};

  DateTimePart part;
  part.year = static_cast<int32_t>(int(ymd.year()));
  part.month = static_cast<uint8_t>(unsigned(ymd.month()));
  part.day = static_cast<uint8_t>(unsigned(ymd.day()));
  part.hour = static_cast<uint8_t>(time_of_day.hours().count());
  part.minute = static_cast<uint8_t>(time_of_day.minutes().count());
  part.second = static_cast<uint8_t>(time_of_day.seconds().count());
  part.millisecond =
      static_cast<uint16_t>(time_of_day.subseconds().count() /
                            1'000'000);  // Convert nanoseconds to milliseconds
  part.nanosecond = static_cast<uint32_t>(time_of_day.subseconds().count());

  part.offset = tz_info.first.offset;
  part.is_daylight_saving = tz_info.first.save != std::chrono::seconds(0);

  part.is_leap_year = ymd.year().is_leap();

  return part;
}


}
}  // namespace dates
}  // namespace nvm

#endif