#include "DemonSlayerDataSource.h"

using namespace data::datasource;

namespace {
constexpr std::string_view BASE_URL = "https://www.demonslayer-api.com/api/v1/characters";
}

DemonSlayerDataSource::DemonSlayerDataSource(const std::shared_ptr<http::IHttpClient>& httpClient)
  : m_httpClient(httpClient) {}

std::string DemonSlayerDataSource::getCharacters(const common::types::Int32 limit,
                                                 const common::types::Int32 page) const
{
  const auto url = std::string(BASE_URL) + "?page=" + std::to_string(page) + "&limit=" + std::to_string(limit);
  return m_httpClient->get(url);
}
