#pragma once

#include <memory>
#include <optional>
#include <string>

#include "MainViewUIState.h"
#include "common/async/Task.h"
#include "domain/model/Character.h"

namespace domain::usecase {
class IUpdateTitleUseCase;
class IGetCharactersUseCase;
}

namespace app::ui {

class MainViewModel {
public:
  using CharactersTask = common::async::Task<domain::model::CharacterView>;

  explicit MainViewModel(
    const std::shared_ptr<domain::usecase::IUpdateTitleUseCase>& updateTitleUseCase,
    const std::shared_ptr<domain::usecase::IGetCharactersUseCase>& getCharactersUseCase);

  void update();
  void nextPage();
  void previousPage();

  [[nodiscard]] bool isLoading() const noexcept;
  [[nodiscard]] std::optional<MainViewUIData> poll();

private:
  void startTask(CharactersTask task);

  std::string m_title;
  std::shared_ptr<domain::usecase::IUpdateTitleUseCase> m_updateTitleUseCase;
  std::shared_ptr<domain::usecase::IGetCharactersUseCase> m_getCharactersUseCase;
  std::optional<CharactersTask> m_currentTask;
  std::optional<MainViewUIData> m_pendingUiData;
};

} // namespace app::ui
