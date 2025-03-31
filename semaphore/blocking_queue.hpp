#pragma once

#include "semaphore.hpp"
#include <deque>
#include <utility>

namespace solutions {
template <typename T>
class BlockingQueue {
 public:
  explicit BlockingQueue(size_t capacity)
      : capacity(capacity),
        sem_slots(capacity),
        sem_items(0),
        sem_mutex(1) {
  }

  void Put(T value) {
    sem_slots.Acquire();
    sem_mutex.Acquire();
    queue.push_back(std::move(value));
    sem_mutex.Release();
    sem_items.Release();
  }

  T Take() {
    sem_items.Acquire();
    sem_mutex.Acquire();
    T value = std::move(queue.front());
    queue.pop_front();
    sem_mutex.Release();
    sem_slots.Release();
    return value;
  }

 private:
  const size_t capacity;
  std::deque<T> queue;
  Semaphore sem_slots;
  Semaphore sem_items;
  Semaphore sem_mutex;
};

}  // namespace solutions
