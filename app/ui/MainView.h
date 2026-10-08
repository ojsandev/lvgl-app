#pragma once

#include <memory>

#include "MainViewUIState.h"
#include "components/CharacterList.h"
#include "lv/core/display.hpp"
#include "lv/widgets/button.hpp"
#include "lv/widgets/label.hpp"
#include "theme/Styles.h"

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
  void render(const MainViewUIData& uiData);

  void onClick(lv::Event event);
  void onNextPage(lv::Event event);
  void onPreviousPage(lv::Event event);

private:
  MainViewModel& m_viewModel;

  lv::SDLDisplay m_display;
  lv::ObjectView m_screen;

  theme::Styles m_styles;

  lv::Label m_titleLabel;
  lv::Label m_pageLabel;

  lv::Button m_previousButton;
  lv::Button m_nextButton;

  std::unique_ptr<CharacterList> m_characterList;
};

} // namespace app::ui
