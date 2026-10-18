#pragma once

#include <memory>

#include "IGetCharactersUseCase.h"
#include "domain/model/Pagination.h"

namespace domain::repository {
class IImageResizerRepository;
class ICharacterRepository;
class ICharacterImageRepository;
} // namespace domain::repository

namespace domain::usecase {

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
  common::async::Task<model::CharacterWithPath> processCharacter(const model::Character& character);

  std::shared_ptr<repository::ICharacterRepository> m_characterRepository;
  std::shared_ptr<repository::ICharacterImageRepository> m_characterImageRepository;
  std::shared_ptr<repository::IImageResizerRepository> m_imageResizerRepository;

  common::types::Int32 m_currentPage = 0;
  common::types::Int32 m_currentLimit = 0;
  model::Pagination m_pagination;
};

} // namespace domain::usecase
