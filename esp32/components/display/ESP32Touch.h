#pragma once

#include <driver/i2c_master.h>
#include "common/types.h"

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

  bool read(common::types::UInt16& x, common::types::UInt16& y) noexcept;

  void setBacklight(bool value);

private:
  void initI2C();
  void initCH422G();
  void initGT911();

  void ch422gWrite(common::types::UInt8 reg, common::types::UInt8 value);
  void ch422gOutput(common::types::UInt8 pin, bool value);
  void writeRegister(common::types::UInt16 reg, common::types::UInt8 value);
  bool readRegisters(common::types::UInt16 reg, common::types::UInt8* data, size_t size) noexcept;

private:
  i2c_master_bus_handle_t m_bus = nullptr;
  i2c_master_dev_handle_t m_gt911 = nullptr;
  i2c_master_dev_handle_t m_ch422g = nullptr;

  common::types::UInt8 m_ch422gOutput = 0xFF;
};

} // namespace infra::display::esp32
