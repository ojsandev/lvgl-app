#pragma once

#include <string>
#include <string_view>

#include "http/IHttpClient.h"

namespace infra::http {
class BoostBeastHttpClient final : public data::http::IHttpClient {
public:
  std::string get(std::string_view url) override;
  std::string post(std::string_view url, std::string_view body, std::string_view contentType) override;
  std::string put(std::string_view url, std::string_view body, std::string_view contentType) override;
  std::string patch(std::string_view url, std::string_view body, std::string_view contentType) override;
  std::string delete_(std::string_view url) override;

private:
  enum class Method { Get, Post, Put, Patch, Delete };

  std::string request(Method method, std::string_view url, std::string_view body = {},
                      std::string_view contentType = {});
};
} // namespace infra::http
