#include "CharacterList.h"

#include "lv/layout/flex.hpp"
#include "ui/theme/Colors.h"

using namespace app::ui;

CharacterList::CharacterList(const lv::ObjectView parent)
  : m_container(lv::vbox(parent)
                    .fill()
                    .grow(1)
                    .padding(4)
                    .gap(12)
                    .scrollable(true)
                    .scroll_dir(LV_DIR_VER)
                    .scrollbar_mode(LV_SCROLLBAR_MODE_ON)
                    .border_color(theme::border())
                    .bg_color(theme::background()))
{}

void CharacterList::setCharacters(const std::vector<domain::model::CharacterWithPath>& characters)
{
  m_container.clean();

  m_characterCards.clear();
  m_characterCards.reserve(characters.size());

  for (const auto& character : characters) {
    m_characterCards.push_back(std::make_unique<CharacterCard>(m_container, character));
  }
}
