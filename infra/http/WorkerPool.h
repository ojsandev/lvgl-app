#pragma once
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace infra::http {

class WorkerPool {
public:
  explicit WorkerPool(std::size_t workerCount);
  ~WorkerPool();

  void submit(std::function<void()> task);

private:
  void run();

private:
  std::mutex m_mutex;
  std::condition_variable m_condition;
  std::deque<std::function<void()>> m_tasks;
  std::vector<std::thread> m_workers;
  bool m_stopping = false;
};
} // namespace infra::http
