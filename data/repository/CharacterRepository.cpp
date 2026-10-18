#include "CharacterRepository.h"

#include <exception>
#include <memory>

#include "common/async/Task.h"
#include "common/logger/Logging.h"
#include "data/datasource/DemonSlayerDataSource.h"
#include "data/mapper/CharacterMapper.h"
#include "data/mapper/ErrorMapper.h"
#include "http/HttpException.h"

using namespace data::repository;

namespace {

using Completion = common::async::TaskCompletionSource<domain::repository::ICharacterRepository::GetCharactersValue>;

void handleError(const Completion& completion, std::exception_ptr error)
{
  try {
    std::rethrow_exception(error);
  } catch (const data::http::HttpException& exception) {
    LOG_ERROR("HTTP error: {}", exception.what());

    completion.set_value(data::mapper::ErrorMapper::map(exception.what(), exception.statusCode()));
  } catch (...) {
    completion.set_exception(error);
  }
}

} // namespace

CharacterRepository::CharacterRepository(
    const std::shared_ptr<datasource::DemonSlayerDataSource>& demonSlayerDataSource)
  : m_demonSlayerDataSource(demonSlayerDataSource)
{}

domain::repository::ICharacterRepository::GetCharactersResult
CharacterRepository::getCharactersPaginated(const common::types::Int32 limit, const common::types::Int32 page)
{
  common::async::TaskCompletionSource<GetCharactersValue> completion;
  auto task = completion.task();

  try {
    m_demonSlayerDataSource->getCharacters(limit, page,
                                           [completion, this](const http::AsyncHttpStringResult& result) mutable {
                                             this->onResponse(result, completion);
                                           });
  } catch (...) {
    handleError(completion, std::current_exception());
  }

  return task;
}

void CharacterRepository::onResponse(const http::AsyncHttpStringResult& result, const Completion& completion)
{
  try {
    processResponse(result, completion);
  } catch (...) {
    handleError(completion, std::current_exception());
  }
}

void CharacterRepository::processResponse(const data::http::AsyncHttpStringResult& result, const Completion& completion)
{
  if (!result.hasValue()) {
    result.rethrowError();
  }

  const auto& json = result.value();

  completion.set_value(data::mapper::CharacterMapper::fromJson(json));
}
