#pragma once

#include <vector>

#include "domain/model/Character.h"

namespace domain::usecase {
class IGetCharactersUseCase {
public:
  virtual ~IGetCharactersUseCase() = default;

  virtual model::CharacterView execute(common::types::Int32 limit, common::types::Int32 page) = 0;

  virtual model::CharacterView nextPage() = 0;

  virtual model::CharacterView previousPage() = 0;
};
} // namespace domain::usecase
