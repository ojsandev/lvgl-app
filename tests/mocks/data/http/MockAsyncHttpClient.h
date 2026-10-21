#pragma once

#include <gmock/gmock.h>

#include "http/IAsyncHttpClient.h"

namespace tests::mocks {
class MockAsyncHttpClient : public data::http::IAsyncHttpClient
{
public:
  MOCK_METHOD(void, getAsync, (std::string_view, StringCompletion), (override));

  MOCK_METHOD(void, postAsync, (std::string_view, std::string_view, StringCompletion, std::string_view), (override));

  MOCK_METHOD(void, putAsync, (std::string_view, std::string_view, StringCompletion, std::string_view), (override));

  MOCK_METHOD(void, patchAsync, (std::string_view, std::string_view, StringCompletion, std::string_view), (override));

  MOCK_METHOD(void, deleteAsync, (std::string_view, StringCompletion), (override));

  MOCK_METHOD(void, getBytesAsync, (std::string_view, BytesCompletion), (override));
};
}
