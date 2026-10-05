#pragma once

#include <kangaru/kangaru.hpp>

#include "data/datasource/DemonSlayerDataSource.h"
#include "data/http/IHttpClient.h"
#include "data/repository/CharacterRepository.h"
#include "infra/http/BoostBeastHttpClient.h"

namespace app::di {
struct HttpClientImplService;

struct HttpClientService
  : kgr::abstract_shared_service<data::http::IHttpClient>
  , kgr::defaults_to<HttpClientImplService> {};

struct HttpClientImplService
  : kgr::shared_service<infra::http::BoostBeastHttpClient>
  , kgr::overrides<HttpClientService> {};

struct DemonSlayerDataSource
  : kgr::shared_service<data::datasource::DemonSlayerDataSource, kgr::dependency<HttpClientService>> {};

struct CharacterRepository
  : kgr::shared_service<data::repository::CharacterRepository, kgr::dependency<DemonSlayerDataSource>> {};
} // namespace app::di
