#include "GetCharactersUseCase.h"

#include <stdexcept>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

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
    , m_pagination({}) {}

IGetCharactersUseCase::Result GetCharactersUseCase::execute(const common::types::Int32 limit,
                                                            const common::types::Int32 page)
{
  if (page <= 0 || limit <= 0) {
    throw exception::CharacterInvalidArgsException();
  }

  m_currentPage = page;
  m_currentLimit = limit;

  LOG_INFO("Fetching characters page={}, limit={}", m_currentPage, m_currentLimit);

  const auto responseResult = co_await m_characterRepository->getCharactersPaginated(m_currentLimit, m_currentPage);

  if (std::holds_alternative<model::error::Error>(responseResult)) {
    const auto& error = std::get<model::error::Error>(responseResult);

    if (error.reason == model::error::Reason::NotFound) {
      throw exception::CharacterNotFoundException();
    }

    LOG_ERROR("Failed to fetch characters; API returned an application error");

    throw std::runtime_error("Character API returned an application error");
  }

  const auto& response = std::get<model::CharactersResponse>(responseResult);

  const auto& characters = response.characters;
  m_pagination = response.pagination;

  std::vector<model::CharacterWithPath> convertedCharacters;
  convertedCharacters.reserve(characters.size());

  for (const auto& character : characters) {
    try {
      convertedCharacters.push_back(co_await processCharacter(character));
    } catch (const std::exception& error) {
      LOG_ERROR("Failed to process characterId {}: {}", character.info.id, error.what());

      convertedCharacters.push_back({
        .info = character.info,
        .imagePath = "",
        .thumbnailPath = ""
      });
    } catch (...) {
      LOG_ERROR("Unknown error processing characterId {}", character.info.id);

      convertedCharacters.push_back({
        .info = character.info,
        .imagePath = "",
        .thumbnailPath = ""
      });
    }
  }

  co_return model::CharacterView{
    .characters = std::move(convertedCharacters),
    .pagination = m_pagination
  };
}

common::async::Task<domain::model::CharacterWithPath> GetCharactersUseCase::processCharacter(
    const model::Character& character)
{
  const auto& info = character.info;
  const auto imagePath = m_characterImageRepository->getImagePath(info.id);
  const auto thumbnailPath = m_characterImageRepository->getThumbnailPath(info.id);

  // Download and store the original image if it does not exist.
  if (!m_characterImageRepository->imageExists(info.id)) {
    const auto storeResult = co_await m_characterImageRepository->storeImage(info.id, imagePath);

    const auto* ioResult = std::get_if<model::IOResult>(&storeResult);

    if (ioResult == nullptr || ioResult->type != model::IOResultType::Success) {
      LOG_ERROR("Failed to store image for characterId {}", info.id);

      co_return model::CharacterWithPath{
        .info = info,
        .imagePath = "",
        .thumbnailPath = ""
      };
    }
  }

  // Create the thumbnail immediately after the original image is available.
  if (!m_characterImageRepository->thumbnailExists(info.id)) {
    if (!m_imageResizerRepository->createThumbnail(imagePath, thumbnailPath, 150, 150)) {
      LOG_ERROR("Failed to create thumbnail for characterId {}", info.id);

      co_return model::CharacterWithPath{
        .info = info,
        .imagePath = imagePath,
        .thumbnailPath = ""
      };
    }
  }

  co_return model::CharacterWithPath{
    .info = info,
    .imagePath = imagePath,
    .thumbnailPath = thumbnailPath
  };
}

IGetCharactersUseCase::Result
GetCharactersUseCase::nextPage()
{
  if (m_currentPage >= m_pagination.totalPages) {
    throw exception::CharactersOutOfBoundsException();
  }

  return execute(m_currentLimit, m_currentPage + 1);
}

IGetCharactersUseCase::Result
GetCharactersUseCase::previousPage()
{
  if (m_currentPage <= 1) {
    throw exception::CharactersOutOfBoundsException();
  }

  return execute(m_currentLimit, m_currentPage - 1);
}
