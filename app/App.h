#pragma once

#include <SDL_events.h>
#include <memory>

#include "lv/core/app.hpp"

namespace app::ui {
class MainView;
class MainViewModel;
} // namespace app::ui

namespace lv {
class App {
public:
  struct Config
  {
    int width;
    int height;
  };

  App(Config config, app::ui::MainViewModel& viewModel);

  ~App();

  App(const App&) = delete;
  App& operator=(const App&) = delete;

  App(App&&) = delete;
  App& operator=(App&&) = delete;

  void run();
  void stop();

private:
  static void processEvents();

  static int SDLCALL onWindowEvent(void* userdata, SDL_Event* event);

private:
  InitGuard m_initGuard;

  Config m_config;

  bool m_isRunning;

  app::ui::MainViewModel& m_viewModel;

  std::unique_ptr<app::ui::MainView> m_view;
};
} // namespace lv
