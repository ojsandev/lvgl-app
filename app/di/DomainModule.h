#pragma once

#include <kangaru/kangaru.hpp>

#include "domain/usecase/UpdateTitleUseCase.h"

namespace app::di
{
    struct UpdateTitleUseCaseImplService;

    struct UpdateTitleUseCaseService
        : kgr::abstract_shared_service<domain::IUpdateTitleUseCase>, kgr::defaults_to<UpdateTitleUseCaseImplService>
    {};

    struct UpdateTitleUseCaseImplService
        : kgr::shared_service<domain::UpdateTitleUseCase>, kgr::overrides<UpdateTitleUseCaseService>
    {};
}