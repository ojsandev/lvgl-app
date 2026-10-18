#include "CharacterRepository.h"

#include <exception>
#include <memory>

#include "common/async/Task.h"
#include "common/logger/Logging.h"
#include "data/datasource/DemonSlayerDataSource.h"
#include "data/http/HttpException.h"
#include "data/mapper/CharacterMapper.h"
#include "mapper/ErrorMapper.h"

using namespace data::repository;

CharacterRepository::CharacterRepository(
  const std::shared_ptr<datasource::DemonSlayerDataSource>& demonSlayerDataSource)
  : m_demonSlayerDataSource(demonSlayerDataSource)
{}

domain::repository::ICharacterRepository::GetCharactersResult
CharacterRepository::getCharactersPaginated(
  const common::types::Int32 limit,
  const common::types::Int32 page)
{
  using Result = domain::repository::ICharacterRepository::GetCharactersValue;
  common::async::TaskCompletionSource<Result> completion;
  auto task = completion.task();

  try {
    m_demonSlayerDataSource->getCharacters(
      limit,
      page,
      [completion](http::AsyncHttpStringResult result) mutable {
        try {
          if (!result.hasValue()) {
            result.rethrowError();
          }

          const auto& json = result.value();
          LOG_INFO("Response received: {} bytes", json.size());
          completion.set_value(mapper::CharacterMapper::fromJson(json));
        } catch (const http::HttpException& exception) {
          LOG_ERROR("Error fetching characters: {}", exception.what());
          completion.set_value(mapper::ErrorMapper::map(exception.what(), exception.statusCode()));
        } catch (...) {
          completion.set_exception(std::current_exception());
        }
      });
  } catch (const http::HttpException& exception) {
    completion.set_value(mapper::ErrorMapper::map(exception.what(), exception.statusCode()));
  } catch (...) {
    completion.set_exception(std::current_exception());
  }

  return task;
}
