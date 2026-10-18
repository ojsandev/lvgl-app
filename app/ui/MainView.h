#pragma once

#include <memory>
#include <lvgl.h>

#include "MainViewUIState.h"
#include "components/CharacterList.h"
#include "app/display/IDisplay.h"
#include "lv/core/display.hpp"
#include "lv/widgets/button.hpp"
#include "lv/widgets/label.hpp"
#include "theme/Styles.h"


namespace app::ui {

class MainViewModel;

class MainView {
public:
  MainView(app::display::IDisplay& display, MainViewModel& viewModel);
  ~MainView();

  MainView(const MainView&) = delete;
  MainView& operator=(const MainView&) = delete;

private:
  void init();
  void render(const MainViewUIData& uiData);
  void showLoading();

  static void onPollTimer(lv_timer_t* timer);
  void onClick(lv::Event event);
  void onNextPage(lv::Event event);
  void onPreviousPage(lv::Event event);

  MainViewModel& m_viewModel;
  app::display::IDisplay& m_display;
  lv::ObjectView m_screen;
  lv_timer_t* m_pollTimer = nullptr;

  theme::Styles m_styles;
  lv::Label m_titleLabel;
  lv::Label m_pageLabel;
  lv::Button m_previousButton;
  lv::Button m_nextButton;
  std::unique_ptr<CharacterList> m_characterList;
};

} // namespace app::ui
