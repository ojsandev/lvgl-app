#include "GetCharactersUseCase.h"

#include <map>
#include <print>

#include "common/logger/Logging.h"
#include "domain/exception/characters/CharacterInvalidArgsException.h"
#include "domain/exception/characters/CharacterNotFoundException.h"
#include "domain/exception/characters/CharactersOutOfBoundsException.h"
#include "domain/repository/ICharacterImageRepository.h"
#include "domain/repository/ICharacterRepository.h"
#include "domain/repository/IImageResizerRepository.h"

using namespace domain::usecase;

GetCharactersUseCase::GetCharactersUseCase(
    const std::shared_ptr<repository::ICharacterRepository>& characterRepository,
    const std::shared_ptr<repository::ICharacterImageRepository>& characterImageRepository,
    const std::shared_ptr<repository::IImageResizerRepository>& imageResizerRepository)
  : m_characterRepository(characterRepository)
  , m_characterImageRepository(characterImageRepository)
  , m_imageResizerRepository(imageResizerRepository)
  , m_currentPage(0)
  , m_TotalPages(0)
  , m_TotalElements(0)
  , m_currentLimit(0)
  , m_pagination()
{}

domain::model::CharacterView GetCharactersUseCase::execute(const common::types::Int32 limit,
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
    const auto& [characters, pagination] = std::get<model::CharactersResponse>(variant);

    if (m_pagination != pagination) {
      m_pagination = pagination;
    }

    auto convertedCharacters = std::vector<model::CharacterWithPath>();
    auto characterWithPath = model::CharacterWithPath();

    for (const auto& [info, _] : characters) {
      const auto imagePath = m_characterImageRepository->getImagePath(info.id);

      const auto thumbnailPath = m_characterImageRepository->getThumbnailPath(info.id);

      if (!m_characterImageRepository->imageExists(info.id)) {

        if (const auto storeResult = m_characterImageRepository->storeImage(info.id, imagePath);
            !std::holds_alternative<model::IOResult>(storeResult)) {
          LOG_ERROR("Failed to store image for characterId {}", info.id);

          convertedCharacters.push_back({ .info = info, .imagePath = "", .thumbnailPath = "" });

          continue;
        }
      }

      if (!m_characterImageRepository->thumbnailExists(info.id)) {
        if (!m_imageResizerRepository->createThumbnail(imagePath, thumbnailPath, 150, 150)) {

          LOG_ERROR("Failed to create thumbnail for characterId {}", info.id);

          convertedCharacters.push_back({ .info = info, .imagePath = imagePath, .thumbnailPath = "" });

          continue;
        }
      }

      convertedCharacters.push_back({ .info = info, .imagePath = imagePath, .thumbnailPath = thumbnailPath });
    }

    return { .characters = convertedCharacters, .pagination = m_pagination };
  }

  if (const auto& [reason, code] = std::get<model::error::Error>(variant); reason == model::error::Reason::NotFound) {
    throw exception::CharacterNotFoundException();
  }

  LOG_ERROR("Unknown error occurred while fetching characters");
  return {};
}

domain::model::CharacterView GetCharactersUseCase::nextPage()
{
  if (m_currentPage >= m_pagination.totalPages) {
    LOG_ERROR("Cannot go to next page. Current page: {}, Total pages: {}", m_currentPage, m_pagination.totalPages);
    throw exception::CharactersOutOfBoundsException();
  }

  return execute(m_currentLimit, m_currentPage + 1);
}

domain::model::CharacterView GetCharactersUseCase::previousPage()
{
  if (m_currentPage <= 1) {
    LOG_ERROR("Cannot go to previous page. Current page: {}", m_currentPage);
    throw exception::CharactersOutOfBoundsException();
  }

  return execute(m_currentLimit, m_currentPage - 1);
}
