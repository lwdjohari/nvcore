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
#include <utility>

#include "date/tz.h"
#include "nvm/dates/datetime_part.h"
#include "nvm/dates/details/datetime_detail_impl.h"

namespace nvm {
namespace dates {

/// @brief Get Timezone Info
/// @param tz_name IANA Timezone name
/// @return
[[nodiscard]] inline const date::time_zone* GetTimezone(
    const std::string& tz_name) {
  auto z = date::locate_zone(tz_name);
  if (!z) {
    throw std::runtime_error("No timezone with name " + tz_name + " is found");
  }
  return z;
}

/// @brief Safe cast chrono::duration to chrono::nanoseconds
/// @tparam T std::chrono duration
/// @param value
template <typename T = std::chrono::seconds>
[[nodiscard]] inline std::chrono::nanoseconds ToNanosecondDuration(
    const T& value) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(value);
}

/// @brief Class for handling datetime with or without timezone, the underlying
/// implementations are rely to the Howard Hinnant date library.
class DateTime {
 private:
  std::shared_ptr<
      date::zoned_time<std::chrono::nanoseconds, const date::time_zone*>>
      time_;
  std::shared_ptr<DateTimePart> part_;

  std::pair<uint8_t, int32_t> DetermineNextMonth() const;

 public:
  /// @brief Create DateTime with Now() value and host current timezone.
  DateTime();

  /// @brief Copy/clone DateTime object
  /// @param other source of DateTime object to copy
  DateTime(const DateTime& other);

  /// @brief Create DateTime object with Now() value with specific timezone.
  /// @param tz_name IANA timezone name
  explicit DateTime(const std::string& tz_name);

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
                    uint8_t minutes, uint8_t second, uint16_t milisecond);

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
                    const std::string& tz_name);

  /// @brief Create DateTime with custom date and time set to 00:00 (midnight)
  /// with current host timezone.
  /// @param year Positive and negative `int32_t` year
  /// @param month between 1 ~ 12
  /// @param day between 1 ~ 31
  explicit DateTime(int32_t year, uint8_t month, uint8_t day);

  /// @brief Create DateTime with custom date and time set to 00:00 (midnight)
  /// with specified IANA Timezone.
  /// @param year Positive and negative `int32_t` year
  /// @param month between 1 ~ 12
  /// @param day between 1 ~ 31
  explicit DateTime(int32_t year, uint8_t month, uint8_t day,
                    const std::string& tz_name);

  /// @brief Create DateTime object from `std::chrono::system_clock::time_point`
  /// with host timezone.
  /// @param time
  explicit DateTime(const std::chrono::system_clock::time_point& time);

  /// @brief Create DateTime object from `std::chrono::system_clock::time_point`
  /// and specified IANA Timezone.
  /// @param time
  /// @param tz_name
  explicit DateTime(const std::chrono::system_clock::time_point& time,
                    const std::string& tz_name);

  explicit DateTime(date::zoned_time<std::chrono::nanoseconds,
                                     const date::time_zone*>&& time_tz);

  ~DateTime();

  /// @brief Get the DateTime value
  /// @return
  const std::shared_ptr<
      date::zoned_time<std::chrono::nanoseconds, const date::time_zone*>>
  TzTime() const;

  /// @brief Return the DateTime parts of current datetime value
  /// @return
  const std::shared_ptr<DateTimePart> TimeParts() const;

  /// @brief Get IANA Timezone for this current DateTime object
  /// @return
  const std::string TzName() const;

  /// @brief Get Is UTC Timezone or not
  /// @return
  bool IsUtc() const;

  /// @brief Get Iso8601 datetime string representation
  /// @return
  [[nodiscard]] std::string ToIso8601() const;

  /// @brief Create DateTime object with host current timezone and current time.
  /// @return
  [[nodiscard]] static DateTime Now();

  /// @brief Create DateTime object with UTC timezone and current UTC time.
  /// @return
  [[nodiscard]] static DateTime UtcNow();

  /// @brief Get host current timezone.
  /// @return
  [[nodiscard]] static std::string GetHostTimezoneName();

  /// @brief Add current DateTime by duration.
  /// @tparam T std::chrono::duration<T>
  /// @param duration value of duration
  /// @return
  template <typename T = std::chrono::seconds>
  [[nodiscard]] DateTime Add(const T& duration) {
    return *this +
           std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
  }

  /// @brief Subtract current DateTime by duration.
  /// @tparam T std::chrono::duration<T>
  /// @param duration value of duration
  /// @return
  template <typename T = std::chrono::seconds>
  [[nodiscard]] DateTime Subtract(const T& duration) {
    return *this -
           std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
  }

  /// @brief Get duration span from current datetime and supplied datetime.
  /// @tparam T std::chrono::duration<T>
  /// @param other value to subtract
  /// @return
  template <typename T = std::chrono::seconds>
  [[nodiscard]] T GetDurationSpan(const DateTime& other) {
    return std::chrono::duration_cast<T>(*this - other);
  }

  /// @brief Convert to UTC timezone.
  /// @return
  [[nodiscard]] DateTime ToUtc() const;

  /// @brief Convert to specific IANA timezone.
  /// @param tz_name
  /// @return
  [[nodiscard]] DateTime ToTimezone(const std::string& tz_name) const;

  /// @brief Get datetime on start of month from current datetime.
  /// @return
  [[nodiscard]] DateTime GetStartOfMonth() const;

  /// @brief
  /// @return
  [[nodiscard]] DateTime GetEndOfMonth() const;

  /// @brief Format datetime to string based on pattern format
  /// @param format
  /// @return
  std::string ToString(const std::string& format);

  bool operator==(const DateTime& other) const;

  bool operator!=(const DateTime& other) const;

  bool operator<(const DateTime& other) const;

  bool operator<=(const DateTime& other) const;

  bool operator>(const DateTime& other) const;

  bool operator>=(const DateTime& other) const;

  friend DateTime operator+(const DateTime& dt,
                            const std::chrono::nanoseconds& duration);

  friend DateTime operator-(const DateTime& dt,
                            const std::chrono::nanoseconds& duration);

  friend std::optional<std::chrono::nanoseconds> operator-(const DateTime& rv,
                                                           const DateTime& lv);

  explicit operator std::string() const;

  friend std::ostream& operator<<(std::ostream& os, const DateTime& dt) {
    os << static_cast<std::string>(dt);
    return os;
  }
};

namespace details {
enum class DateTimeCalculateSpanType : uint8_t {
  DurationAdd = 0,
  DurationSubtract = 1,
};

template <typename TDuration = std::chrono::nanoseconds>
[[nodiscard]] inline DateTime CalculateDurationSpan(
    const DateTime& source, TDuration duration,
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
}  // namespace details

/// @brief
/// @param value
/// @return
[[nodiscard]] inline std::optional<std::string> ToIso8601Optional(
    const std::optional<DateTime>& value) {
  return value.has_value() ? static_cast<std::string>(value.value())
                           : std::optional<std::string>(std::nullopt);
}

/// @brief Overloading the + operator to add duration to DateTime
/// @param dt
/// @param duration
/// @return
inline DateTime operator+(const DateTime& dt,
                          const std::chrono::nanoseconds& duration) {
  return dates::details::CalculateDurationSpan(
      dt, duration, dates::details::DateTimeCalculateSpanType::DurationAdd);
}

/// @brief Overloading the - operator to subtract duration from DateTime
/// @param dt
/// @param duration
/// @return
inline DateTime operator-(const DateTime& dt,
                          const std::chrono::nanoseconds& duration) {
  return dates::details::CalculateDurationSpan(
      dt, duration,
      dates::details::DateTimeCalculateSpanType::DurationSubtract);
}

/// @brief Overloading the - operator to subtract DateTime and DateTime object
/// @param lv
/// @param rv
/// @return
inline std::optional<std::chrono::nanoseconds> operator-(const DateTime& lv,
                                                         const DateTime& rv) {
  return dates::details::CalculateDurationBetween(lv, rv);
}

}  // namespace dates
}  // namespace nvm
#endif
