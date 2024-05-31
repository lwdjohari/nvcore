#pragma once

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "nvm/macro.h"

namespace nvm::threads {

// Define a macro to conditionally use std::result_of for C++14 and
// std::invoke_result for C++17 and later
#if __cplusplus < 201703L
#define RESULT_OF(F, ...) \
  typename std::result_of<F(__VA_ARGS__)>::type  // C++14
#else
#define RESULT_OF(F, ...) \
  typename std::invoke_result<F, __VA_ARGS__>::type  // C++17
#endif

// Define a macro to handle if constexpr for C++14
#if __cplusplus < 201703L
#define IF_CONSTEXPR if
#else
#define IF_CONSTEXPR if constexpr
#endif

enum class ExecutorWaitingMode { None, AllOf, OneOf };

NVM_ENUM_CLASS_DISPLAY_TRAIT(ExecutorWaitingMode)

class TaskPool;
class AsyncExecutor;
class AsyncTask;

struct Task {
  bool state;

  Task() : state() {}
  explicit Task(bool state) : state(state) {}

  friend std::ostream& operator<<(std::ostream& os, const Task& task) {
    os << "Task<void>(state=" << (task.state ? "true" : "false") << ")";
    return os;
  }
};

using TaskPoolPtr = std::shared_ptr<TaskPool>;
using AsyncTaskFunc = std::shared_future<bool>;
using AsyncTaskPromise = std::promise<bool>;

using AsyncTaskList = std::vector<AsyncTaskFunc>;
using CircuitBreaker = std::function<bool(const AsyncTask&)>;

}  // namespace nvm::threads
