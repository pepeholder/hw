#pragma once

#include <mutex>
#include <condition_variable>
#include <cstddef>

namespace solutions {
class Semaphore {
 public:
  explicit Semaphore(size_t initial) : count(initial) {}
  void Acquire() {
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [this]() { return count > 0; });
    --count;
  }

  void Release() {
    std::unique_lock<std::mutex> lock(m);
    ++count;
    cv.notify_one();
  }

 private:
  std::mutex m;
  std::condition_variable cv;
  size_t count;
};

}  // namespace solutions