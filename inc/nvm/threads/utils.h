#pragma once

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <vector>

namespace nvm::threads::utils {
inline uint32_t CalculateMaxExecutionTime(
    const std::vector<uint32_t>& taskDurations, uint32_t maxParallel) {
  if (taskDurations.empty() || maxParallel == 0) {
    return 0;
  }

//   // Priority queue to keep track of the end times of currently running tasks
//   std::priority_queue<int, std::vector<int>, std::greater<int>> pq;

//   int currentTime = 0;

//   for (int duration : taskDurations) {
//     if (pq.size() == maxParallel) {
//       currentTime = pq.top();  // Get the earliest finishing task
//       pq.pop();
//     }

//     pq.push(currentTime + duration);
//   }

//   // The total execution time is the max end time in the priority queue
//   while (pq.size() > 1) {
//     pq.pop();
//   }

//   return pq.top();

  std::vector<uint32_t> endTimes;
    endTimes.reserve(maxParallel);

    for (const uint32_t& duration : taskDurations) {
        if (endTimes.size() < static_cast<size_t>(maxParallel)) {
            endTimes.push_back(duration);
        } else {
            // Find the task that finishes the earliest and start the new task there
            auto earliestEndTimeIt = std::min_element(endTimes.begin(), endTimes.end());
            *earliestEndTimeIt += duration;
        }
    }

    // The total execution time is the maximum end time
    return *std::max_element(endTimes.begin(), endTimes.end());
}
}  // namespace nvm::threads::utils
