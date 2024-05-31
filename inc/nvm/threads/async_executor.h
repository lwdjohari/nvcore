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

#pragma once

#include "nvm/macro.h"
#include "nvm/threads/def.h"
#include "nvm/threads/task_pool.h"

namespace nvm::threads {

class AsyncExecutorParameter {
 public:
  explicit AsyncExecutorParameter(AsyncTaskList& futures,
                                  ExecutorWaitingMode waiting_mode)
                  : futures_(futures), waiting_mode_(waiting_mode) {}

  explicit AsyncExecutorParameter(AsyncTaskList&& futures,
                                  ExecutorWaitingMode waiting_mode)
                  : futures_(std::forward<AsyncTaskList>(futures)),
                    waiting_mode_(waiting_mode) {}

  ExecutorWaitingMode Mode() const {
    return waiting_mode_;
  }

  void ExecuteAsync() {
    switch (waiting_mode_) {
      case ExecutorWaitingMode::AllOf:
        WaitAll(futures_);
        break;

      case ExecutorWaitingMode::OneOf:
        WaitAll(futures_);
        break;

      default:
        break;
    }
  }

 private:
  AsyncTaskList futures_;
  ExecutorWaitingMode waiting_mode_;

  void WaitAll(AsyncTaskList& tasks) {
    std::vector<std::future<void>> async_tasks;
    for (auto& task : tasks) {
      // cppcheck-suppress useStlAlgorithm
      async_tasks.push_back(
          std::async(std::launch::async, [task]() { task.wait(); }));
    }

    for (auto& task : async_tasks) {
      task.get();
    }
  }
};

/// @brief Fluent Async Task, easy to execute multistep multithreading handling
class AsyncExecutor {
 public:
  using ParameterList = std::vector<AsyncExecutorParameter>;

  /// @brief Construct Async Executor using internal threadpool.
  AsyncExecutor() : futures_(), task_pool_ptr_(std::make_shared<TaskPool>()) {}

  /// @brief Construct Async Executor using shared threadpool.
  /// @param thread_pool
  explicit AsyncExecutor(TaskPoolPtr task_pool)
                  : futures_(), task_pool_ptr_(task_pool) {}

  /// @brief Execute async tasks at once and multi-threading executions.
  /// @param futures
  /// @param circuit_breaker
  /// @return
  AsyncExecutor& AllOf(std::initializer_list<AsyncTaskFunc> futures,
                       CircuitBreaker circuit_breaker = nullptr) {
    AsyncTaskList futures_vec(futures);
    futures_.emplace_back(std::move(futures_vec), ExecutorWaitingMode::AllOf);
    return *this;
  }

  /// @brief Execute one task as async
  /// @param future
  /// @param circuit_breaker
  /// @return
  AsyncExecutor& One(AsyncTaskFunc future,
                     CircuitBreaker circuit_breaker = nullptr) {
    AsyncTaskList futures_vec;
    futures_vec.push_back(future);
    futures_.emplace_back(std::move(futures_vec), ExecutorWaitingMode::AllOf);
    return *this;
  }

  /// @brief Will only wait one of futures finished the task
  /// @param futures
  /// @param circuit_breaker
  /// @return
  AsyncExecutor& OneOf(std::initializer_list<AsyncTaskFunc> futures,
                       CircuitBreaker circuit_breaker = nullptr) {
    AsyncTaskList futures_vec(futures);
    futures_.emplace_back(std::move(futures_vec), ExecutorWaitingMode::OneOf);
    return *this;
  }

  bool ExecuteAsync(AsyncTaskFunc future,
                    CircuitBreaker circuit_breaker = nullptr) {
    return false;
  }

  /// @brief
  void ExecuteAll() {
    if (futures_.empty()) {
      return;
    }

    for (auto& future : futures_) {
      future.ExecuteAsync();
    }
  }

  TaskPoolPtr GetTaskPool(){
    return task_pool_ptr_;
  }

 private:
  ParameterList futures_;
  TaskPoolPtr task_pool_ptr_;
};

}  // namespace nvm::threads
