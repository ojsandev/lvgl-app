#include "App.h"

#include <iostream>
#include <ostream>
#include <print>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "ui/MainView.h"
#include "ui/MainViewModel.h"

using namespace lv;

App::App(
    const Config config,
    const std::shared_ptr<ui::MainViewModel>& viewModel
)
    : m_config(config),
      m_view(
          std::make_unique<ui::MainView>(
              m_config.width,
              m_config.height,
              viewModel
          )
      )
{}

App::~App() = default;

void App::run()
{
    while (isRunning())
    {
        processEvents();

        if (!isRunning())
        {
            break;
        }

        const auto delayMs = tick();

        usleep(delayMs * 1000);
    }
}

void App::processEvents()
{
    SDL_PumpEvents();

    SDL_Event event{};

    if (SDL_PeepEvents(
        &event,
        1,
        SDL_PEEKEVENT,
        SDL_QUIT,
        SDL_QUIT
    ) > 0)
    {
        stop();
        return;
    }

    if (SDL_PeepEvents(
        &event,
        1,
        SDL_PEEKEVENT,
        SDL_WINDOWEVENT,
        SDL_WINDOWEVENT
    ) > 0)
    {
        if (event.window.event == SDL_WINDOWEVENT_MOVED)
        {
            std::print(
                "Window moved: {}, {}\n",
                event.window.data1,
                event.window.data2
            );
        }
    }
}

void App::stop()
{
    m_isRunning = false;
}

bool App::isRunning() const noexcept
{
    return m_isRunning;
}
