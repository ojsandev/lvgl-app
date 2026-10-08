#pragma once

#include <vector>

#include "Character.h"
#include "Pagination.h"

namespace domain::model {
struct CharactersResponse
{
  std::vector<Character> characters;
  Pagination pagination;
};
} // namespace domain::model
