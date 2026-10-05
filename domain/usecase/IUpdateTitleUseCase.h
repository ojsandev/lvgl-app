#pragma once

#include <string>

namespace domain::usecase {
class IUpdateTitleUseCase {
public:
  virtual ~IUpdateTitleUseCase() = default;

  virtual std::string execute() = 0;
};
} // namespace domain::usecase
