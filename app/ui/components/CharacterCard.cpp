#include "CharacterCard.h"

#include <functional>
#include <string>

#include "common/logger/Logging.h"
#include "utils/LVGLUtils.h"
#include "lv/core/display.hpp"
#include "lv/layout/flex.hpp"
#include "lv/widgets/image.hpp"
#include "ui/theme/Colors.h"

using namespace app::ui;

namespace {

std::string toLvglPath(const std::string& path)
{
  if (path.empty()) {
    return {};
  }

  if (path.rfind("A:", 0) == 0) {
    return path;
  }

  if (path[0] == '/') {
    return "A:" + path;
  }

  return "A:/" + path;
}
} // namespace

CharacterCard::CharacterCard(const lv::ObjectView parent, const domain::model::CharacterWithPath& character)
  : m_card(parent)
{
  init(character);
}

void CharacterCard::init(const domain::model::CharacterWithPath& character)
{
  auto root = lv::hbox(m_card)
              .name("CharacterCard")
              .width_pct(100)
              .height(150)
              .padding(12)
              .gap(16)
              .radius(16)
              .bg_color(theme::surface())
              .border_width(1)
              .border_color(theme::border())
              .border_opa(LV_OPA_COVER)
              .shadow_width(12)
              .shadow_opa(LV_OPA_20)
              .shadow_color(lv_color_black());

  m_imagePath = toLvglPath(character.thumbnailPath);

  auto image = lv::Image::create(root);

  if (!m_imagePath.empty()) {
    image.src(m_imagePath.c_str());
  }

  const auto info = lv::vbox(root).name("CharacterInfo").fill_height().grow(1).padding(4).column_gap(6);

  m_nameLabel = lv::Label::create(info)
                .text(character.info.name.c_str()).text_color(theme::text());

  m_infoLabel = lv::Label::create(info)
                .text_fmt("Age: %d  •  Gender: %s", character.info.age, character.info.gender.c_str())
                .text_color(theme::textSecondary());

  m_descriptionLabel = lv::Label::create(info)
                       .text(character.info.description.c_str())
                       .fill_width()
                       .fill_height()
                       .text_color(theme::textSecondary())
                       .long_mode(LV_LABEL_LONG_MODE_DOTS);

  utils::enableEventBubble(root);

  root.on_hover_over([](const lv::Event e) {
    const auto targetName = e.current_target().get_name();
    const auto currentTargetName = e.target().get_name();
    LOG_DEBUG("Hover OVER | target={} | current={}", targetName, currentTargetName);
  });
}
