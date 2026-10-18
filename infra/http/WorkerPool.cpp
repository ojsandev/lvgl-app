#include "WorkerPool.h"

#include "common/logger/Logging.h"

using namespace infra::http;

WorkerPool::WorkerPool(const std::size_t workerCount)
{
  const std::size_t actualWorkerCount = std::max<std::size_t>(1, workerCount);
  m_workers.reserve(actualWorkerCount);
  for (std::size_t i = 0; i < actualWorkerCount; ++i) {
    m_workers.emplace_back([this] {
      run();
    });
  }
}

WorkerPool::~WorkerPool()
{
  {
    std::lock_guard lock(m_mutex);
    m_stopping = true;
  }
  m_condition.notify_all();
  for (auto& worker : m_workers) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

void WorkerPool::run()
{
  while (true) {
    std::function<void()> task;
    {
      std::unique_lock lock(m_mutex);
      m_condition.wait(lock, [this] {
        return m_stopping || !m_tasks.empty();
      });
      if (m_stopping && m_tasks.empty()) {
        return;
      }
      task = std::move(m_tasks.front());
      m_tasks.pop_front();
    }

    try {
      task();
    } catch (const std::exception& error) {
      LOG_ERROR("Unhandled exception in async HTTP worker: {}", error.what());
    } catch (...) {
      LOG_ERROR("Unhandled non-standard exception in async HTTP worker");
    }
  }
}

void WorkerPool::submit(std::function<void()> task)
{
  {
    std::lock_guard lock(m_mutex);
    if (m_stopping) {
      throw std::runtime_error("Async HTTP client is shutting down");
    }
    m_tasks.emplace_back(std::move(task));
  }
  m_condition.notify_one();
}
