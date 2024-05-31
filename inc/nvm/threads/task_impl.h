#pragma once

#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>

#include "nvm/threads/def.h"

namespace nvm::threads::task {

template <typename T>
struct decay_custom {
  using type = typename std::decay<T>::type;
};

template <std::size_t N>
struct decay_custom<const char (&)[N]> {
  using type = std::string;
};

template <typename T>
using decay_custom_t = typename decay_custom<T>::type;

template <typename F, typename... Args>
class TaskWrapper {
 public:
  using ReturnType = RESULT_OF(F, Args...);

  TaskWrapper(F&& f, Args&&... args)
                  : func_(std::forward<F>(f)),
                    args_(std::make_tuple(
                        convert_arg(std::forward<Args>(args))...)),
                    promise_(std::make_shared<std::promise<ReturnType>>()),
                    future_(promise_->get_future().share()) {}

  void operator()() {
    try {
      std::apply(
          [this](auto&&... args) {
            IF_CONSTEXPR(std::is_void<ReturnType>::value) {
              func_(std::forward<decltype(args)>(args)...);
              promise_->set_value();
            }
            else {
              promise_->set_value(func_(std::forward<decltype(args)>(args)...));
            }
          },
          std::move(args_));
    } catch (const std::exception& e) {
      std::cerr << "Caught exception: " << e.what() << std::endl;
      promise_->set_exception(std::current_exception());
    } catch (...) {
      std::cerr << "Caught unknown exception" << std::endl;
      promise_->set_exception(std::current_exception());
    }
  }

  auto Future() -> std::shared_future<ReturnType> {
    return future_;
  }

 private:
  F func_;
  std::tuple<decay_custom_t<Args>...> args_;
  std::shared_ptr<std::promise<ReturnType>> promise_;
  std::shared_future<ReturnType> future_;

  template <typename T>
  static T&& convert_arg(T&& arg) {
    return std::forward<T>(arg);
  }

  static std::string convert_arg(const char* arg) {
    return std::string(arg);
  }

  //  static std::string convert_arg(const std::string arg) {
  //   return std::string(arg);
  // }
};

template <typename F, typename... Args>
auto MakeTask(F&& f, Args&&... args) {
  return TaskWrapper<F, Args...>(std::forward<F>(f),
                                 std::forward<Args>(args)...);
}

std::vector<std::future<void>*> PackTaskPtr(
    std::initializer_list<
        std::pair<std::future<void>, std::shared_ptr<std::atomic_bool>>*>&
        futures_list) {
  std::vector<std::future<void>*> futures;
  for (auto& future_pair : futures_list) {
    futures.push_back(&future_pair->first);
  }
  return futures;
}

std::vector<std::future<void>*> PackTaskPtr(
    std::vector<std::pair<std::future<void>,
                          std::shared_ptr<std::atomic_bool>>*>& futures_list) {
  std::vector<std::future<void>*> futures;
  for (auto& future_pair : futures_list) {
    futures.push_back(&future_pair->first);
  }
  return futures;
}



}  // namespace nvm::threads::task
