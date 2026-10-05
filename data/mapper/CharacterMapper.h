#pragma once

#include <string_view>

namespace domain::model {
class CharactersResponse;
}

namespace data::mapper {
class CharacterMapper {
public:
  CharacterMapper() = default;
  ~CharacterMapper() = default;

  static domain::model::CharactersResponse fromJson(const std::string_view& json);
};
} // namespace data::mapper
