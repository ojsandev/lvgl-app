#include "App.h"

#include "common/logger/Logging.h"
#include "ui/MainView.h"
#include "ui/MainViewModel.h"

using namespace lv;

App::App(const Config config, app::ui::MainViewModel& viewModel)
  : m_config(config)
    , m_isRunning(false)
    , m_viewModel(viewModel)
    , m_display(config.width, config.height)
{
  m_display.resizable(true);
  m_display.minimumSize(config.width, config.height);

  m_view = std::make_unique<app::ui::MainView>(m_display, m_viewModel);

  m_display.raise();
  m_display.addEventWatch(&App::onWindowEvent, this);
}

App::~App()
{
  m_display.removeEventWatch(&App::onWindowEvent, this);
}

void App::run()
{
  m_isRunning = true;
  LOG_INFO("App started");

  while (m_isRunning) {
    processEvents();
  }
}

void App::stop()
{
  if (!m_isRunning) {
    return;
  }

  m_isRunning = false;
  LOG_INFO("App stopped");
}

void App::processEvents()
{
  const auto delay = tick();
  sleep_ms(delay);
}

int SDLCALL App::onWindowEvent(void* userdata, SDL_Event* event)
{
  auto* app = static_cast<App*>(userdata);

  if (app == nullptr || event == nullptr) {
    return 0;
  }

  if (event->type == SDL_QUIT) {
    app->stop();
    return 0;
  }

  if (event->type == SDL_MOUSEWHEEL) {
    event->wheel.y = -event->wheel.y;
  }

  if (event->type == SDL_WINDOWEVENT) {
    switch (event->window.event) {
    case SDL_WINDOWEVENT_CLOSE:
      app->stop();
      break;

    case SDL_WINDOWEVENT_MOVED:
      LOG_DEBUG("Window moved: {}, {}", event->window.data1, event->window.data2);
      break;

    case SDL_WINDOWEVENT_HIDDEN:
      LOG_DEBUG("Window hidden");
      break;

    case SDL_WINDOWEVENT_SHOWN:
      LOG_DEBUG("Window shown");
      break;

    case SDL_WINDOWEVENT_FOCUS_GAINED:
      LOG_DEBUG("Window focus gained");
      break;

    case SDL_WINDOWEVENT_FOCUS_LOST:
      LOG_DEBUG("Window focus lost");
      break;

    default:
      break;
    }
  }

  return 0;
}
