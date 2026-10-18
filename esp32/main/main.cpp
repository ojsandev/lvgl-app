#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lv/lv.hpp>
#include <memory>

#include "DemoUseCases.h"
#include "ESP32Display.h"
#include "app/ui/MainView.h"
#include "app/ui/MainViewModel.h"

namespace {
constexpr char TAG[] = "MY_LVGL";
}

extern "C" void app_main(void)
{
  ESP_LOGI(TAG, "Initializing LVGL");

  lv::init();

  infra::display::ESP32Display display;

  ESP_LOGI(TAG, "Display initialized: %ld x %ld", static_cast<long>(display.width()),
           static_cast<long>(display.height()));

  auto updateTitleUseCase = std::make_shared<esp32::demo::UpdateTitleUseCase>();
  auto getCharactersUseCase = std::make_shared<esp32::demo::GetCharactersUseCase>();

  app::ui::MainViewModel viewModel(updateTitleUseCase, getCharactersUseCase);
  app::ui::MainView mainView(display, viewModel);

  ESP_LOGI(TAG, "MainView initialized");

  while (true) {
    lv_timer_handler();
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}
