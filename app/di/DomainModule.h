#pragma once

#include <kangaru/kangaru.hpp>

#include "DataModule.h"
#include "domain/usecase/UpdateTitleUseCase.h"
#include "domain/usecase/GetCharactersUseCase.h"
#include "domain/usecase/IGetCharactersUseCase.h"

namespace app::di {

struct UpdateTitleUseCaseImplService;

struct UpdateTitleUseCaseService
    : kgr::abstract_shared_service<domain::usecase::IUpdateTitleUseCase>
      , kgr::defaults_to<UpdateTitleUseCaseImplService> {};

struct UpdateTitleUseCaseImplService
    : kgr::shared_service<domain::usecase::UpdateTitleUseCase>
      , kgr::overrides<UpdateTitleUseCaseService> {};

// GetCharactersUseCase

struct GetCharactersUseCaseImplService;

struct GetCharactersUseCaseService
    : kgr::abstract_shared_service<domain::usecase::IGetCharactersUseCase>
      , kgr::defaults_to<GetCharactersUseCaseImplService> {};

struct GetCharactersUseCaseImplService
    : kgr::shared_service<
        domain::usecase::GetCharactersUseCase,
        kgr::dependency<CharacterRepository>>
      , kgr::overrides<GetCharactersUseCaseService> {};

} // namespace app::di
