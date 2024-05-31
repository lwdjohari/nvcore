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

  // Priority queue to keep track of the end times of currently running tasks
  std::priority_queue<int, std::vector<int>, std::greater<int>> pq;

  int currentTime = 0;

  for (int duration : taskDurations) {
    if (pq.size() == maxParallel) {
      currentTime = pq.top();  // Get the earliest finishing task
      pq.pop();
    }

    pq.push(currentTime + duration);
  }

  // The total execution time is the max end time in the priority queue
  while (pq.size() > 1) {
    pq.pop();
  }

  return pq.top();
}
}  // namespace nvm::threads::utils
