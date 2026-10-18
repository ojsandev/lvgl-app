#pragma once

#include "common/async/Task.h"
#include "domain/model/Character.h"

namespace domain::usecase {

class IGetCharactersUseCase {
public:
  using Result = common::async::Task<model::CharacterView>;

  virtual ~IGetCharactersUseCase() = default;

  virtual Result execute(common::types::Int32 limit, common::types::Int32 page) = 0;

  virtual Result nextPage() = 0;

  virtual Result previousPage() = 0;
};

} // namespace domain::usecase
