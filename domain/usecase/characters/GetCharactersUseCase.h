#pragma once

#include <memory>
#include <vector>

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

  model::CharacterView execute(common::types::Int32 limit, common::types::Int32 page) override;

  model::CharacterView nextPage() override;
  model::CharacterView previousPage() override;

private:
  std::shared_ptr<repository::ICharacterRepository> m_characterRepository;
  std::shared_ptr<repository::ICharacterImageRepository> m_characterImageRepository;
  std::shared_ptr<repository::IImageResizerRepository> m_imageResizerRepository;

  common::types::Int32 m_currentPage;
  common::types::Int32 m_TotalPages;
  common::types::Int32 m_TotalElements;
  common::types::Int32 m_currentLimit;

  model::Pagination m_pagination;
};
} // namespace domain::usecase
