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

enum class ExecutorWaitingMode { None, AllOf, OneOf };

NVM_ENUM_CLASS_DISPLAY_TRAIT(ExecutorWaitingMode)

class TaskPool;
class AsyncExecutor;
class AsyncTask;

using TaskPoolPtr = std::shared_ptr<TaskPool>;
using AsyncTaskFunc = std::shared_future<bool>;
using AsyncTaskPromise = std::promise<bool>;

using AsyncTaskList = std::vector<AsyncTaskFunc>;
using CircuitBreaker = std::function<bool(const AsyncTask&)>;

}  // namespace nvm::threads
