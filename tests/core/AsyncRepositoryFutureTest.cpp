#include <chrono>
#include <filesystem>
#include <future>
#include <memory>
#include <string>
#include <variant>
#include <vector>

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
  std::string get(std::string_view) override { return {}; }
  std::string post(std::string_view, std::string_view, std::string_view) override { return {}; }
  std::string put(std::string_view, std::string_view, std::string_view) override { return {}; }
  std::string patch(std::string_view, std::string_view, std::string_view) override { return {}; }
  std::string delete_(std::string_view) override { return {}; }
  std::vector<common::types::UInt8> getBytes(std::string_view) override { return {}; }
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

constexpr auto kImmediate = std::chrono::milliseconds(0);
constexpr auto kShortWait = std::chrono::seconds(1);

bool characterRepositoryFutureStaysPendingUntilCompletion()
{
  auto httpClient = std::make_shared<StubHttpClient>();
  auto asyncHttpClient = std::make_shared<DeferredAsyncHttpClient>();
  auto dataSource = std::make_shared<data::datasource::DemonSlayerDataSource>(httpClient, asyncHttpClient);
  data::repository::CharacterRepository repository(dataSource);

  auto future = repository.getCharactersPaginated(20, 1);

  if (future.wait_for(kImmediate) != std::future_status::timeout) {
    return false;
  }

  if (!asyncHttpClient->stringCompletion) {
    return false;
  }

  asyncHttpClient->stringCompletion(data::http::AsyncHttpStringResult::success(
      R"({"content":[{"id":1,"name":"Tanjiro","age":16,"gender":"Male","race":"Human","description":"Demon Slayer","img":"https://example.com/tanjiro.webp"}],"pagination":{"totalElements":1,"elementsOnPage":1,"currentPage":1,"totalPages":1}})"));

  if (future.wait_for(kShortWait) != std::future_status::ready) {
    return false;
  }

  auto result = future.get();
  if (!std::holds_alternative<domain::model::CharactersResponse>(result)) {
    return false;
  }

  const auto& response = std::get<domain::model::CharactersResponse>(result);
  return response.characters.size() == 1 && response.characters.front().info.id == 1;
}

bool imageRepositoryFutureStaysPendingUntilCompletion()
{
  auto httpClient = std::make_shared<StubHttpClient>();
  auto asyncHttpClient = std::make_shared<DeferredAsyncHttpClient>();
  auto dataSource = std::make_shared<data::datasource::DemonSlayerDataSource>(httpClient, asyncHttpClient);
  data::repository::CharacterImageRepository repository(dataSource);

  const auto imagePath = std::filesystem::temp_directory_path() / "character-image-repository-test.webp";
  std::filesystem::remove(imagePath);

  auto future = repository.storeImage(42, imagePath.string());

  if (future.wait_for(kImmediate) != std::future_status::timeout) {
    std::filesystem::remove(imagePath);
    return false;
  }

  if (!asyncHttpClient->bytesCompletion) {
    std::filesystem::remove(imagePath);
    return false;
  }

  asyncHttpClient->bytesCompletion(data::http::AsyncHttpBytesResult::success({0x52, 0x49, 0x46, 0x46}));

  if (future.wait_for(kShortWait) != std::future_status::ready) {
    std::filesystem::remove(imagePath);
    return false;
  }

  auto result = future.get();
  const auto fileExists = std::filesystem::exists(imagePath);
  std::filesystem::remove(imagePath);

  if (!std::holds_alternative<domain::model::IOResult>(result)) {
    return false;
  }

  const auto& ioResult = std::get<domain::model::IOResult>(result);
  return fileExists && ioResult.type == domain::model::IOResultType::Success;
}

} // namespace

int main()
{
  if (!characterRepositoryFutureStaysPendingUntilCompletion()) {
    return 1;
  }

  if (!imageRepositoryFutureStaysPendingUntilCompletion()) {
    return 1;
  }

  return 0;
}

