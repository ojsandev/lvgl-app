#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "common/async/WorkerPool.h"

namespace {

using namespace std::chrono_literals;

bool executesSubmittedTasks()
{
  common::async::WorkerPool pool(2);
  std::mutex mutex;
  std::condition_variable condition;
  int completed = 0;

  for (int i = 0; i < 3; ++i) {
    pool.submit([&] {
      std::lock_guard lock(mutex);
      ++completed;
      condition.notify_all();
    });
  }

  std::unique_lock lock(mutex);
  return condition.wait_for(lock, 1s, [&] { return completed == 3; });
}

bool continuesAfterTaskThrows()
{
  common::async::WorkerPool pool(1);
  std::mutex mutex;
  std::condition_variable condition;
  bool completed = false;

  pool.submit([] { throw std::runtime_error("boom"); });
  pool.submit([&] {
    std::lock_guard lock(mutex);
    completed = true;
    condition.notify_all();
  });

  std::unique_lock lock(mutex);
  return condition.wait_for(lock, 1s, [&] { return completed; });
}

bool destructorDrainsQueuedTasks()
{
  std::mutex mutex;
  std::condition_variable condition;
  bool started = false;
  bool released = false;
  bool secondTaskRan = false;

  std::thread releaser;
  {
    common::async::WorkerPool pool(1);
    pool.submit([&] {
      std::unique_lock lock(mutex);
      started = true;
      condition.notify_all();
      condition.wait(lock, [&] { return released; });
    });
    pool.submit([&] {
      std::lock_guard lock(mutex);
      secondTaskRan = true;
    });

    std::unique_lock lock(mutex);
    if (!condition.wait_for(lock, 1s, [&] { return started; })) {
      return false;
    }

    releaser = std::thread([&] {
      std::this_thread::sleep_for(20ms);
      std::lock_guard lock(mutex);
      released = true;
      condition.notify_all();
    });
  }

  if (releaser.joinable()) {
    releaser.join();
  }

  std::lock_guard stateLock(mutex);
  return secondTaskRan;
}

} // namespace

int main()
{
  if (!executesSubmittedTasks()) {
    return 1;
  }

  if (!continuesAfterTaskThrows()) {
    return 1;
  }

  return destructorDrainsQueuedTasks() ? 0 : 1;
}

