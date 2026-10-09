
#pragma once

#include <SDL_events.h>
#include <cstdint>

#include "display/IDisplay.h"
#include "lv/core/display.hpp"

struct SDL_Window;

namespace infra::display {

class SDLDisplay final : public IDisplay {
public:
  using EventCallback = int (*)(void*, SDL_Event*);

  SDLDisplay(common::types::Int32 width, common::types::Int32 height);
  ~SDLDisplay() override = default;

  SDLDisplay(const SDLDisplay&) = delete;
  SDLDisplay& operator=(const SDLDisplay&) = delete;

  [[nodiscard]] lv::Display& lvgl() noexcept override;
  [[nodiscard]] const lv::Display& lvgl() const noexcept override;

  [[nodiscard]] SDL_Window* nativeWindow() const noexcept;

  void raise() const noexcept;
  void resizable(bool enabled) const noexcept;
  void minimumSize(common::types::Int32 width, common::types::Int32 height) const noexcept;

  void addEventWatch(EventCallback callback, void* userdata);
  void removeEventWatch(EventCallback callback, void* userdata) noexcept;

private:
  lv::SDLDisplay m_display;
  SDL_Window* m_window = nullptr;
};

} // namespace infra::display
