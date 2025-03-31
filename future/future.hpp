#pragma once

#include <memory>
#include <mutex>
#include <condition_variable>
#include <variant>
#include <stdexcept>
#include <exception>
#include <utility>

namespace stdlike {

namespace detail {

template <typename T>
class SharedState {
 public:
  SharedState() : ready(false), result(std::monostate{}) {}

  void SetValue(T val) {
    std::lock_guard<std::mutex> lock(mutex);
    if (ready) {
      throw std::runtime_error("Promise already satisfied");
    }
    result = std::move(val);
    ready = true;
    cv.notify_all();
  }

  void SetException(std::exception_ptr ex) {
    std::lock_guard<std::mutex> lock(mutex);
    if (ready) {
      throw std::runtime_error("Promise already satisfied");
    }
    result = ex;
    ready = true;
    cv.notify_all();
  }

  T Get() {
    std::unique_lock<std::mutex> lock(mutex);
    while (!ready) {
      cv.wait(lock);
    }
    if (std::holds_alternative<std::exception_ptr>(result)) {
      std::rethrow_exception(std::get<std::exception_ptr>(result));
    }
    return std::move(std::get<T>(result));
  }

 private:
  std::mutex mutex;
  std::condition_variable cv;
  bool ready;
  std::variant<std::monostate, T, std::exception_ptr> result;
};

}  // namespace detail

template <typename T>
class Future {
  template <typename U>
  friend class Promise;

 public:
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  Future(Future&&) = default;
  Future& operator=(Future&&) = default;

  T Get() {
    if (!state) {
      throw std::runtime_error("Invalid future state");
    }
    return state->Get();
  }

 private:
  explicit Future(std::shared_ptr<detail::SharedState<T>> s)
      : state(s) {}

 private:
  std::shared_ptr<detail::SharedState<T>> state;
};

}  // namespace stdlike