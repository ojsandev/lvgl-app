#pragma once

#include <memory>
#include <string>

#include "IUpdateTitleUseCase.h"

namespace common::uuid {
class IUUID;
}

namespace domain::usecase {
class UpdateTitleUseCase final : public IUpdateTitleUseCase {
public:
  UpdateTitleUseCase() = default;
  ~UpdateTitleUseCase() override = default;

  std::string execute() override;

private:
  std::shared_ptr<common::uuid::IUUID> m_uuid;
};
} // namespace domain::usecase
