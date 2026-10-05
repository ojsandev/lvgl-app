#include "CharacterRepository.h"

#include <print>

#include "data/datasource/DemonSlayerDataSource.h"
#include "data/mapper/CharacterMapper.h"
#include "domain/model/CharactersResponse.h"
#include "domain/model/error/Error.h"
#include "infra/http/exception/HttpException.h"
#include "mapper/ErrorMapper.h"

using namespace data::repository;

CharacterRepository::CharacterRepository(
    const std::shared_ptr<datasource::DemonSlayerDataSource>& demonSlayerDataSource)
  : m_demonSlayerDataSource(demonSlayerDataSource) {}

domain::repository::ICharacterRepository::GetCharactersResult CharacterRepository::getCharactersPaginated(
    const common::types::Int32 limit, const common::types::Int32 page)
{
  try {
    const auto json = m_demonSlayerDataSource->getCharacters(limit, page);
    return mapper::CharacterMapper::fromJson(json);
  }
  catch (infra::http::exception::HttpException& exception) {
    std::print("CharacterRepository::getCharactersPaginated: {} - {}\n", exception.what(), exception.statusCode());

    return mapper::ErrorMapper::map(exception.what(), exception.statusCode());
  }
}
