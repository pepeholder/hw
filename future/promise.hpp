#pragma once

#include "future.hpp"
#include <memory>
#include <stdexcept>

namespace stdlike {

template <typename T>
class Promise {
 public:
  Promise() : state(std::make_shared<detail::SharedState<T>>()) {}
  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;
  Promise(Promise&&) = default;
  Promise& operator=(Promise&&) = default;
  Future<T> MakeFuture() {
    return Future<T>(state);
  }

  void SetValue(T value) {
    if (!state) {
      throw std::runtime_error("Invalid promise state");
    }
    state->SetValue(std::move(value));
  }

  void SetException(std::exception_ptr ex) {
    if (!state) {
      throw std::runtime_error("Invalid promise state");
    }
    state->SetException(ex);
  }

 private:
  std::shared_ptr<detail::SharedState<T>> state;
};

}  // namespace stdlike