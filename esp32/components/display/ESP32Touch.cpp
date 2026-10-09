#include "ESP32Touch.h"

#include <cstdlib>
#include <esp_check.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include "ESP32DisplayConfig.h"

using namespace infra::display::esp32;

namespace {

constexpr char TAG[] = "ESP32Touch";

constexpr uint16_t GT911_STATUS = 0x814E;
constexpr uint16_t GT911_PRODUCT_ID = 0x8140;
constexpr uint16_t GT911_CONFIG = 0x8047;

} // namespace

ESP32Touch::~ESP32Touch()
{
  if (m_gt911 != nullptr) {
    i2c_master_bus_rm_device(m_gt911);
    m_gt911 = nullptr;
  }

  if (m_ch422g != nullptr) {
    i2c_master_bus_rm_device(m_ch422g);
    m_ch422g = nullptr;
  }

  if (m_bus != nullptr) {
    i2c_del_master_bus(m_bus);
    m_bus = nullptr;
  }
}

void ESP32Touch::init()
{
  initI2C();
  initCH422G();
  initGT911();
}

void ESP32Touch::initI2C()
{
  constexpr i2c_master_bus_config_t config{
    .i2c_port = ESP32DisplayConfig::i2cPort,
    .sda_io_num = ESP32DisplayConfig::i2cSda,
    .scl_io_num = ESP32DisplayConfig::i2cScl,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .flags = {
      .enable_internal_pullup = true,
    },
  };

  ESP_ERROR_CHECK(i2c_new_master_bus(&config, &m_bus));

  constexpr i2c_device_config_t ch422gConfig{
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = ESP32DisplayConfig::ch422gAddress,
    .scl_speed_hz = ESP32DisplayConfig::i2cFrequency,
  };

  ESP_ERROR_CHECK(i2c_master_bus_add_device(m_bus, &ch422gConfig, &m_ch422g));

  constexpr i2c_device_config_t gt911Config{
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = ESP32DisplayConfig::gt911Address,
    .scl_speed_hz = ESP32DisplayConfig::i2cFrequency,
  };

  ESP_ERROR_CHECK(i2c_master_bus_add_device(m_bus, &gt911Config, &m_gt911));
}

void ESP32Touch::initCH422G()
{
  /*
   * CH422G:
   *
   * IO1 -> GT911 reset/control
   * IO2 -> LCD backlight
   */

  ch422gWrite(ESP32DisplayConfig::ch422gModeRegister, 0xFF);

  m_ch422gOutput = 0xFF;

  ch422gOutput(ESP32DisplayConfig::touchResetIo, false);

  vTaskDelay(pdMS_TO_TICKS(100));

  gpio_config_t interruptConfig{
    .pin_bit_mask = 1ULL << ESP32DisplayConfig::touchInterrupt,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE,
  };

  ESP_ERROR_CHECK(gpio_config(&interruptConfig));

  ESP_ERROR_CHECK(gpio_set_level(ESP32DisplayConfig::touchInterrupt, 0));

  vTaskDelay(pdMS_TO_TICKS(100));

  ch422gOutput(ESP32DisplayConfig::touchResetIo, true);

  vTaskDelay(pdMS_TO_TICKS(200));

  interruptConfig.mode = GPIO_MODE_INPUT;
  interruptConfig.intr_type = GPIO_INTR_DISABLE;

  ESP_ERROR_CHECK(gpio_config(&interruptConfig));
}

void ESP32Touch::initGT911()
{
  uint8_t productId[4]{};

  if (!readRegisters(GT911_PRODUCT_ID, productId, sizeof(productId))) {
    ESP_LOGE(TAG, "Unable to read GT911 product ID");
    std::abort();
  }

  ESP_LOGI(TAG, "GT911 ID: %02X %02X %02X, config: %u", productId[0], productId[1], productId[2], productId[3]);

  uint8_t configVersion = 0;

  if (!readRegisters(GT911_CONFIG, &configVersion, 1)) {
    ESP_LOGE(TAG, "Unable to read GT911 configuration");
    std::abort();
  }

  ESP_LOGI(TAG, "GT911 configuration version: %u", configVersion);
}

bool ESP32Touch::touched() const noexcept
{
  constexpr uint8_t address[] = {0x81, 0x4E};
  uint8_t status = 0;

  const esp_err_t result = i2c_master_transmit_receive(
      m_gt911,
      address,
      sizeof(address),
      &status,
      sizeof(status),
      100
      );

  return result == ESP_OK && (status & 0x80U) != 0;
}

bool ESP32Touch::read(uint16_t& x, uint16_t& y) noexcept
{
  uint8_t status = 0;

  if (!readRegisters(GT911_STATUS, &status, 1)) {
    return false;
  }

  if ((status & 0x80U) == 0) {
    return false;
  }

  const uint8_t pointCount = status & 0x0F;

  if (pointCount == 0 || pointCount > 5) {
    return false;
  }

  uint8_t data[8]{};

  if (!readRegisters(GT911_STATUS + 1, data, sizeof(data))) {
    return false;
  }

  x = static_cast<uint16_t>(data[1] | (static_cast<uint16_t>(data[2]) << 8));

  y = static_cast<uint16_t>(data[3] | (static_cast<uint16_t>(data[4]) << 8));

  writeRegister(GT911_STATUS, 0);

  return true;
}

void ESP32Touch::ch422gWrite(const uint8_t reg, const uint8_t value)
{
  const uint8_t data[] = {reg, value};

  ESP_ERROR_CHECK(i2c_master_transmit(m_ch422g, data, sizeof(data), 100));
}

void ESP32Touch::ch422gOutput(const uint8_t pin, const bool value)
{
  if (value) {
    m_ch422gOutput |= static_cast<uint8_t>(1U << pin);
  } else {
    m_ch422gOutput &= static_cast<uint8_t>(~(1U << pin));
  }

  ch422gWrite(ESP32DisplayConfig::ch422gOutputRegister, m_ch422gOutput);
}

void ESP32Touch::writeRegister(const uint16_t reg, const uint8_t value)
{
  const uint8_t address[] = {
    static_cast<uint8_t>(reg >> 8),
    static_cast<uint8_t>(reg & 0xFF),
  };

  const uint8_t data[] = {
    address[0],
    address[1],
    value,
  };

  ESP_ERROR_CHECK(i2c_master_transmit(m_gt911, data, sizeof(data), 100));
}

bool ESP32Touch::readRegisters(const uint16_t reg, uint8_t* data, const size_t size) noexcept
{
  const uint8_t address[] = {
    static_cast<uint8_t>(reg >> 8),
    static_cast<uint8_t>(reg & 0xFF),
  };

  return i2c_master_transmit_receive(m_gt911, address, sizeof(address), data, size, 100) == ESP_OK;
}
