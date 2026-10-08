#pragma once

#include <kangaru/kangaru.hpp>

#include "di/DomainModule.h"
#include "ui/MainViewModel.h"

namespace app::di {
struct MainViewModel
  : kgr::single_service<ui::MainViewModel, kgr::dependency<UpdateTitleUseCaseService, GetCharactersUseCase>>
{};
} // namespace app::di
