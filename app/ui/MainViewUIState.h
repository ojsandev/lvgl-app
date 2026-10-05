#pragma once

#include <string>
#include <vector>

#include "domain/model/Character.h"

namespace app::ui {
struct MainViewUIData {
  std::string title;
  std::vector<domain::model::Character> characters;

  common::types::UInt64 currentPage;
  common::types::UInt64 totalPages;

  bool hasPreviousPage;
  bool hasNextPage;
  bool hasError;
};

class MainViewUIState {
public:
  MainViewUIState() = default;
  ~MainViewUIState() = default;

  static MainViewUIData fromTitle(
      const std::string& title);

  static MainViewUIData data(
      const std::string& title,
      const std::vector<domain::model::Character>& characters,
      common::types::UInt64 currentPage,
      common::types::UInt64 totalPages);

  static MainViewUIData errorState();
};
}
