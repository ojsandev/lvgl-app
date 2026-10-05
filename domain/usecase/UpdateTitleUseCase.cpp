#include "domain/usecase/UpdateTitleUseCase.h"

#include <print>

#include "common/uuid.h"

using namespace domain::usecase;

std::string UpdateTitleUseCase::execute()
{
  const auto uuid = common::uuid::generate();

  std::print("UpdateTitleUseCase::execute: uuid={}\n", uuid);

  return "Title changed by use case: " + uuid;
}
