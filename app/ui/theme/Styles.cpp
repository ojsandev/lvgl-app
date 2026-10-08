#include "Styles.h"

#include "Colors.h"

using namespace app::ui::theme;

Styles::Styles()
{
  m_screen.bg_color(background());
  m_title.text_color(text()).text_align(LV_TEXT_ALIGN_CENTER);
  m_pagination.bg_color(surface())
              .radius(14)
              .border_width(1)
              .border_color(border())
              .border_opa(LV_OPA_COVER)
              .shadow_width(8)
              .shadow_opa(LV_OPA_10)
              .shadow_color(lv_color_black());

  m_secondaryButton.bg_color(surfaceHover())
                   .radius(10)
                   .border_width(1)
                   .border_color(border())
                   .border_opa(LV_OPA_COVER)
                   .text_color(text());

  m_primaryButton.bg_color(primary()).radius(10).border_width(0).text_color(text());

  m_primaryButtonPressed.bg_color(primaryPressed()).radius(10).border_width(0).text_color(text());

  m_disabledButton.bg_color(surfaceHover())
                  .radius(10)
                  .border_width(1)
                  .border_color(border())
                  .text_color(textSecondary())
                  .bg_opa(LV_OPA_50)
                  .text_opa(LV_OPA_50);

  m_secondaryText.text_color(textSecondary());

  m_card.bg_color(surface())
        .radius(16)
        .border_width(1)
        .border_color(border())
        .border_opa(LV_OPA_COVER)
        .shadow_width(12)
        .shadow_opa(LV_OPA_20)
        .shadow_color(lv_color_black());

  m_cardInfo.pad_all(4);

  m_description.text_color(textSecondary());
}

lv::Style& Styles::screen()
{
  return m_screen;
}

lv::Style& Styles::title()
{
  return m_title;
}

lv::Style& Styles::pagination()
{
  return m_pagination;
}

lv::Style& Styles::secondaryButton()
{
  return m_secondaryButton;
}

lv::Style& Styles::primaryButton()
{
  return m_primaryButton;
}

lv::Style& Styles::primaryButtonPressed()
{
  return m_primaryButtonPressed;
}

lv::Style& Styles::disabledButton()
{
  return m_disabledButton;
}

lv::Style& Styles::secondaryText()
{
  return m_secondaryText;
}

lv::Style& Styles::card()
{
  return m_card;
}

lv::Style& Styles::cardInfo()
{
  return m_cardInfo;
}

lv::Style& Styles::description()
{
  return m_description;
}
