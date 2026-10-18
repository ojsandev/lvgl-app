#pragma once

#include <memory>

#include "domain/repository/ICharacterRepository.h"
#include "http/AsyncHttpResult.h"

namespace data::datasource {
class DemonSlayerDataSource;
}

namespace data::repository {
class CharacterRepository final
  : public domain::repository::ICharacterRepository
  , public std::enable_shared_from_this<CharacterRepository> {
public:
  explicit CharacterRepository(const std::shared_ptr<datasource::DemonSlayerDataSource>& demonSlayerDataSource);
  ~CharacterRepository() override = default;

  GetCharactersResult getCharactersPaginated(common::types::Int32 limit, common::types::Int32 page) override;
  void onResponse(const http::AsyncHttpStringResult& result,
                  const common::async::TaskCompletionSource<GetCharactersValue>& completion);
  void processResponse(
      const data::http::AsyncHttpStringResult& result,
      const common::async::TaskCompletionSource<domain::repository::ICharacterRepository::GetCharactersValue>&
          completion);

private:
  std::shared_ptr<datasource::DemonSlayerDataSource> m_demonSlayerDataSource;
};
} // namespace data::repository
