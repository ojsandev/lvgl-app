cmake_minimum_required(VERSION 3.20)

project(
        my-lvgl-app
        VERSION 0.1.0
        LANGUAGES C CXX
)

# =========================================================
# Language
# =========================================================

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =========================================================
# LVGL configuration
# =========================================================

set(
        LV_BUILD_CONF_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h"
        CACHE PATH
        "Path to lv_conf.h"
)

set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_USE_THORVG_INTERNAL ON CACHE BOOL "" FORCE)

# =========================================================
# Dependencies
# =========================================================

set(LIBS SDL2 kangaru Boost OpenSSL)
foreach (lib ${LIBS})
    find_package(${lib} CONFIG REQUIRED)
endforeach ()

include(FetchContent)

# =========================================================
# LVGL
# =========================================================

FetchContent_Declare(
        lvgl
        GIT_REPOSITORY https://github.com/lvgl/lvgl.git
        GIT_TAG v9.5.0
)

FetchContent_MakeAvailable(lvgl)

target_link_libraries(
        lvgl
        PUBLIC
        SDL2::SDL2
)

# =========================================================
# aptumfr/lv - C++ wrapper
# =========================================================

set(LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(LV_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        lv_cpp
        GIT_REPOSITORY https://github.com/aptumfr/lv.git
        GIT_TAG e6b0adbd33b63d617334dc536245e5fe42c2518e
)

FetchContent_MakeAvailable(lv_cpp)

# aptumfr/lv currently changes the global standard to C++20.
# Restore C++26 for the application project.
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =========================================================
# Project modules
# =========================================================

add_subdirectory(domain)
add_subdirectory(data)
add_subdirectory(infra)
add_subdirectory(app)
add_subdirectory(tests)

# =========================================================
# Application
# =========================================================

add_executable(
        my-lvgl-app
        app/main.cpp
        app/App.cpp
        app/App.h
        tests/tls/TlsTest.cpp
)

target_link_libraries(
        my-lvgl-app
        PRIVATE
          app
          data
          infra
          lv::lv
          kangaru
)

set_property(TARGET my-lvgl-app PROPERTY CXX_STANDARD 26)
set_property(TARGET my-lvgl-app PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET my-lvgl-app PROPERTY CXX_EXTENSIONS OFF)

# =========================================================
# Warnings
# =========================================================

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(
          my-lvgl-app
          PRIVATE
          -Wall
          -Wextra
          -Wpedantic
  )
endif()
