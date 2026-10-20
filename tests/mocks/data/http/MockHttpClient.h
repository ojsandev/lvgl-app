#pragma once

#include <gmock/gmock.h>

#include "common/types.h"
#include "http/IAsyncHttpClient.h"
#include "http/IHttpClient.h"

namespace tests::mocks {
class MockHttpClient : public data::http::IHttpClient
{
public:
  MOCK_METHOD(std::string, get, (std::string_view), (override));

  MOCK_METHOD(std::string, post, (std::string_view, std::string_view, std::string_view), (override));

  MOCK_METHOD(std::string, put, (std::string_view, std::string_view, std::string_view), (override));

  MOCK_METHOD(std::string, patch, (std::string_view, std::string_view, std::string_view), (override));

  MOCK_METHOD(std::string, delete_, (std::string_view), (override));

  MOCK_METHOD(std::vector<common::types::UInt8>, getBytes, (std::string_view), (override));
};
}
