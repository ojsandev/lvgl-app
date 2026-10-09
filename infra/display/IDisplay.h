
#pragma once

#include "common/types.h"
#include "lv/core/display.hpp"

namespace infra::display {

class IDisplay {
public:
  virtual ~IDisplay() = default;

  IDisplay(const IDisplay&) = delete;
  IDisplay& operator=(const IDisplay&) = delete;
  IDisplay(IDisplay&&) = delete;
  IDisplay& operator=(IDisplay&&) = delete;

  [[nodiscard]] virtual lv::Display& lvgl() noexcept = 0;
  [[nodiscard]] virtual const lv::Display& lvgl() const noexcept = 0;

  [[nodiscard]] common::types::Int32 width() const noexcept
  {
    return lvgl().width();
  }

  [[nodiscard]] common::types::Int32 height() const noexcept
  {
    return lvgl().height();
  }

protected:
  IDisplay() = default;
};

} // namespace infra::display
