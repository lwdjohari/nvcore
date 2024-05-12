/*
 *  Copyright (c) 2024 Linggawasistha Djohari
 * <linggawasistha.djohari@outlook.com> Licensed to Linggawasistha Djohari under
 * one or more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 *
 *  Linggawasistha Djohari licenses this file to you under the Apache License,
 *  Version 2.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef NVM_CORE_DATETIME_V2_DATETIME_TZ_H
#define NVM_CORE_DATETIME_V2_DATETIME_TZ_H

#include <chrono>
#include <cstdint>
#include <ctime>
#include <optional>
#include <ostream>
#include <stdexcept>

#include "date/tz.h"

namespace nvm::dates {

inline auto ToTzTime(const std::chrono::system_clock::time_point& time) {
  auto z = date::current_zone();
  return date::make_zoned(z, time);
}

inline auto ToTzTime(const date::time_zone* tz,
                     const std::chrono::system_clock::time_point& time) {
  return date::make_zoned(tz, time);
}

inline auto ToTzTime(const std::string& tz_name,
                     const std::chrono::system_clock::time_point& time) {
  return date::make_zoned(tz_name, time);
}

inline auto ToTzTime(int32_t year, uint8_t month, uint8_t day, uint8_t hour,
                     uint8_t minutes, uint8_t second, uint16_t milisecond,
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

inline auto ToTzTime(int32_t year, uint8_t month, uint8_t day, uint8_t hour,
                     uint8_t minutes, uint8_t second, uint16_t milisecond) {
  return ToTzTime(year, month, day, hour, minutes, second, milisecond,
                  date::current_zone()->name());
}

inline const date::time_zone* GetTimezone(const std::string& tz_name) {
  auto z = date::locate_zone(tz_name);
  if (!z) {
    throw std::runtime_error("No timezone with name " + tz_name + " is found");
  }
  return z;
}

inline auto Now() { return ToTzTime(std::chrono::system_clock::now()); }

inline auto UtcNow() {
  return ToTzTime("Etc/Utc", std::chrono::system_clock::now());
}

struct DateTimePart {
  // Year as a signed integer (to handle BCE years if ever necessary)
  int32_t year;
  // Month as an unsigned integer (1-12)
  uint8_t month;
  // Day of the month as an unsigned integer (1-31)
  uint8_t day;
  // Hour of the day as an unsigned integer (0-23)
  uint8_t hour;
  // Minute of the hour as an unsigned integer (0-59)
  uint8_t minute;
  // Second of the minute as an unsigned integer (0-59)
  uint8_t second;
  // Indicates whether daylight saving time is in
  // effect
  bool is_daylight_saving;
  // UTC offset in seconds
  // Indicates whether the year is a leap year
  bool is_leap_year;
  std::chrono::seconds offset;
  // Milliseconds part of the second (0-999)
  uint16_t millisecond;
  // Nanoseconds part of the second (0-999999999)
  uint32_t nanosecond;

  DateTimePart()
      : year(),
        month(),
        day(),
        hour(),
        minute(),
        second(),
        is_daylight_saving(),
        is_leap_year(),
        offset(),
        millisecond(),
        nanosecond() {}

  explicit DateTimePart(int32_t year, uint8_t month, uint8_t day, uint8_t hour,
                        uint8_t minute, uint8_t second, uint16_t millisecond,
                        uint32_t nanosecond, const std::chrono::seconds& offset,
                        bool is_daylight_saving, bool is_leap_year)
      : year(year),
        month(month),
        day(day),
        hour(hour),
        minute(minute),
        second(second),
        is_daylight_saving(is_daylight_saving),
        is_leap_year(is_leap_year),
        offset(std::chrono::seconds(offset)),
        millisecond(millisecond),
        nanosecond(nanosecond) {}
};

DateTimePart GetDateTimePart(
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

/// @brief Class for handling datetime with or without timezone, the underlying
/// implementations are rely to the Howard Hinnant date library.
class DateTime {
 private:
  std::shared_ptr<
      date::zoned_time<std::chrono::nanoseconds, const date::time_zone*>>
      time_;
  std::shared_ptr<DateTimePart> part_;

 public:
  /// @brief Create DateTime with Now() value and host current timezone.
  DateTime()
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            dates::Now())),
        part_(std::make_shared<DateTimePart>(GetDateTimePart(*time_))){};

  /// @brief Copy/clone DateTime object
  /// @param other source of DateTime object to copy
  DateTime(const DateTime& other)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            *other.TzTime())),
        part_(std::make_shared<DateTimePart>(GetDateTimePart(*time_))) {}

  /// @brief Create DateTime object with Now() value with specific timezone.
  /// @param tz_name IANA timezone name
  explicit DateTime(const std::string& tz_name)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            ToTzTime(GetTimezone(tz_name), std::chrono::system_clock::now()))),
        part_(std::make_shared<DateTimePart>(GetDateTimePart(*time_))) {}

  /// @brief Create DateTime with custom date and time with current host
  /// timezone.
  /// @param year Positive and negative `int32_t` year
  /// @param month between 1 ~ 12
  /// @param day between 1 ~ 31
  /// @param hour between 0 ~ 23 second
  /// @param minutes between 0 ~ 59 minute
  /// @param second between 0 ~ 59 second
  /// @param milisecond between 0 ~ 999 milisecond
  explicit DateTime(int32_t year, uint8_t month, uint8_t day, uint8_t hour,
                    uint8_t minutes, uint8_t second, uint16_t milisecond)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            ToTzTime(year, month, day, hour, minutes, second, milisecond))),
        part_(std::make_shared<DateTimePart>(GetDateTimePart(*time_))) {}

  /// @brief Create DateTime with custom date and time with specified IANA
  /// Timezone.
  /// @param year Positive and negative `int32_t` year
  /// @param month between 1 ~ 12
  /// @param day between 1 ~ 31
  /// @param hour between 0 ~ 23 second
  /// @param minutes between 0 ~ 59 minute
  /// @param second between 0 ~ 59 second
  /// @param milisecond between 0 ~ 999 milisecond
  /// @param tz_name
  explicit DateTime(int32_t year, uint8_t month, uint8_t day, uint8_t hour,
                    uint8_t minutes, uint8_t second, uint16_t milisecond,
                    const std::string& tz_name)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            ToTzTime(year, month, day, hour, minutes, second, milisecond,
                     tz_name))),
        part_(std::make_shared<DateTimePart>(GetDateTimePart(*time_))) {}

  /// @brief Create DateTime with custom date and time set to 00:00 (midnight)
  /// with current host timezone.
  /// @param year Positive and negative `int32_t` year
  /// @param month between 1 ~ 12
  /// @param day between 1 ~ 31
  explicit DateTime(int32_t year, uint8_t month, uint8_t day)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            ToTzTime(year, month, day, 0, 0, 0, 0))),
        part_(std::make_shared<DateTimePart>(GetDateTimePart(*time_))) {}

  /// @brief Create DateTime with custom date and time set to 00:00 (midnight)
  /// with specified IANA Timezone.
  /// @param year Positive and negative `int32_t` year
  /// @param month between 1 ~ 12
  /// @param day between 1 ~ 31
  explicit DateTime(int32_t year, uint8_t month, uint8_t day,
                    const std::string& tz_name)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            ToTzTime(year, month, day, 0, 0, 0, 0, tz_name))),
        part_(std::make_shared<DateTimePart>(GetDateTimePart(*time_))) {}

  /// @brief Create DateTime object from `std::chrono::system_clock::time_point`
  /// with host timezone.
  /// @param time
  explicit DateTime(const std::chrono::system_clock::time_point& time)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            ToTzTime(time))),
        part_(std::make_shared<DateTimePart>(GetDateTimePart(*time_))) {}

  /// @brief Create DateTime object from `std::chrono::system_clock::time_point`
  /// and specified IANA Timezone.
  /// @param time
  /// @param tz_name
  explicit DateTime(const std::chrono::system_clock::time_point& time,
                    const std::string& tz_name)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            ToTzTime(GetTimezone(tz_name), time))),
        part_(std::make_shared<DateTimePart>(GetDateTimePart(*time_))) {}

  explicit DateTime(date::zoned_time<std::chrono::nanoseconds,
                                     const date::time_zone*>&& time_tz)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            std::forward<date::zoned_time<std::chrono::nanoseconds,
                                          const date::time_zone*>>(time_tz))),
        part_(std::make_shared<DateTimePart>(GetDateTimePart(*time_))) {}

  ~DateTime() {}

  /// @brief Get the DateTime value
  /// @return
  const std::shared_ptr<
      date::zoned_time<std::chrono::nanoseconds, const date::time_zone*>>
  TzTime() const {
    return time_;
  }

  /// @brief Return the DateTime parts of current datetime value
  /// @return
  const std::shared_ptr<DateTimePart> TimeParts() const { return part_; }

  /// @brief Get IANA Timezone for this current DateTime object
  /// @return
  const std::string TzName() const {
    return std::string(time_->get_time_zone()->name());
  }

  /// @brief Get Is UTC Timezone or not
  /// @return
  bool IsUtc() const { return time_->get_time_zone()->name() == "Etc/UTC"; }

  /// @brief Get Iso8601 datetime string representation
  /// @return
  std::string ToIso8601() const { return date::format("%FT%T%z", *time_); }

  /// @brief Create DateTime object with host current timezone and current time.
  /// @return
  static DateTime Now() { return DateTime(); }

  /// @brief Create DateTime object with UTC timezone and current UTC time.
  /// @return
  static DateTime UtcNow() { return DateTime("Etc/UTC"); }

  /// @brief Get host current timezone.
  /// @return
  static std::string GetHostTimezoneName() {
    return std::string(date::current_zone()->name());
  }

  DateTime ToUtc() const {
    if (time_->get_time_zone()->name() == "Etc/UTC") {
      return DateTime(*this);
    }

    return DateTime(date::zoned_time("Etc/UTC",*time_ ));
  }

  DateTime ToTimezone(const std::string& tz_name) const {
    if (time_->get_time_zone()->name() == tz_name) {
      return DateTime(*this);
    }

    return DateTime(date::zoned_time(tz_name,*time_ ));
  }

  bool operator==(const DateTime& other) const {
    if (!other.time_ || !time_) return false;

    return time_->get_sys_time() == other.time_->get_sys_time();
  }

  bool operator!=(const DateTime& other) const {
    if (!other.time_ || !time_) return false;

    return time_->get_sys_time() != other.time_->get_sys_time();
  }

  bool operator<(const DateTime& other) const {
    if (!other.time_ || !time_) return false;

    return time_->get_sys_time() < other.time_->get_sys_time();
  }

  bool operator<=(const DateTime& other) const {
    if (!other.time_ || !time_) return false;

    return time_->get_sys_time() <= other.time_->get_sys_time();
  }

  bool operator>(const DateTime& other) const {
    if (!other.time_ || !time_) return false;

    return time_->get_sys_time() > other.time_->get_sys_time();
  }

  bool operator>=(const DateTime& other) const {
    if (!other.time_ || !time_) return false;

    return time_->get_sys_time() >= other.time_->get_sys_time();
  }

  friend DateTime operator+(const DateTime& dt,
                            const std::chrono::duration<int64_t>& duration);
  friend DateTime operator-(const DateTime& dt,
                            const std::chrono::duration<int64_t>& duration);

  friend std::optional<std::chrono::nanoseconds> operator-(const DateTime& rv,
                                                           const DateTime& lv);

  explicit operator std::string() const {
    return date::format("%FT%T%z", *time_);
  }

  friend std::ostream& operator<<(std::ostream& os, const DateTime& dt) {
    os << static_cast<std::string>(dt);
    return os;
  }
};

enum class DateTimeCalculateSpanType : uint8_t {
  DurationAdd = 0,
  DurationSubtract = 1,
};

template <typename TDuration>
DateTime CalculateDurationSpan(const DateTime& source,
                               std::chrono::duration<TDuration> duration,
                               DateTimeCalculateSpanType calc_type) {
  // calculate from local time not from sys_time().
  // sys_time() as time_point is platform dependent
  // and could create UB

  auto local_time = source.TzTime()->get_local_time();

  auto local_time_after_duration =
      calc_type == DateTimeCalculateSpanType::DurationAdd
          ? local_time + duration
          : local_time - duration;

  // Convert back to zoned_time for correct time zone handling
  auto zoned_time_after_duration = date::make_zoned(
      source.TzTime()->get_time_zone(), local_time_after_duration);

  return DateTime(std::move(zoned_time_after_duration));
}

inline std::optional<std::chrono::nanoseconds> CalculateDurationBetween(
    const DateTime& dt1, const DateTime& dt2) {
  // Ensure both times are in the same time zone for accurate comparison
  if (dt1.TzTime()->get_time_zone() != dt2.TzTime()->get_time_zone()) {
    return std::nullopt;
  }

  // Calculate duration by subtracting their system times
  auto duration = dt1.TzTime()->get_sys_time() - dt2.TzTime()->get_sys_time();
  return duration;
}

// Overloading the + operator to add duration to DateTime
DateTime operator+(const DateTime& dt,
                   const std::chrono::duration<int64_t>& duration) {
  return CalculateDurationSpan(dt, duration,
                               DateTimeCalculateSpanType::DurationAdd);
}

// Overloading the - operator to subtract duration from DateTime
DateTime operator-(const DateTime& dt,
                   const std::chrono::duration<int64_t>& duration) {
  return CalculateDurationSpan(dt, duration,
                               DateTimeCalculateSpanType::DurationSubtract);
}

// Overloading the - operator to subtract DateTime and DateTime object
std::optional<std::chrono::nanoseconds> operator-(const DateTime& lv,
                                                  const DateTime& rv) {
  return CalculateDurationBetween(lv, rv);
}

}  // namespace nvm::dates
#endif
