#include "MainViewModel.h"

#include <exception>
#include <utility>

#include "common/logger/Logging.h"
#include "domain/exception/characters/CharacterNotFoundException.h"
#include "domain/exception/characters/CharactersOutOfBoundsException.h"
#include "domain/usecase/IUpdateTitleUseCase.h"
#include "domain/usecase/characters/IGetCharactersUseCase.h"

using namespace app::ui;

MainViewModel::MainViewModel(const std::shared_ptr<domain::usecase::IUpdateTitleUseCase>& updateTitleUseCase,
                             const std::shared_ptr<domain::usecase::IGetCharactersUseCase>& getCharactersUseCase)
  : m_updateTitleUseCase(updateTitleUseCase)
  , m_getCharactersUseCase(getCharactersUseCase)
{}

void MainViewModel::update()
{
  if (isLoading()) {
    return;
  }

  try {
    m_title = m_updateTitleUseCase->execute();
    startTask(m_getCharactersUseCase->execute(20, 1));
  } catch (const std::exception& error) {
    LOG_ERROR("MainViewModel::update: {}", error.what());
    m_pendingUiData = MainViewUIState::errorState();
  } catch (...) {
    LOG_ERROR("MainViewModel::update: unknown error");
    m_pendingUiData = MainViewUIState::errorState();
  }
}

void MainViewModel::nextPage()
{
  if (isLoading()) {
    return;
  }

  try {
    startTask(m_getCharactersUseCase->nextPage());
  } catch (const std::exception& error) {
    LOG_ERROR("MainViewModel::nextPage: {}", error.what());
    m_pendingUiData = MainViewUIState::errorState();
  } catch (...) {
    LOG_ERROR("MainViewModel::nextPage: unknown error");
    m_pendingUiData = MainViewUIState::errorState();
  }
}

void MainViewModel::previousPage()
{
  if (isLoading()) {
    return;
  }

  try {
    startTask(m_getCharactersUseCase->previousPage());
  } catch (const std::exception& error) {
    LOG_ERROR("MainViewModel::previousPage: {}", error.what());
    m_pendingUiData = MainViewUIState::errorState();
  } catch (...) {
    LOG_ERROR("MainViewModel::previousPage: unknown error");
    m_pendingUiData = MainViewUIState::errorState();
  }
}

bool MainViewModel::isLoading() const noexcept
{
  return m_currentTask.has_value();
}

std::optional<MainViewUIData> MainViewModel::poll()
{
  if (m_pendingUiData) {
    auto result = std::move(m_pendingUiData);
    m_pendingUiData.reset();
    return result;
  }

  if (!m_currentTask) {
    return std::nullopt;
  }

  if (!m_currentTask->isReady()) {
    return MainViewUIState::loadingState();
  }

  const auto task = *m_currentTask;
  m_currentTask.reset();

  try {
    const auto& [characters, pagination] = task.result();
    return MainViewUIState::data(m_title, characters, pagination.currentPage, pagination.totalPages);
  } catch (...) {
    LOG_ERROR("Character task failed with unknown error");
    return MainViewUIState::errorState();
  }
}

void MainViewModel::startTask(CharactersTask task)
{
  m_currentTask.emplace(std::move(task));
}
