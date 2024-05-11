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

#ifndef NVM_CORE_DATETIME_V2_DATETIME_H
#define NVM_CORE_DATETIME_V2_DATETIME_H

#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>  // For std::put_time
#include <iostream>
#include <memory>
#include <sstream>  // For std::stringstream
#include <stdexcept>

#include "date/tz.h"



namespace nvm::dates {

inline auto
ToTzTime(const std::chrono::system_clock::time_point& time) {
  auto z = date::current_zone();
  return date::make_zoned(z, time);
}

inline auto
ToTzTime(const date::time_zone* tz,
         const std::chrono::system_clock::time_point& time) {
  return date::make_zoned(tz, time);
}

inline auto
ToTzTime(const std::string& tz_name,
         const std::chrono::system_clock::time_point& time) {
  return date::make_zoned(tz_name, time);
}

inline const date::time_zone* GetTimezone(const std::string& tz_name) {
  return date::locate_zone(tz_name);
}

inline auto
Now() {
  return ToTzTime(std::chrono::system_clock::now());
}

inline auto
UtcNow() {
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
      local_time_;

 public:
  DateTime()
      : local_time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                      const date::time_zone*>>(
            dates::Now())){};

  explicit DateTime(const std::string& tz_name)
      : local_time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                      const date::time_zone*>>(
            dates::ToTzTime(GetTimezone(tz_name),
                            std::chrono::system_clock::now()))) {}

  DateTime(const DateTime& other)
      : local_time_(std::make_shared<date::zoned_time<std::chrono::nanoseconds,
                                                      const date::time_zone*>>(
            *other.TzTime())) {}

  ~DateTime() {}

  const std::shared_ptr<
      date::zoned_time<std::chrono::nanoseconds, const date::time_zone*>>
  TzTime() const {
    return local_time_;
  }

  const std::string TzName() const {
    return std::string(local_time_->get_time_zone()->name());
  }

  bool IsUtc() const {
    return local_time_->get_time_zone()->name() == "Etc/UTC";
  }

  std::string ToIso8601() const { return ToIso8601String(*local_time_); }

  static DateTime Now() { return DateTime(); }

  static DateTime UtcNow() { return DateTime("Etc/UTC"); }

  // DateTime ToLocalTime() const { return DateTime::ToLocalTime(*this); }

  // DateTime ToUtc() const { return DateTime::ToUtc(*this); }

  // std::chrono::milliseconds GetDuration(const DateTime& time) const {
  //   return *this - time;
  // }

  // bool operator==(const DateTime& other) const {
  //   EnsureSameTimeZone(other);
  //   return timepoint_ == other.timepoint_;
  // }

  // bool operator!=(const DateTime& other) const {
  //   EnsureSameTimeZone(other);
  //   return timepoint_ != other.timepoint_;
  // }

  // bool operator<(const DateTime& other) const {
  //   EnsureSameTimeZone(other);
  //   return timepoint_ < other.timepoint_;
  // }

  // bool operator<=(const DateTime& other) const {
  //   EnsureSameTimeZone(other);
  //   return timepoint_ <= other.timepoint_;
  // }

  // bool operator>(const DateTime& other) const {
  //   EnsureSameTimeZone(other);
  //   return timepoint_ > other.timepoint_;
  // }

  // bool operator>=(const DateTime& other) const {
  //   EnsureSameTimeZone(other);
  //   return timepoint_ >= other.timepoint_;
  // }

  // std::chrono::milliseconds operator-(const DateTime& other) const {
  //   EnsureSameTimeZone(other);
  //   auto duration = timepoint_ - other.timepoint_;
  //   return std::chrono::duration_cast<std::chrono::milliseconds>(
  //       std::chrono::milliseconds(duration.count()));
  // }

  // // Overloading the + operator to add duration to DateTime
  // DateTime operator+(const std::chrono::duration<int64_t>& duration) const {
  //   return DateTime(timepoint_ + duration, is_utc_);
  // }

  // // Overloading the - operator to subtract duration from DateTime
  // DateTime operator-(const std::chrono::duration<int64_t>& duration) const {
  //   return DateTime(timepoint_ - duration, is_utc_);
  // }
};
}  // namespace nvm::dates
#endif
