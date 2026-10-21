#pragma once

#include <gmock/gmock.h>
#include "domain/usecase/IUpdateTitleUseCase.h"

namespace tests::mocks {
class MockUpdateTitleUseCase : public domain::usecase::IUpdateTitleUseCase {
public:
  MOCK_METHOD(std::string, execute, (), (override));
};
}
