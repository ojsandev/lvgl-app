#include "infra/http/AsyncBoostBeastHttpClient.h"

#include <exception>
#include <stdexcept>
#include <utility>

#include "common/async/WorkerPool.h"
#include "infra/http/BoostBeastHttpClient.h"

using namespace infra::http;

AsyncBoostBeastHttpClient::AsyncBoostBeastHttpClient(const std::size_t workerCount)
  : m_workers(std::make_unique<common::async::WorkerPool>(workerCount))
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

  m_workers->submit([method, url = std::move(url), body = std::move(body), contentType = std::move(contentType),
                     completion = std::move(completion)]() mutable {
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
