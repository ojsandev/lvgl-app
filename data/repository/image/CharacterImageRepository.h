#pragma once

#include <memory>
#include <string>

#include "common/types.h"
#include "domain/repository/ICharacterImageRepository.h"

namespace data::datasource {
class DemonSlayerDataSource;
}

namespace data::repository {
class CharacterImageRepository : public domain::repository::ICharacterImageRepository {
public:
  CharacterImageRepository(const std::shared_ptr<datasource::DemonSlayerDataSource>& demonSlayerDataSource);
  ~CharacterImageRepository() override = default;

  StoreImageResult storeImage(common::types::Int32 characterId, const std::string& imagePath) override;
  GetImageResult getImage(common::types::Int32 characterId) override;
  GetImageResult getThumbnail(common::types::Int32 characterId) override;
  bool imageExists(common::types::Int32 characterId) override;
  bool thumbnailExists(common::types::Int32 characterId) override;
  std::string getImagePath(common::types::Int32 characterId) override;
  std::string getThumbnailPath(common::types::Int32 characterId) override;

private:
  std::shared_ptr<datasource::DemonSlayerDataSource> m_demonSlayerDataSource;
};
} // namespace data::repository
