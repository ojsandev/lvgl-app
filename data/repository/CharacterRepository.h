#pragma once

#include <memory>
#include <variant>

#include "domain/repository/ICharacterRepository.h"

namespace data::datasource {
class DemonSlayerDataSource;
}

namespace data::repository {
class CharacterRepository final : public domain::repository::ICharacterRepository {
public:
  explicit CharacterRepository(const std::shared_ptr<datasource::DemonSlayerDataSource>& demonSlayerDataSource);
  ~CharacterRepository() override = default;

  GetCharactersResult getCharactersPaginated(common::types::Int32 limit, common::types::Int32 page) override;

private:
  std::shared_ptr<datasource::DemonSlayerDataSource> m_demonSlayerDataSource;
};
} // namespace data::repository
