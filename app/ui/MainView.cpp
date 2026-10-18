#include "MainView.h"

#include <lvgl.h>

#include "MainViewModel.h"
#include "common/logger/Logging.h"
#include "lv/layout/flex.hpp"
#include "theme/Colors.h"

using namespace app::ui;

MainView::MainView(display::IDisplay& display, MainViewModel& viewModel)
  : m_viewModel(viewModel)
  , m_display(display)
  , m_screen(m_display.lvgl().screen_active())
{
  init();
}

MainView::~MainView()
{
  if (m_pollTimer != nullptr) {
    lv_timer_delete(m_pollTimer);
    m_pollTimer = nullptr;
  }
}

void MainView::init()
{
  const auto root = lv::vbox(m_screen)
                        .fill()
                        .padding(8)
                        .gap(16)
                        .radius(false)
                        .border_color(theme::border())
                        .bg_color(theme::background());

  m_titleLabel = lv::Label::create(root).text("Demon Slayer");
  m_titleLabel.add_style(m_styles.title().get());

  m_characterList = std::make_unique<CharacterList>(root);

  const auto pagination = lv::hbox(root)
                              .width(lv_pct(100))
                              .height(56)
                              .padding(8)
                              .gap(12)
                              .center_content()
                              .add_style(m_styles.pagination().get());

  m_previousButton = lv::Button::create(pagination).text("Previous").on_click<&MainView::onPreviousPage>(this);
  m_previousButton.add_style(m_styles.secondaryButton().get())
      .add_style(m_styles.disabledButton().get(), LV_STATE_DISABLED);

  m_pageLabel = lv::Label::create(pagination).text("Loading characters...");
  m_pageLabel.add_style(m_styles.secondaryText().get());

  m_nextButton = lv::Button::create(pagination).text("Next").on_click<&MainView::onNextPage>(this);
  m_nextButton.add_style(m_styles.primaryButton().get())
      .add_style(m_styles.primaryButtonPressed().get(), LV_STATE_PRESSED)
      .add_style(m_styles.disabledButton().get(), LV_STATE_DISABLED);

  m_pollTimer = lv_timer_create(&MainView::onPollTimer, 30, this);
  if (m_pollTimer == nullptr) {
    LOG_ERROR("Failed to create LVGL task polling timer");
  }

  showLoading();
  m_viewModel.update();
}

void MainView::render(const MainViewUIData& uiData)
{
  if (uiData.hasError) {
    m_pageLabel.text("Unable to load characters");
    m_previousButton.enabled(false);
    m_nextButton.enabled(false);
    m_characterList->setCharacters({});
    return;
  }

  m_titleLabel.text(uiData.title);
  m_pageLabel.text_fmt("Page %llu / %llu", uiData.currentPage, uiData.totalPages);
  m_previousButton.enabled(uiData.hasPreviousPage);
  m_nextButton.enabled(uiData.hasNextPage);
  m_characterList->setCharacters(uiData.characters);
}

void MainView::showLoading()
{
  m_pageLabel.text("Loading characters...");
  m_previousButton.enabled(false);
  m_nextButton.enabled(false);
}

void MainView::onPollTimer(lv_timer_t* timer)
{
  auto* self = static_cast<MainView*>(lv_timer_get_user_data(timer));
  if (self == nullptr) {
    return;
  }

  if (auto result = self->m_viewModel.poll()) {
    self->render(*result);
    LOG_INFO("page={}/{}", result->currentPage, result->totalPages);
  }
}

void MainView::onClick(lv::Event)
{
  m_viewModel.update();
  showLoading();
}

void MainView::onNextPage(lv::Event)
{
  m_viewModel.nextPage();
  showLoading();
}

void MainView::onPreviousPage(lv::Event)
{
  m_viewModel.previousPage();
  showLoading();
}
