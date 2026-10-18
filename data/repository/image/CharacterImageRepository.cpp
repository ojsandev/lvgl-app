#include "CharacterImageRepository.h"

#include <boost/beast/core/file.hpp>
#include <filesystem>
#include <fstream>
#include <future>
#include "common/logger/Logging.h"
#include "datasource/DemonSlayerDataSource.h"

using namespace data::repository;

CharacterImageRepository::CharacterImageRepository(
    const std::shared_ptr<datasource::DemonSlayerDataSource>& demonSlayerDataSource)
  : m_demonSlayerDataSource(demonSlayerDataSource)
{}

domain::repository::ICharacterImageRepository::StoreImageResult
CharacterImageRepository::storeImage(common::types::Int32 characterId, const std::string& imagePath)
{
  auto promise = std::make_shared<std::promise<std::variant<domain::model::IOResult, StoredImage>>>();
  auto future = promise->get_future();

  try {
    m_demonSlayerDataSource->getCharacterImage(characterId, [promise, imagePath](const http::AsyncHttpBytesResult& result) mutable {
      try {
        if (!result.hasValue()) {
          result.rethrowError();
        }

        const auto& imageData = result.value();

        LOG_INFO("Character image received: {} bytes", imageData.size());

        std::ofstream file(imagePath, std::ios::binary);

        file.write(reinterpret_cast<const std::ostream::char_type*>(imageData.data()), static_cast<std::streamsize>(imageData.size()));

        auto value = domain::model::IOResult {
          .type = domain::model::IOResultType::Success,
          .message = "Character image stored successfully"
        };

        promise->set_value(value);

      } catch (const std::exception& e) {
        LOG_ERROR("Error fetching character image: {}", e.what());
        promise->set_exception(std::current_exception());
      } catch (...) {
        promise->set_exception(std::current_exception());
      }
    });
  } catch (std::exception& e) {
    LOG_ERROR("Error occurred while storing character image for characterId: {}. {}", characterId, e.what());
    const auto ioResult = domain::model::IOResult {
      .type = domain::model::IOResultType::Failure,
      .message = "Failed to store character image"
    };
    promise->set_value(ioResult);
  }  catch (...) {
    promise->set_exception(std::current_exception());
  }

  return future;
}

domain::repository::ICharacterImageRepository::GetImageResult
CharacterImageRepository::getThumbnail(const common::types::Int32 characterId)
{
  try {
    const auto storedPath = getThumbnailPath(characterId);

    std::ifstream file(storedPath, std::ios::binary);

    if (!file) {
      return domain::model::IOResult{ .type = domain::model::IOResultType::Failure,
                                      .message = "Failed to open character thumbnail" };
    }

    return StoredImage(std::istreambuf_iterator(file), std::istreambuf_iterator<char>());
  } catch (const std::exception& e) {
    LOG_ERROR("Error occurred while retrieving character thumbnail for characterId: {}. {}", characterId, e.what());

    return domain::model::IOResult{ .type = domain::model::IOResultType::Failure,
                                    .message = "Failed to retrieve character thumbnail" };
  }
}

domain::repository::ICharacterImageRepository::GetImageResult
CharacterImageRepository::getImage(const common::types::Int32 characterId)
{
  try {
    const auto storedPath = getImagePath(characterId);

    std::ifstream file(storedPath, std::ios::binary);

    if (!file) {
      return domain::model::IOResult {
        .type = domain::model::IOResultType::Failure,
        .message = "Failed to open character image"
      };
    }

    return StoredImage(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
  } catch (const std::exception& e) {
    LOG_ERROR("Error occurred while retrieving character image for characterId: {}. {}", characterId, e.what());

    return domain::model::IOResult{
      .type = domain::model::IOResultType::Failure,
      .message = "Failed to retrieve character image"
    };
  }
}

bool CharacterImageRepository::imageExists(const common::types::Int32 characterId)
{
  return std::filesystem::exists(getImagePath(characterId));
}

bool CharacterImageRepository::thumbnailExists(const common::types::Int32 characterId)
{
  return std::filesystem::exists(getThumbnailPath(characterId));
}

std::string CharacterImageRepository::getImagePath(const common::types::Int32 characterId)
{
  return "/tmp/character-" + std::to_string(characterId) + ".webp";
}

std::string CharacterImageRepository::getThumbnailPath(const common::types::Int32 characterId)
{
  return "/tmp/character-" + std::to_string(characterId) + "-thumb.webp";
}
