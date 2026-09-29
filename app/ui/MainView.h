#pragma once

#include <memory>

#include "lv/core/display.hpp"
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

    void onClick(lv::Event event);
    void onTitleChanged(const lv::Event& event);
    void onHover(const lv::Event& event);
    void onHoverLabel(const lv::Event& event);

    MainViewModel& m_viewModel;

    lv::SDLDisplay m_display;
    lv::ObjectView m_screen;

    lv::Label m_titleLabel;
};
} // namespace app::ui