#include "MainViewModel.h"

#include <utility>
#include "domain/usecase/IUpdateTitleUseCase.h"

using namespace ui;

MainViewModel::MainViewModel(
       std::string initialTitle,
       const std::shared_ptr<domain::IUpdateTitleUseCase>& updateTitleUseCase
   )
       : m_title(std::move(initialTitle)),
         m_updateTitleUseCase(updateTitleUseCase)
{
}

void MainViewModel::handleTitleUpdate()
{
    m_title = m_updateTitleUseCase->execute();
}

const std::string& MainViewModel::title()
{
    return m_title;
}
