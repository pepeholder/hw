#pragma once

#include <atomic>

namespace stdlike {

class CondVar {
 public:
  CondVar() : counter(0) {}

  template <class Mutex>
  void Wait(Mutex& mutex) {
    int old_val = counter.load(std::memory_order_relaxed);
    mutex.unlock();
    counter.wait(old_val);
    mutex.lock();
  }

  void NotifyOne() {
    counter.fetch_add(1, std::memory_order_relaxed);
    counter.notify_one();
  }

  void NotifyAll() {
    counter.fetch_add(1, std::memory_order_relaxed);
    counter.notify_all();
  }

 private:
  std::atomic<int> counter;
};
}  // namespace stdlike
