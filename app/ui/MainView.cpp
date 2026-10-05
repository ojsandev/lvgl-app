#include "MainView.h"

#include <print>

#include "MainViewModel.h"
#include "lv/layout/flex.hpp"
#include "lv/widgets/button.hpp"

using namespace app::ui;

MainView::MainView(const int32_t width, const int32_t height, MainViewModel& viewModel)
  : m_viewModel(viewModel)
    , m_display(width, height)
    , m_screen(m_display.screen_active())
{
  init();
}

void MainView::init()
{
  const auto root = lv::vbox(m_screen).fill().center_content();

  m_titleLabel = lv::Label::create(root).text("");

  lv::Button::create(root).text("Click").on_click<&MainView::onClick>(this);

  std::print("MainView::init: widgets created\n");
}

void MainView::onClick(lv::Event)
{
  auto uiData = m_viewModel.update();
  m_titleLabel.text_fmt("Title: %s\n Characters: %d", uiData.title.c_str(), uiData.characters.size());

  std::print("MainView::onClick: title updated to: {}\n", uiData.title);
}
