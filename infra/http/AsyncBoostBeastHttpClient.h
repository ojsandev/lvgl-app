#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>

#include "data/http/IAsyncHttpClient.h"

namespace infra::http {
class WorkerPool;

class AsyncBoostBeastHttpClient final : public data::http::IAsyncHttpClient {
public:
  explicit AsyncBoostBeastHttpClient(std::size_t workerCount = 2);
  ~AsyncBoostBeastHttpClient() override;

  AsyncBoostBeastHttpClient(const AsyncBoostBeastHttpClient&) = delete;
  AsyncBoostBeastHttpClient& operator=(const AsyncBoostBeastHttpClient&) = delete;
  AsyncBoostBeastHttpClient(AsyncBoostBeastHttpClient&&) = delete;
  AsyncBoostBeastHttpClient& operator=(AsyncBoostBeastHttpClient&&) = delete;

  void getAsync(std::string_view url, StringCompletion completion) override;
  void postAsync(std::string_view url, std::string_view body, StringCompletion completion,
                 std::string_view contentType = "application/json") override;
  void putAsync(std::string_view url, std::string_view body, StringCompletion completion,
                std::string_view contentType = "application/json") override;
  void patchAsync(std::string_view url, std::string_view body, StringCompletion completion,
                  std::string_view contentType = "application/json") override;
  void deleteAsync(std::string_view url, StringCompletion completion) override;
  void getBytesAsync(std::string_view url, BytesCompletion completion) override;

private:
  enum class Method { Get, Post, Put, Patch, Delete };

  std::unique_ptr<WorkerPool> m_workers;

  void requestString(Method method, std::string url, std::string body, std::string contentType,
                     StringCompletion completion);
};

} // namespace infra::http
