#pragma once

#include <memory>
#include <string>

#include "common/types.h"
#include "http/IAsyncHttpClient.h"
#include "http/IHttpClient.h"

namespace data::http {
class IHttpClient;
class IAsyncHttpClient;
}

namespace data::datasource {
class DemonSlayerDataSource {
public:
  using CharactersCallback = std::function<void(http::AsyncHttpStringResult)>;
  using CharactersImageCallback = std::function<void(http::AsyncHttpBytesResult)>;

  explicit DemonSlayerDataSource(const std::shared_ptr<http::IHttpClient>& httpClient, const std::shared_ptr<http::IAsyncHttpClient>& asyncHttpClient);
  ~DemonSlayerDataSource() = default;

  void getCharacters(common::types::Int32 limit, common::types::Int32 page, const CharactersCallback& callback);

  void getCharacterImage(common::types::Int32 characterId, const CharactersImageCallback& callback) const;

private:
  std::shared_ptr<http::IHttpClient> m_httpClient;
  std::shared_ptr<http::IAsyncHttpClient> m_asyncHttpClient;
};
} // namespace data::datasource
