#pragma once

#include "IUpdateTitleUseCase.h"
#include <string>

namespace domain
{
    class UpdateTitleUseCase : public IUpdateTitleUseCase
    {
    public:
        UpdateTitleUseCase() = default;
        ~UpdateTitleUseCase() override = default;

        std::string execute() override;
    };
}
