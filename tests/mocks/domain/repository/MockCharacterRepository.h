#pragma once

#include "domain/repository/ICharacterRepository.h"

namespace tests::mocks {
class MockCharacterRepository : public domain::repository::ICharacterRepository
{
public:
  MOCK_METHOD(domain::repository::ICharacterRepository::GetCharactersResult, getCharactersPaginated,
              (common::types::Int32, common::types::Int32), (override));
};
}
