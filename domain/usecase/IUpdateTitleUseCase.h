#pragma once

#include <string>

namespace domain
{
    class IUpdateTitleUseCase
    {
    public:
        virtual ~IUpdateTitleUseCase() = default;

        virtual std::string execute() = 0;
    };
}
