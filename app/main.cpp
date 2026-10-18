#include <kangaru/kangaru.hpp>

#include "app/di/AppModule.h"
#include "platform/sdl/App.h"

int main()
{
  kgr::container container;

  container.emplace<app::di::WorkerCountService>(4);
  auto& mainViewModel = container.service<app::di::MainViewModel>();

  lv::App app({ .width = 800, .height = 565 }, mainViewModel);

  app.run();

  return 0;
}
