#pragma once

#include <memory>

#include "IGetCharactersUseCase.h"

namespace domain::repository {
class IImageResizerRepository;
class ICharacterRepository;
class ICharacterImageRepository;
} // namespace domain::repository

namespace domain::usecase {

struct GetCharactersUseCaseState;

class GetCharactersUseCase final : public IGetCharactersUseCase {
public:
  explicit GetCharactersUseCase(const std::shared_ptr<repository::ICharacterRepository>& characterRepository,
                                const std::shared_ptr<repository::ICharacterImageRepository>& characterImageRepository,
                                const std::shared_ptr<repository::IImageResizerRepository>& imageResizerRepository);

  ~GetCharactersUseCase() override = default;

  Result execute(common::types::Int32 limit, common::types::Int32 page) override;
  Result nextPage() override;
  Result previousPage() override;

private:
  static common::types::Int32 toCharacterId(const model::CharacterInfo& info);
  static common::async::Task<model::CharacterWithPath>
  processCharacter(const std::shared_ptr<GetCharactersUseCaseState>& state, model::Character character);
  static common::async::Task<model::CharacterView>
  executeRequest(const std::shared_ptr<GetCharactersUseCaseState>& state, common::types::Int32 limit,
                 common::types::Int32 page);

  std::shared_ptr<GetCharactersUseCaseState> m_state;
};

} // namespace domain::usecase
