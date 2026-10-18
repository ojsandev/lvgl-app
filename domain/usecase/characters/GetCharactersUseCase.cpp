#include "GetCharactersUseCase.h"

#include <limits>
#include <stdexcept>
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

struct domain::usecase::GetCharactersUseCaseState
{
  std::shared_ptr<repository::ICharacterRepository> characterRepository;
  std::shared_ptr<repository::ICharacterImageRepository> characterImageRepository;
  std::shared_ptr<repository::IImageResizerRepository> imageResizerRepository;
  common::types::Int32 currentPage = 0;
  common::types::Int32 currentLimit = 0;
  model::Pagination pagination = {};
};

GetCharactersUseCase::GetCharactersUseCase(
    const std::shared_ptr<repository::ICharacterRepository>& characterRepository,
    const std::shared_ptr<repository::ICharacterImageRepository>& characterImageRepository,
    const std::shared_ptr<repository::IImageResizerRepository>& imageResizerRepository)
  : m_state(std::make_shared<GetCharactersUseCaseState>(
        GetCharactersUseCaseState{ .characterRepository = characterRepository,
                                   .characterImageRepository = characterImageRepository,
                                   .imageResizerRepository = imageResizerRepository,
                                   .pagination = {} }))
{}

IGetCharactersUseCase::Result GetCharactersUseCase::execute(const common::types::Int32 limit,
                                                            const common::types::Int32 page)
{
  if (page <= 0 || limit <= 0) {
    throw exception::CharacterInvalidArgsException();
  }

  return executeRequest(m_state, limit, page);
}

IGetCharactersUseCase::Result GetCharactersUseCase::nextPage()
{
  if (m_state->currentPage >= m_state->pagination.totalPages) {
    throw exception::CharactersOutOfBoundsException();
  }

  return execute(m_state->currentLimit, m_state->currentPage + 1);
}

IGetCharactersUseCase::Result GetCharactersUseCase::previousPage()
{
  if (m_state->currentPage <= 1) {
    throw exception::CharactersOutOfBoundsException();
  }

  return execute(m_state->currentLimit, m_state->currentPage - 1);
}

common::types::Int32 GetCharactersUseCase::toCharacterId(const domain::model::CharacterInfo& info)
{
  constexpr auto minId = static_cast<common::types::Int64>(std::numeric_limits<common::types::Int32>::min());
  constexpr auto maxId = static_cast<common::types::Int64>(std::numeric_limits<common::types::Int32>::max());

  if (info.id < minId || info.id > maxId) {
    throw std::out_of_range("Character id is outside Int32 range");
  }

  return static_cast<common::types::Int32>(info.id);
}

common::async::Task<domain::model::CharacterWithPath>
GetCharactersUseCase::processCharacter(const std::shared_ptr<GetCharactersUseCaseState>& state,
                                       const model::Character character)
{
  const auto& info = character.info;
  const auto characterId = toCharacterId(info);
  const auto imagePath = state->characterImageRepository->getImagePath(characterId);
  const auto thumbnailPath = state->characterImageRepository->getThumbnailPath(characterId);

  if (!state->characterImageRepository->imageExists(characterId)) {
    const auto storeResult = co_await state->characterImageRepository->storeImage(characterId, imagePath);
    const auto* ioResult = std::get_if<model::IOResult>(&storeResult);

    if (ioResult == nullptr || ioResult->type != model::IOResultType::Success) {
      LOG_ERROR("Failed to store image for characterId {}", info.id);

      co_return model::CharacterWithPath{ .info = info, .imagePath = "", .thumbnailPath = "" };
    }
  }

  if (!state->characterImageRepository->thumbnailExists(characterId)) {
    if (!state->imageResizerRepository->createThumbnail(imagePath, thumbnailPath, 150, 150)) {
      LOG_ERROR("Failed to create thumbnail for characterId {}", info.id);

      co_return model::CharacterWithPath{ .info = info, .imagePath = imagePath, .thumbnailPath = "" };
    }
  }

  co_return model::CharacterWithPath{ .info = info, .imagePath = imagePath, .thumbnailPath = thumbnailPath };
}

common::async::Task<domain::model::CharacterView>
GetCharactersUseCase::executeRequest(const std::shared_ptr<GetCharactersUseCaseState>& state,
                                     const common::types::Int32 limit, const common::types::Int32 page)
{
  state->currentPage = page;
  state->currentLimit = limit;

  LOG_INFO("Fetching characters page={}, limit={}", state->currentPage, state->currentLimit);

  const auto responseResult = co_await state->characterRepository->getCharactersPaginated(limit, page);

  if (std::holds_alternative<model::error::Error>(responseResult)) {
    const auto& error = std::get<model::error::Error>(responseResult);

    if (error.reason == model::error::Reason::NotFound) {
      throw exception::CharacterNotFoundException();
    }

    LOG_ERROR("Failed to fetch characters; API returned an application error");

    throw std::runtime_error("Character API returned an application error");
  }

  const auto& response = std::get<model::CharactersResponse>(responseResult);
  state->pagination = response.pagination;

  std::vector<common::async::Task<model::CharacterWithPath>> characterTasks;
  characterTasks.reserve(response.characters.size());

  for (const auto& character : response.characters) {
    characterTasks.push_back(processCharacter(state, character));
  }

  std::vector<model::CharacterWithPath> convertedCharacters;
  convertedCharacters.reserve(response.characters.size());

  for (std::size_t index = 0; index < response.characters.size(); ++index) {
    const auto& character = response.characters[index];
    try {
      convertedCharacters.push_back(co_await characterTasks[index]);
    } catch (const std::exception& error) {
      LOG_ERROR("Failed to process characterId {}: {}", character.info.id, error.what());

      convertedCharacters.push_back({ .info = character.info, .imagePath = "", .thumbnailPath = "" });
    } catch (...) {
      LOG_ERROR("Unknown error processing characterId {}", character.info.id);

      convertedCharacters.push_back({ .info = character.info, .imagePath = "", .thumbnailPath = "" });
    }
  }

  co_return model::CharacterView{ .characters = std::move(convertedCharacters), .pagination = state->pagination };
}
