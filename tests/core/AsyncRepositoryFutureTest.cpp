#include <filesystem>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "common/async/Task.h"
#include "data/datasource/DemonSlayerDataSource.h"
#include "data/http/IAsyncHttpClient.h"
#include "data/http/IHttpClient.h"
#include "data/repository/CharacterRepository.h"
#include "data/repository/image/CharacterImageRepository.h"
#include "domain/model/CharactersResponse.h"
#include "domain/model/IOResult.h"

namespace {

class StubHttpClient final : public data::http::IHttpClient {
public:
  std::string get(std::string_view) override
  {
    return {};
  }
  std::string post(std::string_view, std::string_view, std::string_view) override
  {
    return {};
  }
  std::string put(std::string_view, std::string_view, std::string_view) override
  {
    return {};
  }
  std::string patch(std::string_view, std::string_view, std::string_view) override
  {
    return {};
  }
  std::string delete_(std::string_view) override
  {
    return {};
  }
  std::vector<common::types::UInt8> getBytes(std::string_view) override
  {
    return {};
  }
};

class DeferredAsyncHttpClient final : public data::http::IAsyncHttpClient {
public:
  void getAsync(std::string_view, StringCompletion completion) override
  {
    stringCompletion = std::move(completion);
  }

  void postAsync(std::string_view, std::string_view, StringCompletion completion, std::string_view) override
  {
    stringCompletion = std::move(completion);
  }

  void putAsync(std::string_view, std::string_view, StringCompletion completion, std::string_view) override
  {
    stringCompletion = std::move(completion);
  }

  void patchAsync(std::string_view, std::string_view, StringCompletion completion, std::string_view) override
  {
    stringCompletion = std::move(completion);
  }

  void deleteAsync(std::string_view, StringCompletion completion) override
  {
    stringCompletion = std::move(completion);
  }

  void getBytesAsync(std::string_view, BytesCompletion completion) override
  {
    bytesCompletion = std::move(completion);
  }

  StringCompletion stringCompletion;
  BytesCompletion bytesCompletion;
};

common::async::Task<int> awaitAndIncrement(common::async::Task<int> task)
{
  const auto value = co_await task;
  co_return value + 1;
}

bool taskCoroutineResumesWhenDependencyCompletes()
{
  common::async::TaskCompletionSource<int> completion;
  auto chainedTask = awaitAndIncrement(completion.task());

  if (chainedTask.isReady()) {
    return false;
  }

  completion.set_value(41);
  return chainedTask.isReady() && chainedTask.result() == 42;
}

bool taskSupportsMultipleAwaiters()
{
  common::async::TaskCompletionSource<int> completion;
  auto sharedTask = completion.task();
  auto first = awaitAndIncrement(sharedTask);
  auto second = awaitAndIncrement(sharedTask);

  completion.set_value(9);

  return first.isReady() && second.isReady() && first.result() == 10 && second.result() == 10;
}

bool characterRepositoryTaskStaysPendingUntilCompletion()
{
  auto httpClient = std::make_shared<StubHttpClient>();
  auto asyncHttpClient = std::make_shared<DeferredAsyncHttpClient>();
  auto dataSource = std::make_shared<data::datasource::DemonSlayerDataSource>(httpClient, asyncHttpClient);
  data::repository::CharacterRepository repository(dataSource);

  auto task = repository.getCharactersPaginated(20, 1);
  if (task.isReady() || !asyncHttpClient->stringCompletion) {
    return false;
  }

  asyncHttpClient->stringCompletion(data::http::AsyncHttpStringResult::success(
      R"({"content":[{"id":1,"name":"Tanjiro","age":16,"gender":"Male","race":"Human","description":"Demon Slayer","img":"https://example.com/tanjiro.webp"}],"pagination":{"totalElements":1,"elementsOnPage":1,"currentPage":1,"totalPages":1}})"));

  if (!task.isReady() || task.hasError()) {
    return false;
  }

  const auto& result = task.result();
  if (!std::holds_alternative<domain::model::CharactersResponse>(result)) {
    return false;
  }

  const auto& response = std::get<domain::model::CharactersResponse>(result);
  return response.characters.size() == 1 && response.characters.front().info.id == 1;
}

bool imageRepositoryTaskStaysPendingUntilCompletion()
{
  auto httpClient = std::make_shared<StubHttpClient>();
  auto asyncHttpClient = std::make_shared<DeferredAsyncHttpClient>();
  auto dataSource = std::make_shared<data::datasource::DemonSlayerDataSource>(httpClient, asyncHttpClient);
  data::repository::CharacterImageRepository repository(dataSource);

  const auto imagePath = std::filesystem::temp_directory_path() / "character-image-repository-test.webp";
  std::filesystem::remove(imagePath);

  auto task = repository.storeImage(42, imagePath.string());
  if (task.isReady() || !asyncHttpClient->bytesCompletion) {
    std::filesystem::remove(imagePath);
    return false;
  }

  asyncHttpClient->bytesCompletion(data::http::AsyncHttpBytesResult::success({ 0x52, 0x49, 0x46, 0x46 }));

  const auto fileExists = std::filesystem::exists(imagePath);
  std::filesystem::remove(imagePath);

  if (!task.isReady() || task.hasError() || !fileExists) {
    return false;
  }

  const auto& result = task.result();
  if (!std::holds_alternative<domain::model::IOResult>(result)) {
    return false;
  }

  return std::get<domain::model::IOResult>(result).type == domain::model::IOResultType::Success;
}

bool cancelledCharacterRepositoryTaskIgnoresLateCompletion()
{
  auto httpClient = std::make_shared<StubHttpClient>();
  auto asyncHttpClient = std::make_shared<DeferredAsyncHttpClient>();
  auto dataSource = std::make_shared<data::datasource::DemonSlayerDataSource>(httpClient, asyncHttpClient);
  data::repository::CharacterRepository repository(dataSource);

  auto task = repository.getCharactersPaginated(20, 1);
  if (!asyncHttpClient->stringCompletion || !task.requestCancel() || !task.isReady() || !task.isCanceled()) {
    return false;
  }

  asyncHttpClient->stringCompletion(data::http::AsyncHttpStringResult::success(
      R"({"content":[{"id":1,"name":"Tanjiro","age":16,"gender":"Male","race":"Human","description":"Demon Slayer","img":"https://example.com/tanjiro.webp"}],"pagination":{"totalElements":1,"elementsOnPage":1,"currentPage":1,"totalPages":1}})"));

  try {
    static_cast<void>(task.result());
    return false;
  } catch (const common::async::TaskCanceledException&) {
    return true;
  } catch (...) {
    return false;
  }
}

bool cancelledImageRepositoryTaskAvoidsWritingFile()
{
  auto httpClient = std::make_shared<StubHttpClient>();
  auto asyncHttpClient = std::make_shared<DeferredAsyncHttpClient>();
  auto dataSource = std::make_shared<data::datasource::DemonSlayerDataSource>(httpClient, asyncHttpClient);
  data::repository::CharacterImageRepository repository(dataSource);

  const auto imagePath = std::filesystem::temp_directory_path() / "character-image-repository-cancelled-test.webp";
  std::filesystem::remove(imagePath);

  auto task = repository.storeImage(42, imagePath.string());
  if (!asyncHttpClient->bytesCompletion || !task.requestCancel() || !task.isReady() || !task.isCanceled()) {
    std::filesystem::remove(imagePath);
    return false;
  }

  asyncHttpClient->bytesCompletion(data::http::AsyncHttpBytesResult::success({ 0x52, 0x49, 0x46, 0x46 }));

  const auto fileExists = std::filesystem::exists(imagePath);
  std::filesystem::remove(imagePath);

  if (fileExists) {
    return false;
  }

  try {
    static_cast<void>(task.result());
    return false;
  } catch (const common::async::TaskCanceledException&) {
    return true;
  } catch (...) {
    return false;
  }
}

} // namespace

int main()
{
  if (!taskCoroutineResumesWhenDependencyCompletes()) {
    return 1;
  }
  if (!taskSupportsMultipleAwaiters()) {
    return 1;
  }
  if (!characterRepositoryTaskStaysPendingUntilCompletion()) {
    return 1;
  }
  if (!imageRepositoryTaskStaysPendingUntilCompletion()) {
    return 1;
  }
  if (!cancelledCharacterRepositoryTaskIgnoresLateCompletion()) {
    return 1;
  }
  if (!cancelledImageRepositoryTaskAvoidsWritingFile()) {
    return 1;
  }
  return 0;
}
