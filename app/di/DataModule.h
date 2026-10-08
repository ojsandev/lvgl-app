#pragma once

#include <kangaru/kangaru.hpp>

#include "data/datasource/DemonSlayerDataSource.h"
#include "data/http/IHttpClient.h"
#include "data/repository/CharacterRepository.h"
#include "data/repository/image/CharacterImageRepository.h"
#include "image/ImageResizer.h"
#include "infra/http/BoostBeastHttpClient.h"

namespace app::di {
struct HttpClientImplService;

struct HttpClientService
  : kgr::abstract_shared_service<data::http::IHttpClient>
  , kgr::defaults_to<HttpClientImplService>
{};

struct HttpClientImplService
  : kgr::shared_service<infra::http::BoostBeastHttpClient>
  , kgr::overrides<HttpClientService>
{};

struct DemonSlayerDataSource
  : kgr::shared_service<data::datasource::DemonSlayerDataSource, kgr::dependency<HttpClientService>>
{};

struct CharacterRepository
  : kgr::shared_service<data::repository::CharacterRepository, kgr::dependency<DemonSlayerDataSource>>
{};

struct CharacterImageRepository
  : kgr::shared_service<data::repository::CharacterImageRepository, kgr::dependency<DemonSlayerDataSource>>
{};

struct ImageResizerRepository
  : kgr::shared_service<infra::image::ImageResizer>
  , kgr::defaults_to<domain::repository::IImageResizerRepository>
{};

} // namespace app::di
