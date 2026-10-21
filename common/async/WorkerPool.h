#pragma once
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "common/types.h"

namespace common::async {

class WorkerPool {
public:
  explicit WorkerPool(std::size_t workerCount);
  ~WorkerPool();

  void submit(std::function<void()> task);
  types::Int32 getRunningWorkerCount();
  types::Int32 getRunningTaskCount();

private:
  void run();

private:
  std::mutex m_mutex;
  std::condition_variable m_condition;
  std::deque<std::function<void()>> m_tasks;
  std::vector<std::thread> m_workers;
  bool m_stopping = false;
};
} // namespace common::async

