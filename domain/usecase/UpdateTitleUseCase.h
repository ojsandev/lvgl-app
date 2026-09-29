#pragma once

#include <string>

#include "IUpdateTitleUseCase.h"

namespace domain {
class UpdateTitleUseCase : public IUpdateTitleUseCase {
  public:
    UpdateTitleUseCase() = default;
    ~UpdateTitleUseCase() override = default;

    std::string execute() override;
};
} // namespace domain
