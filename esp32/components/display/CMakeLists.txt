idf_component_register(
  SRCS
  ESP32Display.cpp
  ESP32Touch.cpp

  INCLUDE_DIRS
  "."

  REQUIRES
  app
  lv_cpp
  lvgl__lvgl
  esp_lcd
  esp_timer
  esp_driver_i2c
  esp_driver_gpio
)
