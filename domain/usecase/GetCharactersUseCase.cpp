#include "GetCharactersUseCase.h"

#include <print>

#include "domain/exception/characters/CharacterInvalidArgsException.h"
#include "domain/exception/characters/CharactersOutOfBoundsException.h"
#include "domain/exception/characters/CharacterNotFoundException.h"
#include "domain/repository/ICharacterRepository.h"

using namespace domain::usecase;

GetCharactersUseCase::GetCharactersUseCase(const std::shared_ptr<repository::ICharacterRepository>& characterRepository)
  : m_characterRepository(characterRepository),
    m_currentPage(0),
    m_TotalPages(0),
    m_TotalElements(0),
    m_currentLimit(0),
    m_pagination() {}

domain::model::CharactersResponse GetCharactersUseCase::execute(const common::types::Int32 limit,
                                                                const common::types::Int32 page)
{
  if (page <= 0) {
    throw exception::CharacterInvalidArgsException();
  }

  if (limit <= 0) {
    throw exception::CharacterInvalidArgsException();
  }

  m_currentPage = page;
  m_currentLimit = limit;

  std::print("GetCharactersUseCase::execute: page={}, limit={}\n", m_currentPage, m_currentLimit);

  const auto variant = m_characterRepository->getCharactersPaginated(m_currentLimit, m_currentPage);

  if (std::holds_alternative<model::CharactersResponse>(variant)) {
    const auto& response = std::get<model::CharactersResponse>(variant);

    if (m_pagination != response.pagination) {
      m_pagination = response.pagination;
    }

    return response;
  }

  if (const auto& [reason, code] = std::get<model::error::Error>(variant); reason == model::error::Reason::NotFound) {
    throw exception::CharacterNotFoundException();
  }
  return {};
}

domain::model::CharactersResponse GetCharactersUseCase::nextPage()
{
  if (m_currentPage >= m_pagination.totalPages) {
    throw exception::CharactersOutOfBoundsException();
  }

  return execute(m_currentLimit, m_currentPage + 1);
}

domain::model::CharactersResponse GetCharactersUseCase::previousPage()
{
  if (m_currentPage <= 1) {
    throw exception::CharactersOutOfBoundsException();
  }

  return execute(m_currentLimit, m_currentPage - 1);
}
