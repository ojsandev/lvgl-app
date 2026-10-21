#pragma once

#include <gmock/gmock.h>

#include "common/types.h"
#include "domain/repository/ICharacterImageRepository.h"

namespace tests::mocks {
  class MockCharacterImageRepository : public domain::repository::ICharacterImageRepository
  {
  public:
    MOCK_METHOD(StoreImageResult, storeImage, (common::types::Int32 characterId, const std::string& imagePath), (override));
    MOCK_METHOD(GetImageResult, getImage, (common::types::Int32 characterId), (override));
    MOCK_METHOD(GetImageResult, getThumbnail, (common::types::Int32 characterId), (override));
    MOCK_METHOD(bool, imageExists, (common::types::Int32 characterId), (override));
    MOCK_METHOD(bool, thumbnailExists, (common::types::Int32 characterId), (override));
    MOCK_METHOD(std::string, getImagePath, (common::types::Int32 characterId), (override));
    MOCK_METHOD(std::string, getThumbnailPath, (common::types::Int32 characterId), (override));
  };
}
