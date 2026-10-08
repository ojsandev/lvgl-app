#pragma once

#include "lv/core/style.hpp"

namespace app::ui::theme {

class Styles {
public:
  Styles();

  lv::Style& screen();
  lv::Style& title();

  lv::Style& pagination();

  lv::Style& secondaryButton();
  lv::Style& primaryButton();
  lv::Style& primaryButtonPressed();
  lv::Style& disabledButton();

  lv::Style& secondaryText();

  lv::Style& card();
  lv::Style& cardInfo();
  lv::Style& description();

private:
  lv::Style m_screen;
  lv::Style m_title;

  lv::Style m_pagination;

  lv::Style m_secondaryButton;
  lv::Style m_primaryButton;
  lv::Style m_primaryButtonPressed;
  lv::Style m_disabledButton;

  lv::Style m_secondaryText;

  lv::Style m_card;
  lv::Style m_cardInfo;
  lv::Style m_description;
};

} // namespace app::ui::theme
