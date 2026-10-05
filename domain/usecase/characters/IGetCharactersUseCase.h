#pragma once

#include "domain/model/CharactersResponse.h"

namespace domain::usecase {
class IGetCharactersUseCase {
public:
  virtual ~IGetCharactersUseCase() = default;

  virtual model::CharactersResponse execute(common::types::Int32 limit, common::types::Int32 page) = 0;

  virtual model::CharactersResponse nextPage() = 0;

  virtual model::CharactersResponse previousPage() = 0;
};
} // namespace domain::usecase
