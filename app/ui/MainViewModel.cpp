#include "MainViewModel.h"

#include <ostream>

#include "MainViewUIState.h"
#include "domain/usecase/IGetCharactersUseCase.h"
#include "domain/usecase/IUpdateTitleUseCase.h"
#include "domain/exception/characters/CharacterNotFoundException.h"
#include "domain/model/CharactersResponse.h"

using namespace app::ui;

MainViewModel::MainViewModel(const std::shared_ptr<domain::usecase::IUpdateTitleUseCase>& updateTitleUseCase,
                             const std::shared_ptr<domain::usecase::IGetCharactersUseCase>& getCharactersUseCase)
  : m_updateTitleUseCase(updateTitleUseCase)
    , m_getCharactersUseCase(getCharactersUseCase) {}

MainViewUIData MainViewModel::update()
{
  try {
    m_title = m_updateTitleUseCase->execute();

    auto [characters, pagination] = m_getCharactersUseCase->execute(20, 1);

    return MainViewUIState::data(m_title, characters, pagination.currentPage, pagination.totalPages);
  }
  catch (const domain::exception::CharacterNotFoundException&) {
    return MainViewUIState::errorState();
  }
}

MainViewUIData MainViewModel::nextPage() const
{
  try {

    const auto [characters, pagination] = m_getCharactersUseCase->nextPage();
    return MainViewUIState::data(m_title, characters, pagination.currentPage, pagination.totalPages);
  }
  catch (const domain::exception::CharacterNotFoundException& error) {
    std::print("MainViewModel::nextPage: CharacterNotFoundException caught: {}\n", error.what());
    return MainViewUIState::errorState();
  }
}

MainViewUIData MainViewModel::previousPage() const
{
  try {
    const auto [characters, pagination] = m_getCharactersUseCase->previousPage();
    return MainViewUIState::data(m_title, characters, pagination.currentPage, pagination.totalPages);
  }
  catch (const domain::exception::CharacterNotFoundException& error) {
    std::print("MainViewModel::previousPage: CharacterNotFoundException caught: {}\n", error.what());
    return MainViewUIState::errorState();
  }
}
