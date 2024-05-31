#pragma once
#include <atomic>
#include <utility>
#include <vector>

#include "nvm/threads/task_impl.h"
#include "nvm/threads/task_pool.h"

namespace nvm::threads::task {

// Helper function to get the future result for both std::future and
// std::shared_future
template <typename T>
T get_future_result(std::future<T>& future) {
  return future.get();
}

template <typename T>
T get_future_result(std::shared_future<T>& future) {
  return future.get();
}

// Helper struct to collect results into a tuple
template <typename... Futures>
struct ResultsCollector {
  std::tuple<Futures...> futures;

  ResultsCollector(Futures&&... futures)
                  : futures(std::forward<Futures>(futures)...) {}

  template <std::size_t... I>
  auto collect_results(std::index_sequence<I...>) {
    return std::make_tuple(
        std::move(get_future_result(std::get<I>(futures)))...);
  }

  auto collect() {
    return collect_results( std::move( std::index_sequence_for<Futures...>{}));
  }
};

template <typename... Tasks>
auto AsyncWaitAll(std::shared_ptr<class TaskPool> pool, bool debug,
                  Tasks&&... tasks) {
  using ReturnType = std::tuple<decltype(tasks.Future().get())...>;

  // Vector to store pointers to the futures
  std::vector<std::pair<std::future<void>, std::shared_ptr<std::atomic_bool>>>
      futures;

  // Lambda to execute and store the future of each task
  auto execute_task = [pool, &futures](auto& task) {
    auto result = pool->ExecuteTask(task);
    futures.push_back(std::move(result));
  };

  // Expand the parameter pack and execute each task
  (execute_task(tasks), ...);

  // Wait for all tasks to complete
  for (auto& future : futures) {
    if (future.first.valid())
      future.first.wait();
  }

  // Collect results from each task using ResultsCollector
  ResultsCollector results_collector(tasks.Future()...);
  ReturnType results = results_collector.collect();

  // Print results
  if (debug) {
    auto print = [](auto& task) {
      std::cout << task.Future().get() << std::endl;
    };
    (print(tasks), ...);
  }
  return std::move(results);
}
}  // namespace nvm::threads::task