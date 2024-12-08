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

#include "nvm/dates/datetime.h"

namespace nvm {
namespace dates {

// private:

std::pair<uint8_t, int32_t> DateTime::DetermineNextMonth() const {
  uint8_t next_month;
  int32_t next_year;

  if (part_->month >= 12) {
    next_month = 1;
    next_year = part_->year + 1;
  } else {
    next_month = part_->month + 1;
    next_year = part_->year;
  }

  return std::make_pair(next_month, next_year);
}

// public:

DateTime::DateTime()
                : time_(std::make_shared<date::zoned_time<
                            std::chrono::nanoseconds, const date::time_zone*>>(
                      dates::details::Now())),
                  part_(std::make_shared<DateTimePart>(
                      dates::details::GetDateTimePart(*time_))) {};

DateTime::DateTime(const DateTime& other)
                : time_(std::make_shared<date::zoned_time<
                            std::chrono::nanoseconds, const date::time_zone*>>(
                      *other.TzTime())),
                  part_(std::make_shared<DateTimePart>(
                      dates::details::GetDateTimePart(*time_))) {}

DateTime::DateTime(const std::string& tz_name)
                : time_(std::make_shared<date::zoned_time<
                            std::chrono::nanoseconds, const date::time_zone*>>(
                      dates::details::ToTzTime(
                          GetTimezone(tz_name),
                          std::chrono::system_clock::now()))),
                  part_(std::make_shared<DateTimePart>(
                      dates::details::GetDateTimePart(*time_))) {}

DateTime::DateTime(int32_t year, uint8_t month, uint8_t day, uint8_t hour,
                   uint8_t minutes, uint8_t second, uint16_t milisecond)
                : time_(std::make_shared<date::zoned_time<
                            std::chrono::nanoseconds, const date::time_zone*>>(
                      dates::details::ToTzTime(year, month, day, hour, minutes,
                                               second, milisecond))),
                  part_(std::make_shared<DateTimePart>(
                      dates::details::GetDateTimePart(*time_))) {}

DateTime::DateTime(int32_t year, uint8_t month, uint8_t day, uint8_t hour,
                   uint8_t minutes, uint8_t second, uint16_t milisecond,
                   const std::string& tz_name)
                : time_(std::make_shared<date::zoned_time<
                            std::chrono::nanoseconds, const date::time_zone*>>(
                      dates::details::ToTzTime(year, month, day, hour, minutes,
                                               second, milisecond, tz_name))),
                  part_(std::make_shared<DateTimePart>(
                      dates::details::GetDateTimePart(*time_))) {}

DateTime::DateTime(int32_t year, uint8_t month, uint8_t day)
                : time_(std::make_shared<date::zoned_time<
                            std::chrono::nanoseconds, const date::time_zone*>>(
                      dates::details::ToTzTime(year, month, day, 0, 0, 0, 0))),
                  part_(std::make_shared<DateTimePart>(
                      dates::details::GetDateTimePart(*time_))) {}

DateTime::DateTime(int32_t year, uint8_t month, uint8_t day,
                   const std::string& tz_name)
                : time_(std::make_shared<date::zoned_time<
                            std::chrono::nanoseconds, const date::time_zone*>>(
                      dates::details::ToTzTime(year, month, day, 0, 0, 0, 0,
                                               tz_name))),
                  part_(std::make_shared<DateTimePart>(
                      dates::details::GetDateTimePart(*time_))) {}

DateTime::DateTime(const std::chrono::system_clock::time_point& time)
                : time_(std::make_shared<date::zoned_time<
                            std::chrono::nanoseconds, const date::time_zone*>>(
                      dates::details::ToTzTime(time))),
                  part_(std::make_shared<DateTimePart>(
                      dates::details::GetDateTimePart(*time_))) {}

DateTime::DateTime(const std::chrono::system_clock::time_point& time,
                   const std::string& tz_name)
                : time_(std::make_shared<date::zoned_time<
                            std::chrono::nanoseconds, const date::time_zone*>>(
                      dates::details::ToTzTime(GetTimezone(tz_name), time))),
                  part_(std::make_shared<DateTimePart>(
                      dates::details::GetDateTimePart(*time_))) {}

DateTime::DateTime(date::zoned_time<std::chrono::nanoseconds,
                                    const date::time_zone*>&& time_tz)
                : time_(std::make_shared<date::zoned_time<
                            std::chrono::nanoseconds, const date::time_zone*>>(
                      std::forward<date::zoned_time<std::chrono::nanoseconds,
                                                    const date::time_zone*>>(
                          time_tz))),
                  part_(std::make_shared<DateTimePart>(
                      dates::details::GetDateTimePart(*time_))) {}

DateTime::~DateTime() {}

std::shared_ptr<
    date::zoned_time<std::chrono::nanoseconds, const date::time_zone*>>
DateTime::TzTime() const {
  return time_;
}

std::shared_ptr<DateTimePart> DateTime::TimeParts() const {
  return part_;
}

 std::string DateTime::TzName() const {
  return std::string(time_->get_time_zone()->name());
}

bool DateTime::IsUtc() const {
  return time_->get_time_zone()->name() == "Etc/UTC";
}

[[nodiscard]] std::string DateTime::ToIso8601() const {
  return date::format("%FT%T%z", *time_);
}

// static

[[nodiscard]] DateTime DateTime::Now() {
  return DateTime();
}

// static

[[nodiscard]] DateTime DateTime::UtcNow() {
  return DateTime("Etc/UTC");
}

// static

[[nodiscard]] std::string DateTime::GetHostTimezoneName() {
  return std::string(date::current_zone()->name());
}

[[nodiscard]] DateTime DateTime::ToUtc() const {
  if (time_->get_time_zone()->name() == "Etc/UTC") {
    return DateTime(*this);
  }

  return DateTime(date::zoned_time("Etc/UTC", *time_));
}

[[nodiscard]] DateTime DateTime::ToTimezone(const std::string& tz_name) const {
  if (time_->get_time_zone()->name() == tz_name) {
    return DateTime(*this);
  }

  return DateTime(date::zoned_time(tz_name, *time_));
}

[[nodiscard]] DateTime DateTime::GetStartOfMonth() const {
  auto curr_date = date::floor<date::days>(time_->get_local_time());

  date::year_month_day ymd{curr_date};

  auto zoned_first_day_start =
      DateTime(static_cast<int32_t>(ymd.year()),
               static_cast<uint8_t>(static_cast<unsigned>(ymd.month())), 1,
               time_->get_time_zone()->name());

  return DateTime(zoned_first_day_start);
}

[[nodiscard]] DateTime DateTime::GetEndOfMonth() const {
  auto next_month = DetermineNextMonth();

  auto first_day_of_next_month = DateTime(next_month.second, next_month.first,
                                          1, time_->get_time_zone()->name());

  std::chrono::seconds seconds(1);

  return first_day_of_next_month -
         std::chrono::duration_cast<std::chrono::nanoseconds>(seconds);
}

std::string DateTime::ToString(const std::string& format) {
  return date::format(format, *time_);
}

bool DateTime::operator==(const DateTime& other) const {
  if (!other.time_ || !time_) {
    return false;
  }

  return time_->get_sys_time() == other.time_->get_sys_time();
}

bool DateTime::operator!=(const DateTime& other) const {
  if (!other.time_ || !time_) {
    return false;
  }

  return time_->get_sys_time() != other.time_->get_sys_time();
}

bool DateTime::operator<(const DateTime& other) const {
  if (!other.time_ || !time_) {
    return false;
  }

  return time_->get_sys_time() < other.time_->get_sys_time();
}

bool DateTime::operator<=(const DateTime& other) const {
  if (!other.time_ || !time_) {
    return false;
  }

  return time_->get_sys_time() <= other.time_->get_sys_time();
}

bool DateTime::operator>(const DateTime& other) const {
  if (!other.time_ || !time_) {
    return false;
  }

  return time_->get_sys_time() > other.time_->get_sys_time();
}

bool DateTime::operator>=(const DateTime& other) const {
  if (!other.time_ || !time_) {
    return false;
  }

  return time_->get_sys_time() >= other.time_->get_sys_time();
}

DateTime::operator std::string() const {
  return date::format("%FT%T%z", *time_);
}

}  // namespace dates
}  // namespace nvm
