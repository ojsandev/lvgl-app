#include "MainViewUIState.h"

using namespace app::ui;

MainViewUIData MainViewUIState::errorState()
{
  return MainViewUIData{
    .title = "",
    .characters = std::vector<domain::model::Character>{},
    .currentPage = 0,
    .totalPages = 0,
    .hasPreviousPage = false,
    .hasNextPage = false,
    .hasError = true,
  };
}

MainViewUIData MainViewUIState::fromTitle(const std::string& title)
{
  return MainViewUIData{
    .title = title,
    .characters = std::vector<domain::model::Character>{},
    .currentPage = 0,
    .totalPages = 0,
    .hasPreviousPage = false,
    .hasNextPage = false,
    .hasError = false,
  };
}

MainViewUIData MainViewUIState::data(const std::string& title,
                                     const std::vector<domain::model::Character>& characters,
                                     const common::types::UInt64 currentPage,
                                     const common::types::UInt64 totalPages)
{
  return MainViewUIData{
    .title = title,
    .characters = characters,
    .currentPage = currentPage,
    .totalPages = totalPages,
    .hasPreviousPage = currentPage > 1,
    .hasNextPage = currentPage < totalPages,
    .hasError = false
  };
}
