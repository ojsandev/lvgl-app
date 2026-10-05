#pragma once

#include <vector>

namespace domain::model {
class CharactersResponse;
}

namespace data::mapper {
class CharacterMapper {
public:
  CharacterMapper() = default;
  ~CharacterMapper() = default;

  static domain::model::CharactersResponse fromJson(std::string_view json);
};
} // namespace data::mapper
