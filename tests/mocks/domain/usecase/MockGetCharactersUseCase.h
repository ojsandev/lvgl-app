#pragma once

#include <gmock/gmock.h>

#include "domain/usecase/characters/IGetCharactersUseCase.h"

namespace tests::mocks {
class MockGetCharactersUseCase : public domain::usecase::IGetCharactersUseCase {
public:
  MOCK_METHOD(domain::usecase::IGetCharactersUseCase::Result, execute, (common::types::Int32, common::types::Int32), (override));
  MOCK_METHOD(domain::usecase::IGetCharactersUseCase::Result, nextPage, (), (override));
  MOCK_METHOD(domain::usecase::IGetCharactersUseCase::Result, previousPage, (), (override));
};
}

