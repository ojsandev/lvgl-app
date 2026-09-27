#include <memory>

#include "App.h"
#include "domain/usecase/UpdateTitleUseCase.h"
#include "ui/MainViewModel.h"

int main()
{
    auto useCase = std::make_shared<domain::UpdateTitleUseCase>();
    const auto viewModel = std::make_shared<ui::MainViewModel>("Initial title", useCase);

    lv::App app(
        {
            .width = 700,
            .height = 400
        }, viewModel
    );

    app.run();

    return 0;
}
