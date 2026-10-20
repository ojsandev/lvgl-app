#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "app/ui/MainViewModel.h"
#include "common/async/Task.h"
#include "domain/model/CharactersResponse.h"
#include "domain/model/IOResult.h"
#include "domain/repository/MockCharacterImageRepository.h"
#include "domain/repository/MockCharacterRepository.h"
#include "domain/usecase/characters/GetCharactersUseCase.h"
#include "domain/usecase/MockGetCharactersUseCase.h"
#include "domain/usecase/MockUpdateTitleUseCase.h"
#include "tests/mocks/domain/repository/MockImageResizerRepository.h"

namespace {

using ::testing::_;
using ::testing::Return;

domain::model::CharactersResponse makeCharactersResponse()
{
  return {
    .characters = {
      {
        .info = {
          .id = 1,
          .name = "Tanjiro",
          .age = std::nullopt,
          .gender = "Male",
          .race = "Human",
          .description = "One"
        },
        .img = "img-1"
      },
      {
        .info = {
          .id = 2,
          .name = "Nezuko",
          .age = std::nullopt,
          .gender = "Female",
          .race = "Demon",
          .description = "Two"
        },
        .img = "img-2"
      }
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
}

domain::model::CharacterView makeCharacterView()
{
  return {
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
  };
}

} // namespace

TEST(GetCharactersUseCaseTest, StartsCharacterImageWorkConcurrently)
{
  using CharacterRepositoryValue = domain::repository::ICharacterRepository::GetCharactersValue;
  using StoreImageValue = domain::repository::ICharacterImageRepository::StoreImageValue;

  auto characterRepository = std::make_shared<testing::StrictMock<tests::mocks::MockCharacterRepository>>();

  auto characterImageRepository = std::make_shared<testing::StrictMock<tests::mocks::MockCharacterImageRepository>>();

  auto imageResizerRepository = std::make_shared<testing::StrictMock<tests::mocks::MockImageResizerRepository>>();

  common::async::TaskCompletionSource<CharacterRepositoryValue> responseCompletion;
  common::async::TaskCompletionSource<StoreImageValue> firstImageCompletion;
  common::async::TaskCompletionSource<StoreImageValue> secondImageCompletion;

  EXPECT_CALL(*characterRepository, getCharactersPaginated(20, 1))
        .WillOnce(Return(responseCompletion.task()));

  for (const common::types::Int32 id : {1, 2}) {
    EXPECT_CALL(*characterImageRepository, imageExists(id))
            .WillOnce(Return(false));

    EXPECT_CALL(*characterImageRepository, thumbnailExists(id))
            .WillOnce(Return(false));

    EXPECT_CALL(*characterImageRepository, getImagePath(id))
            .WillOnce(Return(
                                                            (std::filesystem::temp_directory_path() / (
                                                               "character-" + std::to_string(id) + ".webp")).string()));

    EXPECT_CALL(*characterImageRepository, getThumbnailPath(id))
            .WillOnce(
                                                                    Return(
                                                                    (std::filesystem::temp_directory_path() / (
                                                                       "character-" + std::to_string(id) +
                                                                       "-thumb.webp")).string()));
  }

  EXPECT_CALL(*characterImageRepository, storeImage(1, _)).WillOnce(Return(firstImageCompletion.task()));
  EXPECT_CALL(*characterImageRepository, storeImage(2, _)).WillOnce(Return(secondImageCompletion.task()));
  EXPECT_CALL(*imageResizerRepository, createThumbnail(_, _, _, _))
        .Times(2)
        .WillRepeatedly(Return(true));

  domain::usecase::GetCharactersUseCase useCase(characterRepository, characterImageRepository, imageResizerRepository);

  const auto task = useCase.execute(20, 1);

  ASSERT_FALSE(task.isReady());

  responseCompletion.set_value(makeCharactersResponse());

  EXPECT_FALSE(task.isReady());

  firstImageCompletion.set_value(StoreImageValue{
    domain::model::IOResult{
      .type = domain::model::IOResultType::Success,
      .message = "ok"
    }
  });

  EXPECT_FALSE(task.isReady());

  secondImageCompletion.set_value(StoreImageValue{
    domain::model::IOResult{
      .type = domain::model::IOResultType::Success,
      .message = "ok"
    }
  });

  ASSERT_TRUE(task.isReady());
  ASSERT_FALSE(task.hasError());

  const auto& view = task.result();

  EXPECT_EQ(view.characters.size(), 2U);
  EXPECT_EQ(view.pagination.currentPage, 1);
}

TEST(MainViewModelTest, PollsPendingTaskAndCancelsOnDestruction)
{
  auto updateTitleUseCase = std::make_shared<testing::StrictMock<tests::mocks::MockUpdateTitleUseCase>>();
  auto getCharactersUseCase = std::make_shared<testing::StrictMock<tests::mocks::MockGetCharactersUseCase>>();

  common::async::TaskCompletionSource<domain::model::CharacterView> firstCompletion;
  common::async::TaskCompletionSource<domain::model::CharacterView> nextPageCompletion;

  const auto firstTask = firstCompletion.task();
  const auto nextPageTask = nextPageCompletion.task();

  EXPECT_CALL(*updateTitleUseCase, execute()).WillOnce(Return("Async title"));
  EXPECT_CALL(*getCharactersUseCase, execute(_, _)).WillOnce(Return(firstTask));
  EXPECT_CALL(*getCharactersUseCase, nextPage()).WillOnce(Return(nextPageTask));

  {
    app::ui::MainViewModel viewModel(updateTitleUseCase, getCharactersUseCase);

    viewModel.update();

    const auto loadingState = viewModel.poll();

    ASSERT_TRUE(loadingState.has_value());
    EXPECT_EQ(loadingState->state, app::ui::MainViewUIStateType::Loading);

    firstCompletion.set_value(makeCharacterView());

    const auto readyState = viewModel.poll();

    ASSERT_TRUE(readyState.has_value());
    EXPECT_EQ(readyState->state, app::ui::MainViewUIStateType::Data);
    EXPECT_EQ(readyState->title, "Async title");
    ASSERT_EQ(readyState->characters.size(), 1U);
    EXPECT_EQ(readyState->characters.front().info.id, 7);
    EXPECT_FALSE(viewModel.poll().has_value());

    viewModel.nextPage();

    EXPECT_TRUE(viewModel.isLoading());
  }

  EXPECT_TRUE(nextPageTask.isCanceled());
}
