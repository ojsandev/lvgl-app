#pragma once

#include <driver/i2c_master.h>

namespace infra::display::esp32 {

class ESP32Touch
{
public:
  ESP32Touch() = default;
  ~ESP32Touch();

  ESP32Touch(const ESP32Touch&) = delete;
  ESP32Touch& operator=(const ESP32Touch&) = delete;

  ESP32Touch(ESP32Touch&&) = delete;
  ESP32Touch& operator=(ESP32Touch&&) = delete;

  void init();

  [[nodiscard]] bool touched() const noexcept;

  bool read(uint16_t& x, uint16_t& y) noexcept;

private:
  void initI2C();
  void initCH422G();
  void initGT911();

  void ch422gWrite(uint8_t reg, uint8_t value);

  void ch422gOutput(uint8_t pin, bool value);

  void writeRegister(uint16_t reg, uint8_t value);

  bool readRegisters(uint16_t reg, uint8_t* data, size_t size) noexcept;

private:
  i2c_master_bus_handle_t m_bus = nullptr;
  i2c_master_dev_handle_t m_gt911 = nullptr;
  i2c_master_dev_handle_t m_ch422g = nullptr;

  uint8_t m_ch422gOutput = 0xFF;
};

} // namespace infra::display::esp32
