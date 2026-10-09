#pragma once

#include <cstdint>
#include <esp_lcd_panel_ops.h>
#include <esp_timer.h>
#include <esp_lcd_panel_rgb.h>
#include "ESP32Touch.h"
#include "display/IDisplay.h"
#include "lv/core/display.hpp"

namespace infra::display {

class ESP32Display final : public IDisplay
{
public:
  ESP32Display();

  ~ESP32Display() override;

  ESP32Display(const ESP32Display&) = delete;
  ESP32Display& operator=(const ESP32Display&) = delete;

  ESP32Display(ESP32Display&&) = delete;
  ESP32Display& operator=(ESP32Display&&) = delete;

  [[nodiscard]] lv::Display& lvgl() noexcept override;
  [[nodiscard]] const lv::Display& lvgl() const noexcept override;

private:
  void initPanel();
  void initLVGL();
  void initBacklight();
  void initTick();
  void initTouch();

  static void flushCallback(lv_display_t* display, const lv_area_t* area, uint8_t* pxMap);

  static void touchCallback(lv_indev_t* indev, lv_indev_data_t* data);

  static void tickCallback(void* arg);

  static bool colorTransDoneCallback(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t* eventData,
                                     void* userData);

private:
  lv::Display m_display;

  esp_lcd_panel_handle_t m_panel = nullptr;

  esp32::ESP32Touch m_touch;

  esp_timer_handle_t m_tickTimer = nullptr;
};

} // namespace infra::display
