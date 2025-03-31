#pragma once

#include <atomic>

namespace stdlike {

class Mutex {
 public:
  Mutex() : state(0) {}

  void Lock() {
    uint32_t expected = 0;
    if (state.compare_exchange_strong(expected, 1, std::memory_order_acquire)) {
      return;
    }
    while (true) {
      expected = state.load(std::memory_order_relaxed);
      if (expected == 0) {
        if (state.compare_exchange_strong(expected, 2, std::memory_order_acquire)) {
          return;
        }
        continue;
      }
      if (expected == 1) {
        if (state.compare_exchange_strong(expected, 2, std::memory_order_acquire)) {
        } else {
          continue;
        }
      }
      state.wait(2, std::memory_order_relaxed);
    }
  }

  void Unlock() {
    uint32_t prev = state.fetch_sub(1, std::memory_order_release);
    if (prev != 1) {
      state.store(0, std::memory_order_release);
      state.notify_one();
    }
  }

 private:
  std::atomic<uint32_t> state;
};
}  // namespace stdlike