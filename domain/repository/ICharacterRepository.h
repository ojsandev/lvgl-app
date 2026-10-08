#pragma once

#include <variant>

#include "domain/model/CharactersResponse.h"
#include "domain/model/error/Error.h"

namespace domain::repository {
class ICharacterRepository {
public:
  using GetCharactersResult = std::variant<model::CharactersResponse, model::error::Error>;

  virtual ~ICharacterRepository() = default;

  virtual GetCharactersResult getCharactersPaginated(common::types::Int32 limit, common::types::Int32 page) = 0;
};
} // namespace domain::repository
