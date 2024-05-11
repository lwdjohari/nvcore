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
#include <sstream>  // For std::stringstream
#include <stdexcept>

namespace nvm::date {

inline std::chrono::system_clock::time_point Now() {
  // Get current time as time_t object
  std::time_t t = std::time(nullptr);
  // Convert to local time
  std::tm* local_time = std::localtime(&t);
  // Convert back to time_t and then to time_point
  std::time_t local_time_t = std::mktime(local_time);
  return std::chrono::system_clock::from_time_t(local_time_t);
}

inline std::chrono::system_clock::time_point UtcNow() {
  return std::chrono::system_clock::now();
}

inline std::string ToIso8601String(
    const std::chrono::system_clock::time_point& time, const bool& is_utc) {
  std::time_t t = std::chrono::system_clock::to_time_t(time);
  std::tm* time_tm;
  if (is_utc) {
    time_tm = std::gmtime(&t);
  } else {
    time_tm = std::localtime(&t);
  }

  std::stringstream ss;
  ss << std::put_time(time_tm, "%FT%T");
  if (is_utc) {
    ss << "Z";  // ISO 8601 UTC times end with 'Z'
  }

  return ss.str();
}

class DateTime {
 private:
  std::chrono::system_clock::time_point timepoint_;
  bool is_utc_;
  void EnsureSameTimeZone(const DateTime& other) const {
    if (is_utc_ != other.is_utc_) {
      throw std::logic_error(
          "Comparison between UTC and local time is not allowed");
    }
  }

 public:
  DateTime() : timepoint_(date::Now()), is_utc_(false){};
  explicit DateTime(const std::chrono::system_clock::time_point& time,
                    bool is_utc)
      : timepoint_(time), is_utc_(is_utc){};
  ~DateTime() {}

  const std::chrono::system_clock::time_point& Timepoint() const {
    return timepoint_;
  }

  bool IsUtc() const { return is_utc_; }

  std::string ToIso8601() const { return ToIso8601String(timepoint_, is_utc_); }

  static DateTime Now() { return DateTime(); }

  static DateTime UtcNow() { return DateTime(date::UtcNow(), true); }

  static DateTime ToUtc(const DateTime& local_datetime) {
    if (local_datetime.is_utc_) return local_datetime;  // Already UTC

    std::time_t tt =
        std::chrono::system_clock::to_time_t(local_datetime.timepoint_);
    std::tm* gmt = std::gmtime(&tt);        // Convert time_t to UTC
    std::time_t tt_utc = std::mktime(gmt);  // Convert tm to time_t as UTC
    return DateTime(std::chrono::system_clock::from_time_t(tt_utc), true);
  }

  static DateTime ToLocalTime(const DateTime& utc_datetime) {
    if (!utc_datetime.is_utc_) return utc_datetime;  // Already local

    std::time_t tt =
        std::chrono::system_clock::to_time_t(utc_datetime.timepoint_);
    std::tm* local_tm = std::localtime(&tt);  // Convert time_t to local time
    std::time_t tt_local =
        std::mktime(local_tm);  // Convert tm to time_t as local
    return DateTime(std::chrono::system_clock::from_time_t(tt_local), false);
  }

  DateTime ToLocalTime() const { return DateTime::ToLocalTime(*this); }

  DateTime ToUtc() const { return DateTime::ToUtc(*this); }
  
  std::chrono::milliseconds GetDuration(const DateTime& time) const {
    return *this - time;
  }

  bool operator==(const DateTime& other) const {
    EnsureSameTimeZone(other);
    return timepoint_ == other.timepoint_;
  }

  bool operator!=(const DateTime& other) const {
    EnsureSameTimeZone(other);
    return timepoint_ != other.timepoint_;
  }

  bool operator<(const DateTime& other) const {
    EnsureSameTimeZone(other);
    return timepoint_ < other.timepoint_;
  }

  bool operator<=(const DateTime& other) const {
    EnsureSameTimeZone(other);
    return timepoint_ <= other.timepoint_;
  }

  bool operator>(const DateTime& other) const {
    EnsureSameTimeZone(other);
    return timepoint_ > other.timepoint_;
  }

  bool operator>=(const DateTime& other) const {
    EnsureSameTimeZone(other);
    return timepoint_ >= other.timepoint_;
  }

  std::chrono::milliseconds operator-(const DateTime& other) const {
    EnsureSameTimeZone(other);
    auto duration = timepoint_ - other.timepoint_;
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::milliseconds(duration.count()));
  }

  // Overloading the + operator to add duration to DateTime
  DateTime operator+(const std::chrono::duration<int64_t>& duration) const {
    return DateTime(timepoint_ + duration, is_utc_);
  }

  // Overloading the - operator to subtract duration from DateTime
  DateTime operator-(const std::chrono::duration<int64_t>& duration) const {
    return DateTime(timepoint_ - duration, is_utc_);
  }
};
}  // namespace nvm::datetime
#endif
