#pragma once

#include <driver/gpio.h>

#include "common/types.h"

namespace infra::display::esp32 {

struct ESP32DisplayConfig
{
  static constexpr int width = 1024;
  static constexpr int height = 600;

  static constexpr int pixelClockHz = 30 * 1000 * 1000;

  static constexpr int bitsPerPixel = 16;
  static constexpr int rgbDataWidth = 16;

  static constexpr int bounceBufferLines = 10;

  static constexpr gpio_num_t vsync = GPIO_NUM_3;
  static constexpr gpio_num_t hsync = GPIO_NUM_46;
  static constexpr gpio_num_t de = GPIO_NUM_5;
  static constexpr gpio_num_t pclk = GPIO_NUM_7;

  static constexpr gpio_num_t rgbData0 = GPIO_NUM_14;
  static constexpr gpio_num_t rgbData1 = GPIO_NUM_38;
  static constexpr gpio_num_t rgbData2 = GPIO_NUM_18;
  static constexpr gpio_num_t rgbData3 = GPIO_NUM_17;
  static constexpr gpio_num_t rgbData4 = GPIO_NUM_10;

  static constexpr gpio_num_t rgbData5 = GPIO_NUM_39;
  static constexpr gpio_num_t rgbData6 = GPIO_NUM_0;
  static constexpr gpio_num_t rgbData7 = GPIO_NUM_45;
  static constexpr gpio_num_t rgbData8 = GPIO_NUM_48;
  static constexpr gpio_num_t rgbData9 = GPIO_NUM_47;
  static constexpr gpio_num_t rgbData10 = GPIO_NUM_21;

  static constexpr gpio_num_t rgbData11 = GPIO_NUM_1;
  static constexpr gpio_num_t rgbData12 = GPIO_NUM_2;
  static constexpr gpio_num_t rgbData13 = GPIO_NUM_42;
  static constexpr gpio_num_t rgbData14 = GPIO_NUM_41;
  static constexpr gpio_num_t rgbData15 = GPIO_NUM_40;

  static constexpr gpio_num_t touchInterrupt = GPIO_NUM_4;

  static constexpr gpio_num_t touchReset = GPIO_NUM_NC;

  static constexpr gpio_num_t displayEnable = GPIO_NUM_NC;

  static constexpr gpio_num_t i2cSda = GPIO_NUM_8;
  static constexpr gpio_num_t i2cScl = GPIO_NUM_9;

  static constexpr int i2cPort = I2C_NUM_0;
  static constexpr common::types::UInt32 i2cFrequency = 400'000;

  static constexpr common::types::UInt8 gt911Address = 0x5D;

  static constexpr common::types::UInt8 ch422gAddress = 0x24;

  static constexpr common::types::UInt8 ch422gModeRegister = 0x02;
  static constexpr common::types::UInt8 ch422gOutputRegister = 0x03;

  static constexpr common::types::UInt8 touchResetIo = 1;
  static constexpr common::types::UInt8 backlightIo = 2;
};

} // namespace infra::display::esp32
