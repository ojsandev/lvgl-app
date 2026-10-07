#include "MainView.h"

#include <print>

#include "MainViewModel.h"
#include "common/logger/Logging.h"
#include "lv/layout/flex.hpp"
#include "lv/widgets/button.hpp"

using namespace app::ui;

namespace {
constexpr auto kBackground = lv_color_hex(0x0F1117);
constexpr auto kSurface = lv_color_hex(0x191C24);
constexpr auto kPrimary = lv_color_hex(0x6C63FF);
constexpr auto kPrimaryPressed = lv_color_hex(0x554DCC);
constexpr auto kText = lv_color_hex(0xF8FAFC);
constexpr auto kTextSecondary = lv_color_hex(0xA7AFBF);
constexpr auto kBorder = lv_color_hex(0x292E3A);
} // namespace

MainView::MainView(const int32_t width, const int32_t height, MainViewModel& viewModel)
  : m_viewModel(viewModel)
    , m_display(width, height)
    , m_screen(m_display.screen_active())
{
  init();
}

void MainView::init()
{
  initStyles();

  m_screen.add_style(m_screenStyle.get());

  const auto root = lv::vbox(m_screen)
                         .fill()
                         .center_content()
                         .padding(32)
                         .gap(16)
                         .bg_color(kBackground);

  m_titleLabel = lv::Label::create(root)
                     .text("Characters")
                     .add_style(m_titleStyle.get());

  lv::Button::create(root)
      .text("Load characters")
      .add_style(m_buttonStyle.get())
      .add_style(m_buttonPressedStyle.get(), LV_STATE_PRESSED)
      .on_click<&MainView::onClick>(this);
}

void MainView::initStyles()
{
  m_screenStyle
      .bg_color(kBackground)
      .bg_opa(LV_OPA_COVER);

  m_titleStyle
      .text_color(kText)
      .text_align(LV_TEXT_ALIGN_CENTER)
      .pad_all(8);

  m_buttonStyle
      .bg_color(kPrimary)
      .bg_opa(LV_OPA_COVER)
      .text_color(kText)
      .radius(12)
      .padding(14)
      .padding_hor(24)
      .border_width(0)
      .shadow_width(12)
      .shadow_opa(LV_OPA_20)
      .shadow_offset(0, 4);

  m_buttonPressedStyle
      .bg_color(kPrimaryPressed)
      .shadow_width(4)
      .shadow_opa(LV_OPA_10)
      .transform_scale(245);
}

void MainView::onClick(lv::Event)
{
  auto uiData = m_viewModel.update();
  m_titleLabel.text_fmt(
      "Title: %s\nCharacters: %d",
      uiData.title.c_str(),
      uiData.characters.size());

  LOG_DEBUG("title updated to: {}", uiData.title);
}
