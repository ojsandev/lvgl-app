#pragma once

#include <lvgl.h>

namespace app::ui::theme {

inline lv_color_t background()
{
  return lv_color_hex(0x0F1117);
}

inline lv_color_t surface()
{
  return lv_color_hex(0x191C24);
}

inline lv_color_t surfaceHover()
{
  return lv_color_hex(0x222735);
}

inline lv_color_t primary()
{
  return lv_color_hex(0x8B5CF6);
}

inline lv_color_t primaryPressed()
{
  return lv_color_hex(0x6D28D9);
}

inline lv_color_t text()
{
  return lv_color_hex(0x131414);
}

inline lv_color_t textSecondary()
{
  return lv_color_hex(0x696b6b);
}

inline lv_color_t border()
{
  return lv_color_hex(0x292E3A);
}

inline lv_color_t error()
{
  return lv_color_hex(0xEF4444);
}

} // namespace app::ui::theme
