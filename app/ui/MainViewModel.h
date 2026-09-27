#pragma once

#include <memory>
#include <string>

namespace domain
{
    class IUpdateTitleUseCase;
}

namespace ui
{
    class MainViewModel
    {
    public:
        MainViewModel(
            std::string initialTitle,
            const std::shared_ptr<domain::IUpdateTitleUseCase>& updateTitleUseCase
        );

        void handleTitleUpdate();

        const std::string& title();

    private:
        std::string m_title;
        std::shared_ptr<domain::IUpdateTitleUseCase> m_updateTitleUseCase;
    };
}
