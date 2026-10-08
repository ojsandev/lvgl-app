#pragma once

#include <memory>
#include <vector>

#include "CharacterCard.h"
#include "lv/core/object.hpp"

namespace app::ui {

class CharacterList {
public:
  explicit CharacterList(lv::ObjectView parent);

  CharacterList(const CharacterList&) = delete;
  CharacterList& operator=(const CharacterList&) = delete;

  ~CharacterList() = default;

  void setCharacters(const std::vector<domain::model::CharacterWithPath>& characters);

private:
  lv::ObjectView m_container;

  std::vector<std::unique_ptr<CharacterCard>> m_characterCards;
};

} // namespace app::ui
