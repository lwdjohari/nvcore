#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "nvm/threads/def.h"
#include "nvm/threads/promisizer.h"

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

class TaskPool final : public std::enable_shared_from_this<TaskPool> {
 public:
  using Task =
      std::pair<std::function<void()>, std::shared_ptr<std::atomic<bool>>>;
  using TaskQueue = std::queue<Task>;

  explicit TaskPool(uint16_t thread_count = std::thread::hardware_concurrency(),
                    uint16_t task_queue_limit = 500)
                  : thread_count_(thread_count == 0
                                      ? std::thread::hardware_concurrency()
                                      : thread_count),
                    task_queue_limit_(task_queue_limit == 0 ? 500
                                                            : task_queue_limit),
                    stop_(false) {
    workers_ = PrepareThreads(thread_count_, task_queue_limit_);
  }

  // Delete copy constructor and copy assignment operator
  TaskPool(const TaskPool&) = delete;
  TaskPool& operator=(const TaskPool&) = delete;

  ~TaskPool() {
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      stop_ = true;
    }
    condition_.notify_all();
    for (std::thread& worker : workers_) {
      worker.join();
    }
  }

  static std::shared_ptr<TaskPool> Create(
      uint16_t thread_count = std::thread::hardware_concurrency(),
      uint16_t queue_limit = 500) {
    return std::make_shared<TaskPool>(thread_count, queue_limit);
  }

  std::shared_ptr<TaskPool> Share() {
    return shared_from_this();
  }

  template <class F, class... Args>
  auto ExecuteTask(F&& f, Args&&... args)
      -> std::pair<std::future<RESULT_OF(F, Args...)>,
                   std::shared_ptr<std::atomic<bool>>> {
    using return_type = RESULT_OF(F, Args...);
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> res = task->get_future();
    auto cancel_flag = std::make_shared<std::atomic<bool>>(false);

    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      task_available_.wait(lock, [this] {
        return this->tasks_.size() < this->task_queue_limit_ || this->stop_;
      });
      if (stop_) {
        throw std::runtime_error("Submit on stopped TaskPool");
      }
      tasks_.emplace(
          [task, cancel_flag]() {
            if (!cancel_flag->load()) {
              (*task)();
            }
          },
          cancel_flag);
    }
    condition_.notify_one();
    return std::make_pair(std::move(res), cancel_flag);
  }

 private:
  uint16_t thread_count_;
  uint16_t task_queue_limit_;
  std::vector<std::thread> workers_;
  TaskQueue tasks_;
  std::mutex queue_mutex_;
  std::condition_variable condition_;
  std::condition_variable task_available_;
  bool stop_;

  std::vector<std::thread> PrepareThreads(const uint16_t& thread_count,
                                          const uint16_t& task_queue_limit) {
    std::vector<std::thread> workers;
    for (size_t i = 0; i < thread_count; ++i) {
      workers.emplace_back([this] {
        while (true) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(this->queue_mutex_);
            this->condition_.wait(
                lock, [this] { return this->stop_ || !this->tasks_.empty(); });
            if (this->stop_ && this->tasks_.empty()) {
              return;
            }
            task = std::move(this->tasks_.front().first);
            this->tasks_.pop();
            this->task_available_.notify_one();
          }
          try {
            task();
          } catch (const std::exception& e) {
            std::cerr << "Task threw an exception: " << e.what() << std::endl;
          } catch (...) {
            std::cerr << "Task threw an unknown exception" << std::endl;
          }
        }
      });
    }
    return workers;
  }
};

inline void WaitAllTask(std::vector<std::future<void>*>& futures) {
  for (auto& future : futures) {
    future->wait();
  }
}

}  // namespace nvm::threads
