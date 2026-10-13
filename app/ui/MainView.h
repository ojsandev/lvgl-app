#pragma once

#include <memory>

#include "MainViewUIState.h"
#include "components/CharacterList.h"
#include "infra/display/IDisplay.h"
#include "lv/core/display.hpp"
#include "lv/widgets/button.hpp"
#include "lv/widgets/label.hpp"
#include "theme/Styles.h"

namespace app::ui {

class MainViewModel;

class MainView {
public:
  MainView(infra::display::IDisplay& display, MainViewModel& viewModel);
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

  infra::display::IDisplay& m_display;
  lv::ObjectView m_screen;

  theme::Styles m_styles;

  lv::Label m_titleLabel;
  lv::Label m_pageLabel;

  lv::Button m_previousButton;
  lv::Button m_nextButton;

  std::unique_ptr<CharacterList> m_characterList;
};

} // namespace app::ui
