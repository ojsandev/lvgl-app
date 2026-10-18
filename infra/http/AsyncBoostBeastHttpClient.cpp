#include "infra/http/AsyncBoostBeastHttpClient.h"

#include <algorithm>
#include <condition_variable>
#include <deque>
#include <exception>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include "common/logger/Logging.h"
#include "infra/http/BoostBeastHttpClient.h"

using namespace infra::http;

class AsyncBoostBeastHttpClient::WorkerPool {
public:
  explicit WorkerPool(std::size_t workerCount)
  {
    workerCount = std::max<std::size_t>(1, workerCount);
    m_workers.reserve(workerCount);
    for (std::size_t i = 0; i < workerCount; ++i) {
      m_workers.emplace_back([this] { run(); });
    }
  }

  ~WorkerPool()
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

  void submit(std::function<void()> task)
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

private:
  void run()
  {
    while (true) {
      std::function<void()> task;
      {
        std::unique_lock lock(m_mutex);
        m_condition.wait(lock, [this] { return m_stopping || !m_tasks.empty(); });
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

  std::mutex m_mutex;
  std::condition_variable m_condition;
  std::deque<std::function<void()>> m_tasks;
  std::vector<std::thread> m_workers;
  bool m_stopping = false;
};

AsyncBoostBeastHttpClient::AsyncBoostBeastHttpClient(const std::size_t workerCount)
  : m_workers(std::make_unique<WorkerPool>(workerCount))
{}

AsyncBoostBeastHttpClient::~AsyncBoostBeastHttpClient() = default;

void AsyncBoostBeastHttpClient::getAsync(const std::string_view url, StringCompletion completion)
{
  requestString(Method::Get, std::string(url), {}, {}, std::move(completion));
}

void AsyncBoostBeastHttpClient::postAsync(const std::string_view url, const std::string_view body,
                                          StringCompletion completion, const std::string_view contentType)
{
  requestString(Method::Post, std::string(url), std::string(body), std::string(contentType), std::move(completion));
}

void AsyncBoostBeastHttpClient::putAsync(const std::string_view url, const std::string_view body,
                                         StringCompletion completion, const std::string_view contentType)
{
  requestString(Method::Put, std::string(url), std::string(body), std::string(contentType), std::move(completion));
}

void AsyncBoostBeastHttpClient::patchAsync(const std::string_view url, const std::string_view body,
                                           StringCompletion completion, const std::string_view contentType)
{
  requestString(Method::Patch, std::string(url), std::string(body), std::string(contentType), std::move(completion));
}

void AsyncBoostBeastHttpClient::deleteAsync(const std::string_view url, StringCompletion completion)
{
  requestString(Method::Delete, std::string(url), {}, {}, std::move(completion));
}

void AsyncBoostBeastHttpClient::getBytesAsync(const std::string_view url, BytesCompletion completion)
{
  if (!completion) {
    throw std::invalid_argument("getBytesAsync requires a completion callback");
  }

  auto copiedUrl = std::string(url);
  m_workers->submit([copiedUrl = std::move(copiedUrl), completion = std::move(completion)]() mutable {
    try {
      BoostBeastHttpClient client;
      completion(data::http::AsyncHttpBytesResult::success(client.getBytes(copiedUrl)));
    } catch (...) {
      completion(data::http::AsyncHttpBytesResult::failure(std::current_exception()));
    }
  });
}

void AsyncBoostBeastHttpClient::requestString(const Method method, std::string url, std::string body,
                                              std::string contentType, StringCompletion completion)
{
  if (!completion) {
    throw std::invalid_argument("Async HTTP request requires a completion callback");
  }

  m_workers->submit([method, url = std::move(url), body = std::move(body),
                     contentType = std::move(contentType), completion = std::move(completion)]() mutable {
    try {
      BoostBeastHttpClient client;
      std::string response;

      switch (method) {
      case Method::Get:
        response = client.get(url);
        break;
      case Method::Post:
        response = client.post(url, body, contentType);
        break;
      case Method::Put:
        response = client.put(url, body, contentType);
        break;
      case Method::Patch:
        response = client.patch(url, body, contentType);
        break;
      case Method::Delete:
        response = client.delete_(url);
        break;
      }

      completion(data::http::AsyncHttpStringResult::success(std::move(response)));
    } catch (...) {
      completion(data::http::AsyncHttpStringResult::failure(std::current_exception()));
    }
  });
}
