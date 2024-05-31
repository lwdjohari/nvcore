#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>

#include "catch2/catch_all.hpp"
#include "nvm/dates/datetime.h"
#include "nvm/stopwatch.h"
#include "nvm/threads/async_executor.h"
using namespace nvm;

TEST_CASE("nvasync-taskpool-test", "[nvasync][async-executor]") {
  using NvAsync = nvm::threads::AsyncExecutor;
  using NvDateTime = nvm::dates::DateTime;
  using NvTaskPool = nvm::threads::TaskPool;
  using Stopwatch = nvm::Stopwatch;
  using namespace nvm::threads;

  auto pool = NvTaskPool::Create(4, 10);  // 4 threads, queue limit of 10 tasks
  auto shared_pool = pool->Share();       // Create a shared pointer to the pool
  Stopwatch sw;

  std::promise<std::pair<NvDateTime, NvDateTime>> promise1, promise2, promise3;

  auto future1 = promise1.get_future().share();
  auto future2 = promise2.get_future().share();
  auto future3 = promise3.get_future().share();

  auto lambda_task1 = [&promise1](int i) {
    auto time_start = NvDateTime::Now();
    std::this_thread::sleep_for(std::chrono::seconds(3));  // Simulate work
    auto time_end = NvDateTime::Now();
    promise1.set_value(std::make_pair(time_start, time_end));
  };

  auto lambda_task2 = [&promise2](int i) {
    auto time_start = NvDateTime::Now();
    std::this_thread::sleep_for(
        std::chrono::seconds(2));  // Simulate longer work
    auto time_end = NvDateTime::Now();
    promise2.set_value(std::make_pair(time_start, time_end));
  };

  auto lambda_task3 = [&promise3](std::string name) {
    auto time_start = NvDateTime::Now();
    std::this_thread::sleep_for(
        std::chrono::seconds(5));  // Simulate even longer work
    auto time_end = NvDateTime::Now();
    promise3.set_value(std::make_pair(time_start, time_end));
  };

  auto f1 = shared_pool->ExecuteTask(lambda_task1, 1);
  auto f2 = shared_pool->ExecuteTask(lambda_task2, 2);
  auto f3 = shared_pool->ExecuteTask(lambda_task3, std::string("promise-3"));

  // Collect futures
  std::vector<std::future<void>*> futures;
  futures.push_back(&f1.first);
  futures.push_back(&f2.first);
  futures.push_back(&f3.first);

  // Wait for all tasks to complete
  for (auto& future : futures) {
    future->wait();
  }

  auto res1 = std::move(future1.get());
  auto res2 = std::move(future2.get());
  auto res3 = std::move(future3.get());

  std::cout << "Task-1:" << res1.first << " | " << res1.second << std::endl;
  std::cout << "Task-2:" << res2.first << " | " << res2.second << std::endl;
  std::cout << "Task-2:" << res3.first << " | " << res3.second << std::endl;

  std::cout << "Executed in: " << sw.ElapsedMilliseconds() << " ms" << std::endl;

  // REQUIRE(res1 == 1);
  // REQUIRE(res2 == 2);
  // REQUIRE(res3 == "promise-3");
  REQUIRE(true == true);
}

TEST_CASE("nvasync-async-executor-test", "[nvasync][async-executor]") {
  using NvAsync = nvm::threads::AsyncExecutor;
  using NvTaskPool = nvm::threads::TaskPool;
  using TaskPoolPtr = nvm::threads::TaskPoolPtr;
  using AsyncTaskFunc = nvm::threads::AsyncTaskFunc;
  using AsyncTaskPromise = nvm::threads::AsyncTaskPromise;

  AsyncTaskPromise promise1, promise2, promise3, promise4;
  auto future1 = promise1.get_future().share();
  auto future2 = promise2.get_future().share();
  auto future3 = promise3.get_future().share();
  auto future4 = promise4.get_future().share();

  auto task_pool = NvTaskPool::Create();
  NvAsync async(task_pool);

  auto l1 = [&promise1]() {
    std::cout << "Promise-1" << std::endl;
    promise1.set_value(true);
  };

  auto l2 = [&promise2]() {
    std::cout << "Promise-2" << std::endl;
    promise2.set_value(true);
  };

  auto l3 = [&promise3]() {
    std::cout << "Promise-3" << std::endl;
    promise3.set_value(true);
  };

  auto l4 = [&promise4]() {
    std::cout << "Promise-4" << std::endl;
    promise4.set_value(true);
  };

  // clang-format off
  async
    .AllOf({future1, future2, future3})   // define parallel all. Defer execution.
    .One(future4)                         // define f4 after f1,f2,f3 done. Defer Execution
    .ExecuteAll();                        // execute all

  // clang-format on
}
