#include "CharacterImageRepository.h"

#include <exception>
#include <filesystem>
#include <fstream>
#include <iterator>

#include "common/async/Task.h"
#include "common/logger/Logging.h"
#include "data/datasource/DemonSlayerDataSource.h"

using namespace data::repository;

CharacterImageRepository::CharacterImageRepository(
    const std::shared_ptr<datasource::DemonSlayerDataSource>& demonSlayerDataSource)
  : m_demonSlayerDataSource(demonSlayerDataSource) {}

domain::repository::ICharacterImageRepository::StoreImageResult CharacterImageRepository::storeImage(
    const common::types::Int32 characterId, const std::string& imagePath)
{
  common::async::TaskCompletionSource<StoreImageValue> completion;
  auto task = completion.task();

  try {
    m_demonSlayerDataSource->getCharacterImage(
        characterId, [completion, imagePath](const http::AsyncHttpBytesResult& result) mutable {
          try {
            if (!result.hasValue()) {
              result.rethrowError();
            }

            const auto& imageData = result.value();

            std::ofstream file(imagePath, std::ios::binary | std::ios::trunc);

            if (!file) {
              completion.set_value(domain::model::IOResult{
                .type = domain::model::IOResultType::Failure,
                .message = "Failed to open image output file"
              });
              return;
            }

            file.write(reinterpret_cast<const char*>(imageData.data()), static_cast<std::streamsize>(imageData.size()));

            file.flush();

            if (!file) {
              file.close();

              completion.set_value(domain::model::IOResult{
                .type = domain::model::IOResultType::Failure,
                .message = "Failed to write image output file"
              });
              return;
            }

            file.close();

            if (file.fail()) {
              completion.set_value(domain::model::IOResult{
                .type = domain::model::IOResultType::Failure,
                .message = "Failed to close image output file"
              });
              return;
            }

            LOG_INFO("Character image fully written and closed: path={}, bytes={}", imagePath, imageData.size());

            completion.set_value(domain::model::IOResult{
              .type = domain::model::IOResultType::Success,
              .message = "Character image stored successfully"
            });
          } catch (...) {
            completion.set_exception(std::current_exception());
          }
        });
  } catch (...) {
    LOG_ERROR("Failed to start image request for characterId {}", characterId);
    completion.set_exception(std::current_exception());
  }

  return task;
}

domain::repository::ICharacterImageRepository::GetImageResult
CharacterImageRepository::getThumbnail(const common::types::Int32 characterId)
{
  try {
    const auto storedPath = getThumbnailPath(characterId);
    std::ifstream file(storedPath, std::ios::binary);
    if (!file) {
      return domain::model::IOResult{
        .type = domain::model::IOResultType::Failure,
        .message = "Failed to open character thumbnail"
      };
    }
    return StoredImage(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
  } catch (const std::exception& e) {
    LOG_ERROR("Error retrieving thumbnail for characterId {}: {}", characterId, e.what());
    return domain::model::IOResult{
      .type = domain::model::IOResultType::Failure,
      .message = "Failed to retrieve character thumbnail"
    };
  }
}

domain::repository::ICharacterImageRepository::GetImageResult
CharacterImageRepository::getImage(const common::types::Int32 characterId)
{
  try {
    const auto storedPath = getImagePath(characterId);
    std::ifstream file(storedPath, std::ios::binary);
    if (!file) {
      return domain::model::IOResult{
        .type = domain::model::IOResultType::Failure,
        .message = "Failed to open character image"
      };
    }
    return StoredImage(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
  } catch (const std::exception& e) {
    LOG_ERROR("Error retrieving image for characterId {}: {}", characterId, e.what());
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
