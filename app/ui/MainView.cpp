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

    m_titleLabel = lv::Label::create(root).on_hover_leave<&MainView::onHoverLabel>(this)
        .text(m_viewModel.title());

    lv::Button::create(root)
        .text("Click")
        .on_hover_over<&MainView::onHover>(this)
        .on_click<&MainView::onClick>(this);

    std::print("MainView::init: widgets created\n");
}

void MainView::onClick(lv::Event)
{
    m_viewModel.handleTitleUpdate();
    m_titleLabel.text(m_viewModel.title());
    std::print(""
               "MainView::onClick: title updated to: {}\n",
               m_viewModel.title());
}

void MainView::onTitleChanged(const lv::Event& event)
{
    std::print("MainView::onTitleChanged: Title Changed Code: {}\n",
               static_cast<int>(event.code()));
}

void MainView::onHover(const lv::Event& event)
{
    std::print("MainView::onHover: Hover Code: {}\n", static_cast<int>(event.code()));
}

void MainView::onHoverLabel(const lv::Event& event)
{
    std::print("MainView::onHoverLabel: Hover Label Code: {}\n", static_cast<int>(event.code()));
}
