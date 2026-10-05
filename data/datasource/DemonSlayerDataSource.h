#pragma once

#include <memory>
#include <string>

#include "common/types.h"
#include "http/IHttpClient.h"

namespace data::http {
class IHttpClient;
}

namespace data::datasource {
class DemonSlayerDataSource {
public:
  explicit DemonSlayerDataSource(const std::shared_ptr<http::IHttpClient>& httpClient);
  ~DemonSlayerDataSource() = default;

  [[nodiscard]] std::string getCharacters(common::types::Int32 limit, common::types::Int32 page) const;

private:
  std::shared_ptr<http::IHttpClient> m_httpClient;
};
} // namespace data::datasource
