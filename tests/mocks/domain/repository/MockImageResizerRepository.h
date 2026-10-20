#pragma once

#include <gmock/gmock.h>
#include <domain/repository/IImageResizerRepository.h>

namespace tests::mocks {
class MockImageResizerRepository : public domain::repository::IImageResizerRepository
{
public:
  MOCK_METHOD(bool, createThumbnail,
              (const std::string&, const std::string&, common::types::UInt32, common::types::UInt32), (override));
};
}
