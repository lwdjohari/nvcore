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

  if (milisecond > 999) {
    milisecond = 999;
  }

  auto z = date::locate_zone(tz_name);
  if (!z) {
    throw std::runtime_error("No timezone with name " + tz_name + " is found");
  }

  return date::make_zoned(z);
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

inline std::string ToIso8601String(
    const date::zoned_time<std::chrono::nanoseconds, const date::time_zone*>&
        tz_time) {
  return date::format("%FT%TZ", tz_time.get_local_time());
}



class DateTime {
 private:
  std::shared_ptr<
      date::zoned_time<std::chrono::nanoseconds, const date::time_zone*>>
      time_;

 public:
  /// @brief Create DateTime with Now() value and host current timezone.
  DateTime()
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            dates::Now())){};

  /// @brief Copy/clone DateTime object
  /// @param other source of DateTime object to copy
  DateTime(const DateTime& other)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            *other.TzTime())) {}

  /// @brief Create DateTime object with Now() value with specific timezone.
  /// @param tz_name IANA timezone name
  explicit DateTime(const std::string& tz_name)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            ToTzTime(GetTimezone(tz_name), std::chrono::system_clock::now()))) {
  }

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
            ToTzTime(year, month, day, hour, minutes, second, milisecond))) {}

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
                     tz_name))) {}

  /// @brief Create DateTime with custom date and time set to 00:00 (midnight)
  /// with current host timezone.
  /// @param year Positive and negative `int32_t` year
  /// @param month between 1 ~ 12
  /// @param day between 1 ~ 31
  explicit DateTime(int32_t year, uint8_t month, uint8_t day)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            ToTzTime(year, month, day, 0, 0, 0, 0))) {}

  /// @brief Create DateTime with custom date and time set to 00:00 (midnight)
  /// with specified IANA Timezone.
  /// @param year Positive and negative `int32_t` year
  /// @param month between 1 ~ 12
  /// @param day between 1 ~ 31
  explicit DateTime(int32_t year, uint8_t month, uint8_t day,
                    const std::string& tz_name)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            ToTzTime(year, month, day, 0, 0, 0, 0, tz_name))) {}

  /// @brief Create DateTime object from `std::chrono::system_clock::time_point`
  /// with host timezone.
  /// @param time
  explicit DateTime(const std::chrono::system_clock::time_point& time)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            ToTzTime(time))) {}

  /// @brief Create DateTime object from `std::chrono::system_clock::time_point`
  /// and specified IANA Timezone.
  /// @param time
  /// @param tz_name
  explicit DateTime(const std::chrono::system_clock::time_point& time,
                    const std::string& tz_name)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            ToTzTime(GetTimezone(tz_name), time))) {}

  explicit DateTime(date::zoned_time<std::chrono::nanoseconds,
                                     const date::time_zone*>&& time_tz)
      : time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                const date::time_zone*>>(
            std::forward<date::zoned_time<std::chrono::nanoseconds,
                                          const date::time_zone*>>(time_tz))) {}

  ~DateTime() {}

  /// @brief Get the DateTime value
  /// @return
  const std::shared_ptr<
      date::zoned_time<std::chrono::nanoseconds, const date::time_zone*>>
  TzTime() const {
    return time_;
  }

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
  std::string ToIso8601() const { return ToIso8601String(*time_); }

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

  // DateTime ToLocalTime() const { return DateTime::ToLocalTime(*this); }

  // DateTime ToUtc() const { return DateTime::ToUtc(*this); }

  // std::chrono::milliseconds GetDuration(const DateTime& time) const {
  //   return *this - time;
  // }

  bool operator==(const DateTime& other) const {
    if (!other.time_ || !time_) return false;

    return time_ == other.time_;
  }

  bool operator!=(const DateTime& other) const {
    if (!other.time_ || !time_) return false;

    return time_ != other.time_;
  }

  bool operator<(const DateTime& other) const {
    if (!other.time_ || !time_) return false;

    return time_ < other.time_;
  }

  bool operator<=(const DateTime& other) const {
    if (!other.time_ || !time_) return false;

    return time_ <= other.time_;
  }

  bool operator>(const DateTime& other) const {
    if (!other.time_ || !time_) return false;

    return time_ > other.time_;
  }

  bool operator>=(const DateTime& other) const {
    if (!other.time_ || !time_) return false;

    return time_ >= other.time_;
  }

  friend DateTime operator+(const DateTime& dt,
                            const std::chrono::duration<int64_t>& duration);
  friend DateTime operator-(const DateTime& dt,
                            const std::chrono::duration<int64_t>& duration);

  

  // std::chrono::milliseconds operator-(const DateTime& other) const {
  //   EnsureSameTimeZone(other);
  //   auto duration = timepoint_ - other.timepoint_;
  //   return std::chrono::duration_cast<std::chrono::milliseconds>(
  //       std::chrono::milliseconds(duration.count()));
  // }
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

// Overloading the + operator to add duration to DateTime
DateTime operator+(const DateTime& dt, const std::chrono::duration<int64_t>& duration) {
    return CalculateDurationSpan(dt, duration, DateTimeCalculateSpanType::DurationAdd);
}

// Overloading the - operator to subtract duration from DateTime
DateTime operator-(const DateTime& dt, const std::chrono::duration<int64_t>& duration) {
    return CalculateDurationSpan(dt, duration, DateTimeCalculateSpanType::DurationSubtract);
}

}  // namespace nvm::dates
#endif
