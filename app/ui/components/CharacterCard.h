#pragma once

#include <string>

#include "domain/model/Character.h"
#include "lv/core/object.hpp"
#include "lv/widgets/label.hpp"

namespace app::ui {

class CharacterCard {
public:
  CharacterCard(lv::ObjectView parent, const domain::model::CharacterWithPath& character);

  CharacterCard(const CharacterCard&) = delete;
  CharacterCard& operator=(const CharacterCard&) = delete;

  ~CharacterCard() = default;

private:
  void init(const domain::model::CharacterWithPath& character);

private:
  lv::ObjectView m_card;

  lv::Label m_nameLabel;
  lv::Label m_infoLabel;
  lv::Label m_descriptionLabel;

  std::string m_imagePath;
};

} // namespace app::ui
