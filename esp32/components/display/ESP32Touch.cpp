#include "ESP32Touch.h"

#include <cstdlib>
#include <driver/gpio.h>
#include <esp_check.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ESP32DisplayConfig.h"

using namespace infra::display::esp32;

namespace {

constexpr char TAG[] = "ESP32Touch";

constexpr common::types::UInt16 GT911_STATUS = 0x814E;
constexpr common::types::UInt16 GT911_PRODUCT_ID = 0x8140;
constexpr common::types::UInt16 GT911_CONFIG = 0x8047;

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
  common::types::UInt8 productId[4]{};

  if (!readRegisters(GT911_PRODUCT_ID, productId, sizeof(productId))) {
    ESP_LOGE(TAG, "Unable to read GT911 product ID");
    std::abort();
  }

  ESP_LOGI(TAG, "GT911 ID: %02X %02X %02X, config: %u", productId[0], productId[1], productId[2], productId[3]);

  common::types::UInt8 configVersion = 0;

  if (!readRegisters(GT911_CONFIG, &configVersion, 1)) {
    ESP_LOGE(TAG, "Unable to read GT911 configuration");
    std::abort();
  }

  ESP_LOGI(TAG, "GT911 configuration version: %u", configVersion);
}

bool ESP32Touch::touched() const noexcept
{
  constexpr common::types::UInt8 address[] = { 0x81, 0x4E };
  common::types::UInt8 status = 0;

  const esp_err_t result = i2c_master_transmit_receive(m_gt911, address, sizeof(address), &status, sizeof(status), 100);

  return result == ESP_OK && (status & 0x80U) != 0;
}

bool ESP32Touch::read(common::types::UInt16& x, common::types::UInt16& y) noexcept
{
  common::types::UInt8 status = 0;

  if (!readRegisters(GT911_STATUS, &status, 1)) {
    return false;
  }

  if ((status & 0x80U) == 0) {
    return false;
  }

  const common::types::UInt8 pointCount = status & 0x0F;

  if (pointCount == 0 || pointCount > 5) {
    return false;
  }

  common::types::UInt8 data[8]{};

  if (!readRegisters(GT911_STATUS + 1, data, sizeof(data))) {
    return false;
  }

  x = static_cast<common::types::UInt16>(data[1] | (static_cast<common::types::UInt16>(data[2]) << 8));

  y = static_cast<common::types::UInt16>(data[3] | (static_cast<common::types::UInt16>(data[4]) << 8));

  writeRegister(GT911_STATUS, 0);

  return true;
}

/**
 * @brief Writes a value to a CH422G register.
 *
 * Sends the register address and the value in a single I2C
 * transaction. Communication errors are handled by ESP_ERROR_CHECK().
 *
 * @param reg The 8-bit CH422G register address.
 * @param value The 8-bit value to write.
 */
void ESP32Touch::ch422gWrite(const common::types::UInt8 reg, const common::types::UInt8 value)
{
  const common::types::UInt8 data[] = { reg, value };

  ESP_ERROR_CHECK(i2c_master_transmit(m_ch422g, data, sizeof(data), 100));
}

/**
 * @brief Sets the logical state of a CH422G output.
 *
 * Updates the corresponding bit in m_ch422gOutput and writes
 * the updated byte to the CH422G output register.
 *
 * @param pin The bit index corresponding to the output.
 * @param value true to set the output high; false to set it low.
 */
void ESP32Touch::ch422gOutput(const common::types::UInt8 pin, const bool value)
{
  if (value) {
    m_ch422gOutput |= static_cast<common::types::UInt8>(1U << pin);
  } else {
    m_ch422gOutput &= static_cast<common::types::UInt8>(~(1U << pin));
  }

  ch422gWrite(ESP32DisplayConfig::ch422gOutputRegister, m_ch422gOutput);
}

/**
 * @brief Writes a byte to a GT911 register.
 *
 * The 16-bit register address is sent most-significant byte
 * first, followed by the least-significant byte and the value.
 * Communication errors are handled by ESP_ERROR_CHECK().
 *
 * @param reg The 16-bit GT911 register address.
 * @param value The 8-bit value to write.
 */

void ESP32Touch::writeRegister(const common::types::UInt16 reg, const common::types::UInt8 value)
{
  const common::types::UInt8 address[] = {
    static_cast<common::types::UInt8>(reg >> 8),
    static_cast<common::types::UInt8>(reg & 0xFF),
  };

  const common::types::UInt8 data[] = {
    address[0],
    address[1],
    value,
  };

  ESP_ERROR_CHECK(i2c_master_transmit(m_gt911, data, sizeof(data), 100));
}

/**
 * @brief Reads consecutive bytes from the GT911 starting at a register.
 *
 * Sends the register address and receives the requested data
 * into the provided buffer. This method does not propagate
 * C++ exceptions.
 *
 * @param reg The 16-bit address of the first GT911 register to read.
 * @param data Pointer to a destination buffer with capacity for
 *             at least size bytes.
 * @param size The number of bytes to read.
 *
 * @return true if the I2C transaction succeeds; false otherwise.
 */
bool ESP32Touch::readRegisters(const common::types::UInt16 reg, common::types::UInt8* data, const size_t size) noexcept
{
  const common::types::UInt8 address[] = {
    static_cast<common::types::UInt8>(reg >> 8),
    static_cast<common::types::UInt8>(reg & 0xFF),
  };

  return i2c_master_transmit_receive(m_gt911, address, sizeof(address), data, size, 100) == ESP_OK;
}

/**
 * @brief Sets the backlight state of the display.
 *
 * Uses the CH422G to control the backlight output pin.
 *
 * @param value true to turn on the backlight; false to turn it off.
 */
void ESP32Touch::setBacklight(const bool value)
{
  ch422gOutput(ESP32DisplayConfig::backlightIo, value);
}
