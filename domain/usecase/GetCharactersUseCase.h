#pragma once

#include <memory>
#include <vector>

#include "IGetCharactersUseCase.h"
#include "domain/model/CharactersResponse.h"

namespace domain::repository {
class ICharacterRepository;
}

namespace domain::usecase {
class GetCharactersUseCase final : public IGetCharactersUseCase {
public:
  explicit GetCharactersUseCase(const std::shared_ptr<repository::ICharacterRepository>& characterRepository);

  ~GetCharactersUseCase() override = default;

  model::CharactersResponse execute(common::types::Int32 limit, common::types::Int32 page) override;

  model::CharactersResponse nextPage() override;
  model::CharactersResponse previousPage() override;

private:
  std::shared_ptr<repository::ICharacterRepository> m_characterRepository;
  common::types::Int32 m_currentPage;
  common::types::Int32 m_TotalPages;
  common::types::Int32 m_TotalElements;
  common::types::Int32 m_currentLimit;

  model::Pagination m_pagination;

};
} // namespace domain::usecase
