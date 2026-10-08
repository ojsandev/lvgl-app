#include "DemonSlayerDataSource.h"

#include "common/logger/Logging.h"

using namespace data::datasource;

namespace {
constexpr std::string_view BASE_URL = "https://www.demonslayer-api.com/api/v1/characters";
constexpr std::string_view IMAGE_BASE_URL = "https://www.demonslayer-api.com/api/v1/characters/images";
} // namespace

DemonSlayerDataSource::DemonSlayerDataSource(const std::shared_ptr<http::IHttpClient>& httpClient)
  : m_httpClient(httpClient)
{}

std::string DemonSlayerDataSource::getCharacters(const common::types::Int32 limit,
                                                 const common::types::Int32 page) const
{
  LOG_INFO("Fetching characters from Demon Slayer API with limit: {}, page: {}", limit, page);
  const auto url = std::string(BASE_URL) + "?page=" + std::to_string(page) + "&limit=" + std::to_string(limit);
  return m_httpClient->get(url);
}

std::vector<common::types::UInt8> DemonSlayerDataSource::getCharacterImage(common::types::Int32 characterId) const
{
  LOG_INFO("Fetching character image from Demon Slayer API for character ID: {}", characterId);
  const auto url = std::string(IMAGE_BASE_URL) + "/" + std::to_string(characterId) + ".webp";
  return m_httpClient->getBytes(url);
}
