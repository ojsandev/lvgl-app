#include "App.h"

#include <SDL2/SDL.h>
#include <print>

#include "ui/MainView.h"
#include "ui/MainViewModel.h"

using namespace lv;

App::App(const Config config, app::ui::MainViewModel& viewModel)
    : m_config(config)
    , m_isRunning(false)
    , m_viewModel(viewModel)
    , m_isWindowClosed()
{
    m_view = std::make_unique<app::ui::MainView>(m_config.width, m_config.height, m_viewModel);

    SDL_AddEventWatch(&App::onWindowEvent, this);
}

App::~App()
{
    SDL_DelEventWatch(&App::onWindowEvent, this);
}

void App::run()
{
    std::print("App::run: starting main loop with width: {}, height: {}\n",
        m_config.width,
        m_config.height);

    m_isRunning = true;

    while (m_isRunning)
    {
        processEvents();
    }
}

void App::stop()
{
    if (!m_isRunning)
    {
        return;
    }

    m_isRunning = false;

    std::print("App::stop: stopping main loop\n");
}

bool App::isRunning() const noexcept
{
    return m_isRunning;
}

void App::processEvents()
{
    const auto delay = tick();
    sleep_ms(delay);
}

int SDLCALL App::onWindowEvent(void* userdata, SDL_Event* event)
{
    auto* app = static_cast<App*>(userdata);

    if (app == nullptr || event == nullptr)
    {
        return 0;
    }

    if (event->type == SDL_QUIT)
    {
        std::print("App::onWindowEvent: SDL_QUIT received\n");

        app->stop();

        return 0;
    }

    if (event->type == SDL_WINDOWEVENT)
    {
        switch (event->window.event)
        {
        case SDL_WINDOWEVENT_CLOSE:
            std::print("App::onWindowEvent: Window close requested\n");

            app->stop();
            break;

        case SDL_WINDOWEVENT_MOVED:
            std::print("App::onWindowEvent: Window moved: {}, {}\n",
                event->window.data1,
                event->window.data2);
            break;

        case SDL_WINDOWEVENT_HIDDEN:
            std::print("App::onWindowEvent: Window hidden\n");
            break;

        case SDL_WINDOWEVENT_SHOWN:
            std::print("App::onWindowEvent: Window shown\n");
            break;

        case SDL_WINDOWEVENT_FOCUS_GAINED:
            std::print("App::onWindowEvent: Window focus gained\n");
            break;

        case SDL_WINDOWEVENT_FOCUS_LOST:
            std::print("App::onWindowEvent: Window focus lost\n");
            break;

        default:
            break;
        }
    }

    return 0;
}
