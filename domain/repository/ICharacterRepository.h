#pragma once

#include <variant>

#include "common/async/Task.h"
#include "domain/model/CharactersResponse.h"
#include "domain/model/error/Error.h"

namespace domain::repository {

class ICharacterRepository {
public:
  using GetCharactersValue = std::variant<model::CharactersResponse, model::error::Error>;
  using GetCharactersResult = common::async::Task<GetCharactersValue>;

  virtual ~ICharacterRepository() = default;

  virtual GetCharactersResult getCharactersPaginated(common::types::Int32 limit, common::types::Int32 page) = 0;
};

} // namespace domain::repository
