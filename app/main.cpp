#include <kangaru/kangaru.hpp>

#include "App.h"
#include "app/di/AppModule.h"

int main()
{
    kgr::container container;

    auto& mainViewModel = container.service<app::di::MainViewModel>();

    lv::App app(
        {
            .width = 700,
            .height = 400
        },
        mainViewModel
    );

    app.run();

    return 0;
}
