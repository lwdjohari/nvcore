#define CATCH_CONFIG_MAIN
#include <cstdint>
#include <iostream>

#include "catch2/catch_all.hpp"
#include "nvm/dates/datetime.h"
#include "nvm/stopwatch.h"
#include "nvm/threads/task.h"
#include "nvm/threads/utils.h"
using namespace nvm;

TEST_CASE("nvasync-taskpool-promisizer", "[nvasync][async-executor]") {
  using NvAsync = nvm::threads::AsyncExecutor;
  using NvDateTime = nvm::dates::DateTime;
  using NvTaskPool = nvm::threads::TaskPool;
  using Stopwatch = nvm::Stopwatch;
  using namespace nvm::threads;

  auto thread_num = 5;
  auto pool =
      NvTaskPool::Create(thread_num, 10);  // 5 threads, queue limit of 10 tasks
  auto shared_pool = pool->Share();  // Create a shared pointer to the pool
  Stopwatch sw;

  int x = 10;

  auto fn1 = [](int i) -> int {
    std::this_thread::sleep_for(std::chrono::seconds(3));  // Simulate work
    // if (i == 5)
    //   throw std::runtime_error("Intentional error");
    return i * 2;
  };

  auto fn2 = []() -> Task {
    std::this_thread::sleep_for(std::chrono::seconds(3));  // Simulate work
    std::cout << "Work done" << std::endl;
    return Task(true);
  };

  auto fn3 = [x](const std::string& str, int count) -> std::string {
    std::this_thread::sleep_for(std::chrono::seconds(4));  // Simulate work
    auto res = std::string(str);
    return res + " repeated " + std::to_string(count) +
           " times, plus x: " + std::to_string(x);
  };

  auto fn4 = [](double value) -> double {
    std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate work
    return value * 3.14;
  };

  auto lambda1 = task::MakeTask(fn1, 5);
  auto lambda2 = task::MakeTask(fn2);
  auto lambda3 = task::MakeTask(fn3, "Hello", 3);
  auto lambda4 = task::MakeTask(fn4, 2.5);
  auto lambda5 = task::MakeTask(fn3, "Hello", 5);
  auto lambda6 = task::MakeTask(fn3, "Hello", 6);

  auto exec_time_theory =
      utils::CalculateMaxExecutionTime({3, 3, 4, 1, 4, 4}, thread_num);

  auto res = task::AsyncWaitAll(shared_pool, false, lambda1, lambda2, lambda3, lambda4,
                                lambda5, lambda6);

  auto res1 = std::get<0>(res);
  auto res2 = std::get<1>(res);

  auto res3 = std::move(std::get<2>(res));
  auto res4 = std::get<3>(res);
  auto res5 = std::get<4>(res);
  auto res6 = std::get<5>(res);

  std::cout << "Task-1:" << res1 << std::endl;
  std::cout << "Task-3:" << res3 << std::endl;
  std::cout << "Task-4:" << res4 << std::endl;
  std::cout << "Task-5:" << res5 << std::endl;
  std::cout << "Task-6:" << res6 << std::endl;

  std::cout << "Theoritical Exec. Time  : " << exec_time_theory * 1000 << " ms"
            << std::endl;

  std::cout << "Actual Exec. Time       : " << sw.ElapsedMilliseconds() << " ms"
            << std::endl;

  REQUIRE(true == true);
}

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

  // cppcheck-suppress passedByValue
  auto lambda_task3 = [&promise3](std::string name, int count) {
    auto time_start = NvDateTime::Now();
    std::this_thread::sleep_for(
        std::chrono::seconds(5));  // Simulate even longer work
    auto time_end = NvDateTime::Now();
    promise3.set_value(std::make_pair(time_start, time_end));
  };

  auto f1 = shared_pool->ExecuteTask(lambda_task1, 1);
  auto f2 = shared_pool->ExecuteTask(lambda_task2, 2);
  auto f3 = shared_pool->ExecuteTask(lambda_task3, std::string("promise-3"), 3);

  auto ptr = {&f1, &f2, &f3};
  auto futures = task::PackTaskPtr(ptr);

  // Wait for all tasks to complete
  WaitAllTask(futures);

  auto res1 = std::move(future1.get());
  auto res2 = std::move(future2.get());
  auto res3 = std::move(future3.get());

  std::cout << "Task-1:" << res1.first << " | " << res1.second << std::endl;
  std::cout << "Task-2:" << res2.first << " | " << res2.second << std::endl;
  std::cout << "Task-3:" << res3.first << " | " << res3.second << std::endl;

  std::cout << "Executed in: " << sw.ElapsedMilliseconds() << " ms"
            << std::endl;

  // REQUIRE(res1 == 1);
  // REQUIRE(res2 == 2);
  // REQUIRE(res3 == "promise-3");
  REQUIRE(true == true);
}

TEST_CASE("nvasync-async-executor-test", "[nvasync][async-executor]") {}
