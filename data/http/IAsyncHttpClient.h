#pragma once

#include <functional>
#include <string_view>

#include "data/http/AsyncHttpResult.h"

namespace data::http {

class IAsyncHttpClient {
public:
  using StringCompletion = std::function<void(AsyncHttpStringResult)>;
  using BytesCompletion = std::function<void(AsyncHttpBytesResult)>;

  virtual ~IAsyncHttpClient() = default;

  // Completion callbacks run on a worker thread, never inline on the caller's
  // stack. Callers must marshal UI work back to the UI thread.
  virtual void getAsync(std::string_view url, StringCompletion completion) = 0;
  virtual void postAsync(std::string_view url, std::string_view body, StringCompletion completion,
                         std::string_view contentType = "application/json") = 0;
  virtual void putAsync(std::string_view url, std::string_view body, StringCompletion completion,
                        std::string_view contentType = "application/json") = 0;
  virtual void patchAsync(std::string_view url, std::string_view body, StringCompletion completion,
                          std::string_view contentType = "application/json") = 0;
  virtual void deleteAsync(std::string_view url, StringCompletion completion) = 0;
  virtual void getBytesAsync(std::string_view url, BytesCompletion completion) = 0;
}

} // namespace data::http
