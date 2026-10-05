#pragma once

#include <memory>
#include <string>

#include "MainViewUIState.h"

namespace domain::model {
struct CharactersResponse;
}

namespace domain::usecase {
class IUpdateTitleUseCase;
class IGetCharactersUseCase;
} // namespace domain::usecase

namespace app::ui {
class MainViewModel {
public:
  explicit MainViewModel(const std::shared_ptr<domain::usecase::IUpdateTitleUseCase>& updateTitleUseCase,
                         const std::shared_ptr<domain::usecase::IGetCharactersUseCase>& getCharactersUseCase);

  MainViewUIData update();

  MainViewUIData nextPage() const;

  MainViewUIData previousPage() const;

private:
  void handleCharactersResponse(const domain::model::CharactersResponse& response);

private:
  std::string m_title;
  std::shared_ptr<domain::usecase::IUpdateTitleUseCase> m_updateTitleUseCase;
  std::shared_ptr<domain::usecase::IGetCharactersUseCase> m_getCharactersUseCase;
};
} // namespace app::ui
