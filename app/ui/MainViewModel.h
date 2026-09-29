#pragma once

#include <memory>
#include <string>

namespace domain
{
    class IUpdateTitleUseCase;
}

namespace app::ui
{
    class MainViewModel
    {
    public:
        explicit MainViewModel(
        const std::shared_ptr<domain::IUpdateTitleUseCase>& updateTitleUseCase
        );

        void handleTitleUpdate();

        [[nodiscard]]
        const std::string& title() const noexcept;

    private:
        std::string m_title;
        std::shared_ptr<domain::IUpdateTitleUseCase> m_updateTitleUseCase;
    };
}
