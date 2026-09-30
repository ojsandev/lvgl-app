#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <kangaru/kangaru.hpp>
#include <print>

#include "App.h"
#include "app/di/AppModule.h"
#include "infra/http/BoostBeastHttpClient.h"

int main()
{
    kgr::container container;

    auto& mainViewModel = container.service<app::di::MainViewModel>();

    lv::App app({.width = 700, .height = 400}, mainViewModel);

    app.run();

    return 0;
}