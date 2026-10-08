#include "MainView.h"

#include "MainViewModel.h"
#include "common/logger/Logging.h"
#include "lv/layout/flex.hpp"
#include "theme/Colors.h"

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
  const auto root =
      lv::vbox(m_screen)
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

  m_previousButton = lv::Button::create(pagination)
                     .text("Previous")
                     .on_click<&MainView::onPreviousPage>(this);
  m_previousButton.add_style(m_styles.secondaryButton().get()).add_style(
      m_styles.disabledButton().get(), LV_STATE_DISABLED);

  m_pageLabel = lv::Label::create(pagination).text("Page 0 / 0");
  m_pageLabel.add_style(m_styles.secondaryText().get());

  m_nextButton = lv::Button::create(pagination).text("Next").on_click<&MainView::onNextPage>(this);
  m_nextButton.add_style(m_styles.primaryButton().get())
              .add_style(m_styles.primaryButtonPressed().get(), LV_STATE_PRESSED)
              .add_style(m_styles.disabledButton().get(), LV_STATE_DISABLED);

  const auto uiData = m_viewModel.update();

  render(uiData);
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

  m_pageLabel.text_fmt("Page %llu / %llu", uiData.currentPage, uiData.totalPages);

  m_previousButton.enabled(uiData.hasPreviousPage);
  m_nextButton.enabled(uiData.hasNextPage);
  m_characterList->setCharacters(uiData.characters);
}

void MainView::onClick(lv::Event)
{
  const auto uiData = m_viewModel.update();

  render(uiData);

  LOG_INFO("page={}/{}", uiData.currentPage, uiData.totalPages);
}

void MainView::onNextPage(lv::Event)
{
  const auto uiData = m_viewModel.nextPage();

  render(uiData);

  LOG_INFO("page={}/{}", uiData.currentPage, uiData.totalPages);
}

void MainView::onPreviousPage(lv::Event)
{
  const auto uiData = m_viewModel.previousPage();

  render(uiData);

  LOG_INFO("page={}/{}", uiData.currentPage, uiData.totalPages);
}
