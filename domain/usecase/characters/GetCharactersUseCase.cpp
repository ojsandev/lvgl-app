#include "GetCharactersUseCase.h"

#include <print>

#include "common/logger/Logging.h"
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
    LOG_ERROR("Invalid page number: {}. Page number must be greater than 0.", page);
    throw exception::CharacterInvalidArgsException();
  }

  if (limit <= 0) {
    LOG_ERROR("Invalid limit: {}. Limit must be greater than 0.", limit);
    throw exception::CharacterInvalidArgsException();
  }

  m_currentPage = page;
  m_currentLimit = limit;

  LOG_INFO("page={}, limit={}", m_currentPage, m_currentLimit);

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

  LOG_ERROR("Unknown error occurred while fetching characters");
  return {};
}

domain::model::CharactersResponse GetCharactersUseCase::nextPage()
{
  if (m_currentPage >= m_pagination.totalPages) {
    LOG_ERROR("Cannot go to next page. Current page: {}, Total pages: {}", m_currentPage, m_pagination.totalPages);
    throw exception::CharactersOutOfBoundsException();
  }

  return execute(m_currentLimit, m_currentPage + 1);
}

domain::model::CharactersResponse GetCharactersUseCase::previousPage()
{
  if (m_currentPage <= 1) {
    LOG_ERROR("Cannot go to previous page. Current page: {}", m_currentPage);
    throw exception::CharactersOutOfBoundsException();
  }

  return execute(m_currentLimit, m_currentPage - 1);
}
