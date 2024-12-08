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

#ifndef NVM_CORE_DATETIME_V2_DATETIME_PART_H
#define NVM_CORE_DATETIME_V2_DATETIME_PART_H

#include <chrono>
#include <cstdint>
#include <ctime>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <utility>

#include "date/tz.h"

namespace nvm {
namespace dates {
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

}  // namespace dates
}  // namespace nvm

#endif