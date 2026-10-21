#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <string_view>
#include <variant>

#include "common/async/Task.h"
#include "data/datasource/DemonSlayerDataSource.h"
#include "data/http/IAsyncHttpClient.h"
#include "data/repository/CharacterRepository.h"
#include "data/repository/image/CharacterImageRepository.h"
#include "domain/model/CharactersResponse.h"
#include "domain/model/IOResult.h"
#include "data/http/MockAsyncHttpClient.h"
#include "data/http/MockHttpClient.h"

namespace {

using ::testing::_;
using ::testing::NiceMock;
using ::testing::SaveArg;

using AsyncHttpClient = data::http::IAsyncHttpClient;
using StringCompletion = AsyncHttpClient::StringCompletion;
using BytesCompletion = AsyncHttpClient::BytesCompletion;

constexpr std::string_view characterResponseJson = R"({
    "content": [{
        "id": 1,
        "name": "Tanjiro",
        "age": 16,
        "gender": "Male",
        "race": "Human",
        "description": "Demon Slayer",
        "img": "https://example.com/tanjiro.webp"
    }],
    "pagination": {
        "totalElements": 1,
        "elementsOnPage": 1,
        "currentPage": 1,
        "totalPages": 1
    }
})";

common::async::Task<int> awaitAndIncrement(common::async::Task<int> task)
{
  const auto value = co_await task;
  co_return value + 1;
}

struct HttpDependencies
{
  std::shared_ptr<NiceMock<tests::mocks::MockHttpClient>> httpClient =
      std::make_shared<NiceMock<tests::mocks::MockHttpClient>>();

  std::shared_ptr<NiceMock<tests::mocks::MockAsyncHttpClient>> asyncHttpClient = std::make_shared<NiceMock<
    tests::mocks::MockAsyncHttpClient>>();

  std::shared_ptr<data::datasource::DemonSlayerDataSource> dataSource = std::make_shared<
    data::datasource::DemonSlayerDataSource>(httpClient, asyncHttpClient);
};

} // namespace

TEST(TaskTest, CoroutineResumesWhenDependencyCompletes)
{
  const common::async::TaskCompletionSource<int> completion;
  const auto chainedTask = awaitAndIncrement(completion.task());

  ASSERT_FALSE(chainedTask.isReady());

  completion.set_value(41);

  ASSERT_TRUE(chainedTask.isReady());
  EXPECT_EQ(chainedTask.result(), 42);
}

TEST(TaskTest, SupportsMultipleAwaiters)
{
  const common::async::TaskCompletionSource<int> completion;
  const auto sharedTask = completion.task();

  const auto first = awaitAndIncrement(sharedTask);
  const auto second = awaitAndIncrement(sharedTask);

  completion.set_value(9);

  ASSERT_TRUE(first.isReady());
  ASSERT_TRUE(second.isReady());
  EXPECT_EQ(first.result(), 10);
  EXPECT_EQ(second.result(), 10);
}

TEST(CharacterRepositoryTest, TaskStaysPendingUntilCompletion)
{
  const HttpDependencies dependencies;
  data::repository::CharacterRepository repository(dependencies.dataSource);

  StringCompletion completion;

  EXPECT_CALL(*dependencies.asyncHttpClient, getAsync(_, _))
        .WillOnce(SaveArg<1>(&completion));

  const auto task = repository.getCharactersPaginated(20, 1);

  ASSERT_FALSE(task.isReady());
  ASSERT_TRUE(static_cast<bool>(completion));

  completion(data::http::AsyncHttpStringResult::success(std::string(characterResponseJson)));

  ASSERT_TRUE(task.isReady());
  ASSERT_FALSE(task.hasError());

  const auto& result = task.result();

  ASSERT_TRUE(std::holds_alternative<domain::model::CharactersResponse>(result));

  const auto& response = std::get<domain::model::CharactersResponse>(result);

  ASSERT_EQ(response.characters.size(), 1U);
  EXPECT_EQ(response.characters.front().info.id, 1);
}

TEST(CharacterImageRepositoryTest, TaskStaysPendingUntilCompletion)
{
  HttpDependencies dependencies;
  data::repository::CharacterImageRepository repository(
      dependencies.dataSource);

  const auto imagePath =
      std::filesystem::temp_directory_path() /
      "character-image-repository-test.webp";

  std::filesystem::remove(imagePath);

  BytesCompletion completion;

  EXPECT_CALL(*dependencies.asyncHttpClient, getBytesAsync(_, _))
        .WillOnce(SaveArg<1>(&completion));

  auto task = repository.storeImage(42, imagePath.string());

  ASSERT_FALSE(task.isReady());
  ASSERT_TRUE(static_cast<bool>(completion));

  completion(data::http::AsyncHttpBytesResult::success({0x52, 0x49, 0x46, 0x46}));

  const bool fileExists = std::filesystem::exists(imagePath);
  std::filesystem::remove(imagePath);

  ASSERT_TRUE(task.isReady());
  ASSERT_FALSE(task.hasError());
  ASSERT_TRUE(fileExists);

  const auto& result = task.result();

  ASSERT_TRUE(std::holds_alternative<domain::model::IOResult>(result));

  EXPECT_EQ(std::get<domain::model::IOResult>(result).type, domain::model::IOResultType::Success);
}

TEST(CharacterRepositoryTest, CancelledTaskIgnoresLateCompletion)
{
  HttpDependencies dependencies;
  data::repository::CharacterRepository repository(dependencies.dataSource);

  StringCompletion completion;

  EXPECT_CALL(*dependencies.asyncHttpClient, getAsync(_, _))
        .WillOnce(SaveArg<1>(&completion));

  auto task = repository.getCharactersPaginated(20, 1);

  ASSERT_TRUE(static_cast<bool>(completion));
  ASSERT_TRUE(task.requestCancel());
  ASSERT_TRUE(task.isReady());
  ASSERT_TRUE(task.isCanceled());

  completion(data::http::AsyncHttpStringResult::success(std::string(characterResponseJson)));

  EXPECT_THROW(static_cast<void>(task.result()), common::async::TaskCanceledException);
}

TEST(CharacterImageRepositoryTest, CancelledTaskAvoidsWritingFile)
{
  const HttpDependencies dependencies;
  data::repository::CharacterImageRepository repository(dependencies.dataSource);

  const auto imagePath =
      std::filesystem::temp_directory_path() /
      "character-image-repository-cancelled-test.webp";

  std::filesystem::remove(imagePath);

  BytesCompletion completion;

  EXPECT_CALL(*dependencies.asyncHttpClient, getBytesAsync(_, _))
        .WillOnce(SaveArg<1>(&completion));

  auto task = repository.storeImage(42, imagePath.string());

  ASSERT_TRUE(static_cast<bool>(completion));
  ASSERT_TRUE(task.requestCancel());
  ASSERT_TRUE(task.isReady());
  ASSERT_TRUE(task.isCanceled());

  completion(data::http::AsyncHttpBytesResult::success({0x52, 0x49, 0x46, 0x46}));

  EXPECT_FALSE(std::filesystem::exists(imagePath));

  EXPECT_THROW(static_cast<void>(task.result()), common::async::TaskCanceledException);
}
