#pragma once

#include <variant>
#include <vector>

#include "domain/model/IOResult.h"

namespace domain::repository {

class ICharacterImageRepository {
public:
  using StoredImage = std::vector<char>;
  using StoreImageResult = std::variant<model::IOResult, StoredImage>;
  using GetImageResult = std::variant<model::IOResult, StoredImage>;

  virtual ~ICharacterImageRepository() = default;

  virtual StoreImageResult storeImage(common::types::Int32 characterId, const std::string& imagePath) = 0;

  virtual GetImageResult getImage(common::types::Int32 characterId) = 0;

  virtual GetImageResult getThumbnail(common::types::Int32 characterId) = 0;

  virtual bool imageExists(common::types::Int32 characterId) = 0;

  virtual bool thumbnailExists(common::types::Int32 characterId) = 0;

  virtual std::string getImagePath(common::types::Int32 characterId) = 0;

  virtual std::string getThumbnailPath(common::types::Int32 characterId) = 0;
};

} // namespace domain::repository
