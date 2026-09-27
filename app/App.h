#pragma once

#include <memory>

#include "lv/core/app.hpp"

namespace ui
{
    class MainView;
    class MainViewModel;
}

namespace lv
{
    class App
    {
    public:
        struct Config
        {
            int width;
            int height;
        };

        App(
            Config config,
            const std::shared_ptr<ui::MainViewModel>& viewModel
        );

        ~App();

        App(const App&) = delete;
        App& operator=(const App&) = delete;

        App(App&&) = delete;
        App& operator=(App&&) = delete;

        void run();
        void stop();

        [[nodiscard]]
        bool isRunning() const noexcept;

    private:
        void processEvents();

        InitGuard m_initGuard;

        Config m_config;

        bool m_isRunning{true};

        std::unique_ptr<ui::MainView> m_view;
    };
}