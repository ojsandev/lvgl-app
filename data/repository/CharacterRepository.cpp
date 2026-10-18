#include "CharacterRepository.h"

#include <print>

#include "common/logger/Logging.h"
#include "data/datasource/DemonSlayerDataSource.h"
#include "data/mapper/CharacterMapper.h"
#include "domain/model/CharactersResponse.h"
#include "domain/model/error/Error.h"
#include "data/http/HttpException.h"
#include "mapper/ErrorMapper.h"
#include <future>
#include <memory>
#include <exception>

using namespace data::repository;

CharacterRepository::CharacterRepository(
    const std::shared_ptr<datasource::DemonSlayerDataSource>& demonSlayerDataSource)
  : m_demonSlayerDataSource(demonSlayerDataSource) {}

domain::repository::ICharacterRepository::GetCharactersResult
CharacterRepository::getCharactersPaginated(
    const common::types::Int32 limit,
    const common::types::Int32 page)
{
  auto promise = std::make_shared<std::promise<std::variant<
    domain::model::CharactersResponse, domain::model::error::Error>>>();
  auto future = promise->get_future();

  try {
    m_demonSlayerDataSource->getCharacters(limit, page, [promise](const http::AsyncHttpStringResult& result) mutable {
      try {
        if (!result.hasValue()) {
          result.rethrowError();
        }

        const auto& json = result.value();

        LOG_INFO("Response received: {} bytes", json.size());

        promise->set_value(mapper::CharacterMapper::fromJson(json));
      } catch (const http::HttpException& exception) {
        LOG_ERROR("Error fetching characters: {}", exception.what());

        promise->set_value(mapper::ErrorMapper::map(exception.what(), exception.statusCode()));
      } catch (...) {
        promise->set_exception(std::current_exception());
      }
    });
  } catch (const http::HttpException& exception) {
    promise->set_value(mapper::ErrorMapper::map(exception.what(), exception.statusCode()));
  } catch (...) {
    promise->set_exception(std::current_exception());
  }

  return future;
}
