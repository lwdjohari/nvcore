#pragma once
#include <atomic>
#include <utility>
#include <vector>

#include "nvm/threads/task_impl.h"
#include "nvm/threads/task_pool.h"

namespace nvm::threads::task {

// Helper function to convert arguments
template <typename T>
auto convert_arg(T&& arg) -> decltype(std::forward<T>(arg)) {
  return std::forward<T>(arg);
}

auto convert_arg(const char* arg) -> std::string {
  return std::string(arg);
}

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

  explicit ResultsCollector(Futures&&... futures)
                  : futures(std::forward<Futures>(futures)...) {}

  template <std::size_t... I>
  auto collect_results(std::index_sequence<I...>) {
    return std::make_tuple(
        std::move(convert_arg(get_future_result(std::get<I>(futures))))...);
  }

  auto collect() {
    return collect_results(std::index_sequence_for<Futures...>{});
  }
};

template <typename... Tasks>
bool AsyncWaitAll(std::shared_ptr<class TaskPool> pool, bool debug,
                  Tasks&&... tasks) {
  //   using ReturnType = std::tuple<decltype(tasks.Result().get())...>;
  //   using ReturnType =
  //       std::tuple<decltype(std::declval<Tasks>().Result().get())...>;
  // Vector to store pointers to the futures
  AsyncTaskHandleList futures;

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

  //   //Collect results from each task using ResultsCollector
  //   ResultsCollector results_collector(tasks.Result()...);
  //   ReturnType res = results_collector.collect();

  // Print results
  //   ReturnType res = std::make_tuple( tasks.Result().get()...);
  //   ReturnType res =
  //   std::make_tuple(convert_arg(tasks.Result().get())...);

  if (debug) {
    auto print = [](auto& task) {
      std::cout << task.Result().get() << std::endl;
    };
    (print(tasks), ...);
  }

  return true;
}

template <typename... Tasks>
auto AsyncNoWaitAll(std::shared_ptr<class TaskPool> pool, bool debug,
                    Tasks&&... tasks) {
  AsyncTaskHandleList futures;

  // Lambda to execute and store the future of each task
  auto execute_task = [pool, &futures](auto& task) {
    auto result = pool->ExecuteTask(task);
    futures.push_back(std::move(result));
  };

  // Expand the parameter pack and execute each task
  (execute_task(tasks), ...);

  if (debug) {
    auto print = [](auto& task) {
      std::cout << task.Result().get() << std::endl;
    };
    (print(tasks), ...);
  }

  return std::move(futures);
}
}  // namespace nvm::threads::task