#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "app/ui/MainViewModel.h"
#include "common/async/Task.h"
#include "domain/model/CharactersResponse.h"
#include "domain/model/IOResult.h"
#include "domain/repository/ICharacterImageRepository.h"
#include "domain/repository/ICharacterRepository.h"
#include "domain/repository/IImageResizerRepository.h"
#include "domain/usecase/IUpdateTitleUseCase.h"
#include "domain/usecase/characters/GetCharactersUseCase.h"

namespace {

class ImmediateCharacterRepository final : public domain::repository::ICharacterRepository
{
public:
  explicit ImmediateCharacterRepository(domain::model::CharactersResponse response)
    : m_response(std::move(response)) {}

  GetCharactersResult getCharactersPaginated(common::types::Int32, common::types::Int32) override
  {
    common::async::TaskCompletionSource<GetCharactersValue> completion;
    auto task = completion.task();
    completion.set_value(m_response);
    return task;
  }

private:
  domain::model::CharactersResponse m_response;
};

class DeferredImageRepository final : public domain::repository::ICharacterImageRepository
{
public:
  StoreImageResult storeImage(common::types::Int32 characterId, const std::string& imagePath) override
  {
    requestedIds.push_back(characterId);
    requestedPaths.push_back(imagePath);

    common::async::TaskCompletionSource<domain::repository::ICharacterImageRepository::StoreImageValue> completion;
    auto task = completion.task();
    completions.push_back(completion);
    return task;
  }

  GetImageResult getImage(common::types::Int32) override
  {
    return domain::model::IOResult{.type = domain::model::IOResultType::Failure, .message = "unused"};
  }

  GetImageResult getThumbnail(common::types::Int32) override
  {
    return domain::model::IOResult{.type = domain::model::IOResultType::Failure, .message = "unused"};
  }

  bool imageExists(common::types::Int32) override
  {
    return false;
  }

  bool thumbnailExists(common::types::Int32) override
  {
    return false;
  }

  std::string getImagePath(common::types::Int32 characterId) override
  {
    return (std::filesystem::temp_directory_path() / ("character-" + std::to_string(characterId) + ".webp")).string();
  }

  std::string getThumbnailPath(common::types::Int32 characterId) override
  {
    return (std::filesystem::temp_directory_path() / ("character-" + std::to_string(characterId) + "-thumb.webp"))
        .string();
  }

  std::vector<common::types::Int32> requestedIds;
  std::vector<std::string> requestedPaths;
  std::vector<common::async::TaskCompletionSource<domain::repository::ICharacterImageRepository::StoreImageValue>>
  completions;
};

class RecordingImageResizer final : public domain::repository::IImageResizerRepository
{
public:
  bool createThumbnail(const std::string& inputPath, const std::string& outputPath, common::types::UInt32,
                       common::types::UInt32) override
  {
    createdThumbnails.emplace_back(inputPath, outputPath);
    return true;
  }

  std::vector<std::pair<std::string, std::string>> createdThumbnails;
};

class StubUpdateTitleUseCase final : public domain::usecase::IUpdateTitleUseCase
{
public:
  std::string execute() override
  {
    return "Async title";
  }
};

class DeferredGetCharactersUseCase final : public domain::usecase::IGetCharactersUseCase
{
public:
  Result execute(common::types::Int32, common::types::Int32) override
  {
    completion = common::async::TaskCompletionSource<domain::model::CharacterView>{};
    lastTask = completion.task();
    return lastTask;
  }

  Result nextPage() override
  {
    completion = common::async::TaskCompletionSource<domain::model::CharacterView>{};
    lastTask = completion.task();
    return lastTask;
  }

  Result previousPage() override
  {
    completion = common::async::TaskCompletionSource<domain::model::CharacterView>{};
    lastTask = completion.task();
    return lastTask;
  }

  common::async::TaskCompletionSource<domain::model::CharacterView> completion;
  Result lastTask = completion.task();
};

bool getCharactersUseCaseStartsCharacterWorkConcurrently()
{
  domain::model::CharactersResponse response = {
    .characters = {
      {
        .info = {
          .id = 1, .name = "Tanjiro", .age = std::nullopt, .gender = "Male", .race = "Human", .description = "One"
        },
        .img = "img-1"
      },
      {
        .info = {
          .id = 2, .name = "Nezuko", .age = std::nullopt, .gender = "Female", .race = "Demon", .description = "Two"
        },
        .img = "img-2"
      },
    },
    .pagination = {
      .totalElements = 2,
      .elementsOnPage = 2,
      .currentPage = 1,
      .totalPages = 3,
      .previousPage = "",
      .nextPageUrl = "page=2"
    }
  };

  auto characterRepository = std::make_shared<ImmediateCharacterRepository>(response);
  auto imageRepository = std::make_shared<DeferredImageRepository>();
  auto imageResizer = std::make_shared<RecordingImageResizer>();
  domain::usecase::GetCharactersUseCase useCase(characterRepository, imageRepository, imageResizer);

  auto task = useCase.execute(20, 1);

  if (task.isReady() || imageRepository->requestedIds.size() != 2 || imageRepository->completions.size() != 2) {
    return false;
  }

  imageRepository->completions[0].set_value(
      domain::model::IOResult{.type = domain::model::IOResultType::Success, .message = "ok"});
  if (task.isReady()) {
    return false;
  }

  imageRepository->completions[1].set_value(
      domain::model::IOResult{.type = domain::model::IOResultType::Success, .message = "ok"});

  if (!task.isReady() || task.hasError()) {
    return false;
  }

  const auto& view = task.result();
  return view.characters.size() == 2 && view.pagination.currentPage == 1 && imageResizer->createdThumbnails.size() == 2;
}

bool mainViewModelPollsPendingTaskAndCancelsOnDestruction()
{
  auto updateTitleUseCase = std::make_shared<StubUpdateTitleUseCase>();
  auto getCharactersUseCase = std::make_shared<DeferredGetCharactersUseCase>();

  {
    app::ui::MainViewModel viewModel(updateTitleUseCase, getCharactersUseCase);
    viewModel.update();

    const auto loadingState = viewModel.poll();
    if (!loadingState || loadingState->state != app::ui::MainViewUIStateType::Loading) {
      return false;
    }

    getCharactersUseCase->completion.set_value(
        domain::model::CharacterView{
          .characters = {
            {
              .info = {
                .id = 7,
                .name = "Zenitsu",
                .age = std::nullopt,
                .gender = "Male",
                .race = "Human",
                .description = "Thunder"
              },
              .imagePath = "img",
              .thumbnailPath = "thumb"
            }
          },
          .pagination = {
            .totalElements = 1,
            .elementsOnPage = 1,
            .currentPage = 1,
            .totalPages = 1,
            .previousPage = "",
            .nextPageUrl = ""
          }
        });

    const auto readyState = viewModel.poll();
    if (!readyState || readyState->state != app::ui::MainViewUIStateType::Data || readyState->title != "Async title" ||
        readyState->characters.size() != 1) {
      return false;
    }

    if (viewModel.poll().has_value()) {
      return false;
    }

    viewModel.nextPage();
    if (!viewModel.isLoading()) {
      return false;
    }
  }

  return getCharactersUseCase->lastTask.isCanceled();
}

} // namespace

int main()
{
  if (!getCharactersUseCaseStartsCharacterWorkConcurrently()) {
    return 1;
  }
  if (!mainViewModelPollsPendingTaskAndCancelsOnDestruction()) {
    return 1;
  }
  return 0;
}
