#include "domain/usecase/UpdateTitleUseCase.h"

#include <print>

#include "common/uuid.h"
#include "common/logger/Logging.h"

using namespace domain::usecase;

std::string UpdateTitleUseCase::execute()
{
  const auto uuid = common::uuid::generate();

  LOG_INFO("UpdateTitleUseCase::execute: uuid: {}", uuid);

  return "Title changed by use case: " + uuid;
}
