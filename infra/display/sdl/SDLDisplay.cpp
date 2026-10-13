
#include "SDLDisplay.h"

#include <SDL_video.h>

using namespace infra::display;

SDLDisplay::SDLDisplay(const common::types::Int32 width, const common::types::Int32 height)
  : m_display(width, height)
  , m_window(lv_sdl_window_get_window(m_display.get()))
{}

lv::Display& SDLDisplay::lvgl() noexcept
{
  return m_display;
}

const lv::Display& SDLDisplay::lvgl() const noexcept
{
  return m_display;
}

SDL_Window* SDLDisplay::nativeWindow() const noexcept
{
  return m_window;
}

void SDLDisplay::raise() const noexcept
{
  if (m_window == nullptr) {
    return;
  }

  SDL_RaiseWindow(m_window);
}

void SDLDisplay::resizable(const bool enabled) const noexcept
{
  if (m_window == nullptr) {
    return;
  }

  SDL_SetWindowResizable(m_window, enabled ? SDL_TRUE : SDL_FALSE);
}

void SDLDisplay::minimumSize(const common::types::Int32 width, const common::types::Int32 height) const noexcept
{
  if (m_window == nullptr) {
    return;
  }

  SDL_SetWindowMinimumSize(m_window, width, height);
}

void SDLDisplay::addEventWatch(const EventCallback callback, void* userdata)
{
  SDL_AddEventWatch(callback, userdata);
}

void SDLDisplay::removeEventWatch(const EventCallback callback, void* userdata) noexcept
{
  SDL_DelEventWatch(callback, userdata);
}
