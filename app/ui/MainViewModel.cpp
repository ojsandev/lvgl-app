#include "MainViewModel.h"

#include <utility>
#include "domain/usecase/IUpdateTitleUseCase.h"

using namespace app::ui;

MainViewModel::MainViewModel(
    const std::shared_ptr<domain::IUpdateTitleUseCase>& updateTitleUseCase
)
    : m_title("Initial Title"),
      m_updateTitleUseCase(updateTitleUseCase)
{
}

void MainViewModel::handleTitleUpdate()
{
    m_title = m_updateTitleUseCase->execute();
}

const std::string& MainViewModel::title() const noexcept
{
    return m_title;
}
