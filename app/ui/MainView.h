#pragma once

#include "lv/core/display.hpp"
#include "lv/core/style.hpp"
#include "lv/widgets/label.hpp"

namespace app::ui {
class MainViewModel;

class MainView {
public:
  MainView(int32_t width, int32_t height, MainViewModel& viewModel);
  ~MainView() = default;
  MainView(const MainView&) = delete;
  MainView& operator=(const MainView&) = delete;

private:
  void init();
  void initStyles();

  void onClick(lv::Event event);

  MainViewModel& m_viewModel;

  lv::SDLDisplay m_display;
  lv::ObjectView m_screen;

  lv::Style m_screenStyle;
  lv::Style m_titleStyle;
  lv::Style m_buttonStyle;
  lv::Style m_buttonPressedStyle;

  lv::Label m_titleLabel;
};
} // namespace app::ui
