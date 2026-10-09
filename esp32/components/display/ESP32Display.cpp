#include "ESP32Display.h"

#include <cassert>
#include <esp_check.h>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include "ESP32DisplayConfig.h"

using namespace infra::display;

namespace {

constexpr char TAG[] = "ESP32Display";

constexpr size_t LVGL_DRAW_BUFFER_LINES = 40;

} // namespace

ESP32Display::ESP32Display()
  : m_display(lv::Display::create(esp32::ESP32DisplayConfig::width, esp32::ESP32DisplayConfig::height))
{
  initPanel();
  initBacklight();
  initLVGL();
  initTouch();
  initTick();
}

void ESP32Display::initTouch()
{
  m_touch.init();

  ESP_LOGI(TAG, "Touch controller initialized");
}

void ESP32Display::initTick()
{
  const esp_timer_create_args_t timerConfig{
    .callback = &ESP32Display::tickCallback,
    .arg = nullptr,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "lvgl_tick",
    .skip_unhandled_events = true,
  };

  ESP_ERROR_CHECK(esp_timer_create(&timerConfig, &m_tickTimer));

  ESP_ERROR_CHECK(esp_timer_start_periodic(m_tickTimer, 1000));
}

void ESP32Display::tickCallback(void* arg)
{
  (void)arg;
  lv_tick_inc(1);
}

void ESP32Display::flushCallback(lv_display_t* display, const lv_area_t* area, uint8_t* pxMap)
{
  auto* self = static_cast<ESP32Display*>(lv_display_get_user_data(display));

  if (self == nullptr || self->m_panel == nullptr) {
    lv_display_flush_ready(display);
    return;
  }

  const esp_err_t result = esp_lcd_panel_draw_bitmap(self->m_panel, area->x1, area->y1, area->x2 + 1, area->y2 + 1,
                                                     pxMap);

  if (result != ESP_OK) {
    ESP_LOGE(TAG, "Failed to flush display: %s", esp_err_to_name(result));
    lv_display_flush_ready(display);
  }
}

void ESP32Display::touchCallback(
    lv_indev_t* indev,
    lv_indev_data_t* data)
{
  auto* touch = static_cast<esp32::ESP32Touch*>(
    lv_indev_get_user_data(indev)
  );

  if (touch == nullptr) {
    data->state = LV_INDEV_STATE_RELEASED;
    return;
  }

  uint16_t x = 0;
  uint16_t y = 0;

  if (touch->read(x, y)) {
    data->point.x = static_cast<int32_t>(x);
    data->point.y = static_cast<int32_t>(y);
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

ESP32Display::~ESP32Display()
{
  if (m_tickTimer != nullptr) {
    esp_timer_stop(m_tickTimer);
    esp_timer_delete(m_tickTimer);
    m_tickTimer = nullptr;
  }

  if (m_panel != nullptr) {
    esp_lcd_panel_del(m_panel);
    m_panel = nullptr;
  }
}

lv::Display& ESP32Display::lvgl() noexcept
{
  return m_display;
}

void ESP32Display::initPanel()
{
  using Config = esp32::ESP32DisplayConfig;

  esp_lcd_rgb_panel_config_t panelConfig{};

  panelConfig.clk_src = LCD_CLK_SRC_DEFAULT;

  panelConfig.timings.pclk_hz = Config::pixelClockHz;
  panelConfig.timings.h_res = Config::width;
  panelConfig.timings.v_res = Config::height;

  panelConfig.timings.hsync_pulse_width = 162;
  panelConfig.timings.hsync_back_porch = 152;
  panelConfig.timings.hsync_front_porch = 48;

  panelConfig.timings.vsync_pulse_width = 45;
  panelConfig.timings.vsync_back_porch = 13;
  panelConfig.timings.vsync_front_porch = 3;

  panelConfig.timings.flags.pclk_active_neg = 1;

  panelConfig.data_width = Config::rgbDataWidth;
  panelConfig.bits_per_pixel = Config::bitsPerPixel;

  panelConfig.num_fbs = 1;

  panelConfig.bounce_buffer_size_px = Config::width * Config::bounceBufferLines;

  panelConfig.sram_trans_align = 4;
  panelConfig.psram_trans_align = 64;

  panelConfig.hsync_gpio_num = Config::hsync;
  panelConfig.vsync_gpio_num = Config::vsync;
  panelConfig.de_gpio_num = Config::de;
  panelConfig.pclk_gpio_num = Config::pclk;
  panelConfig.disp_gpio_num = Config::displayEnable;

  panelConfig.data_gpio_nums[0] = Config::rgbData0;
  panelConfig.data_gpio_nums[1] = Config::rgbData1;
  panelConfig.data_gpio_nums[2] = Config::rgbData2;
  panelConfig.data_gpio_nums[3] = Config::rgbData3;
  panelConfig.data_gpio_nums[4] = Config::rgbData4;
  panelConfig.data_gpio_nums[5] = Config::rgbData5;
  panelConfig.data_gpio_nums[6] = Config::rgbData6;
  panelConfig.data_gpio_nums[7] = Config::rgbData7;
  panelConfig.data_gpio_nums[8] = Config::rgbData8;
  panelConfig.data_gpio_nums[9] = Config::rgbData9;
  panelConfig.data_gpio_nums[10] = Config::rgbData10;
  panelConfig.data_gpio_nums[11] = Config::rgbData11;
  panelConfig.data_gpio_nums[12] = Config::rgbData12;
  panelConfig.data_gpio_nums[13] = Config::rgbData13;
  panelConfig.data_gpio_nums[14] = Config::rgbData14;
  panelConfig.data_gpio_nums[15] = Config::rgbData15;

  panelConfig.flags.fb_in_psram = 1;

  ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panelConfig, &m_panel));

  ESP_ERROR_CHECK(esp_lcd_panel_init(m_panel));

  ESP_LOGI(TAG, "RGB panel initialized: %dx%d", Config::width, Config::height);
}

void ESP32Display::initLVGL()
{
  const size_t bufferSize = esp32::ESP32DisplayConfig::width * LVGL_DRAW_BUFFER_LINES * sizeof(lv_color16_t);

  void* buffer1 = heap_caps_malloc(bufferSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

  void* buffer2 = heap_caps_malloc(bufferSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

  assert(buffer1 != nullptr);
  assert(buffer2 != nullptr);

  m_display.color_format(LV_COLOR_FORMAT_RGB565)
           .buffers(buffer1, buffer2, bufferSize, LV_DISPLAY_RENDER_MODE_PARTIAL)
           .flush_cb(&ESP32Display::flushCallback);

  lv_display_set_user_data(m_display.get(), this);

  lv_display_set_default(m_display.get());

  auto* indev = lv_indev_create();

  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);

  lv_indev_set_display(indev, m_display.get());

  lv_indev_set_user_data(indev, &m_touch);

  lv_indev_set_read_cb(indev, &ESP32Display::touchCallback);

  const esp_lcd_rgb_panel_event_callbacks_t callbacks{.on_color_trans_done = &ESP32Display::colorTransDoneCallback};

  ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(m_panel, &callbacks, m_display.get()));
}

const lv::Display& ESP32Display::lvgl() const noexcept
{
  return m_display;
}

bool ESP32Display::colorTransDoneCallback(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t* eventData,
                                          void* userData)
{
  (void)panel;
  (void)eventData;

  auto* display = static_cast<lv_display_t*>(userData);

  if (display != nullptr) {
    lv_display_flush_ready(display);
  }

  return false;
}

void ESP32Display::initBacklight()
{
  using Config = esp32::ESP32DisplayConfig;

  const uint8_t mode[] = {Config::ch422gModeRegister, 0xFF};

  const uint8_t output[] = {Config::ch422gOutputRegister, static_cast<uint8_t>(1U << Config::backlightIo)};

  /*
   * The CH422G is initialized by ESP32Touch because
   * the same I2C bus is required by GT911.
   *
   * Backlight control is therefore performed there.
   */
}
