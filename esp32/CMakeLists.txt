cmake_minimum_required(VERSION 3.16)

include(FetchContent)

# Register shared modules as first-class IDF components. Their CMake files
# select the ESP-IDF branch through ESP_PLATFORM, so sources are owned by the
# module instead of being listed from a bridge component under esp32/.
set(
  EXTRA_COMPONENT_DIRS
  "${CMAKE_CURRENT_LIST_DIR}/../common"
  "${CMAKE_CURRENT_LIST_DIR}/../domain"
  "${CMAKE_CURRENT_LIST_DIR}/../app"
  CACHE STRING
  "Shared project ESP-IDF components"
)

# =========================================================
# C++ wrapper: aptumfr/lv
# =========================================================

set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}/.deps" CACHE PATH
  "Directory for ESP32 FetchContent dependencies" FORCE
)

FetchContent_Declare(
  lv_cpp
  GIT_REPOSITORY https://github.com/aptumfr/lv.git
  GIT_TAG e6b0adbd33b63d617334dc536245e5fe42c2518e
  SOURCE_SUBDIR include
)

FetchContent_MakeAvailable(lv_cpp)

# =========================================================
# ESP-IDF
# =========================================================

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(my-lvgl-esp32)
