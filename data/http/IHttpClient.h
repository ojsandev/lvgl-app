#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "common/types.h"

namespace data::http {
class IHttpClient {
public:
  virtual ~IHttpClient() = default;

  virtual std::string get(std::string_view url) = 0;
  virtual std::string post(std::string_view url, std::string_view body,
                           std::string_view contentType = "application/json") = 0;
  virtual std::string put(std::string_view url, std::string_view body,
                          std::string_view contentType = "application/json") = 0;
  virtual std::string patch(std::string_view url, std::string_view body,
                            std::string_view contentType = "application/json") = 0;
  virtual std::string delete_(std::string_view url) = 0;

  virtual std::vector<common::types::UInt8> getBytes(std::string_view url) = 0;
};
} // namespace data::http
