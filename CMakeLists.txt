cmake_minimum_required(VERSION 3.20)

project(
        my-lvgl-app
        VERSION 0.1.0
        LANGUAGES C CXX
)

# =========================================================
# Language
# =========================================================

if (MSVC)
    # CMake 4.4 does not expose CXX26 for the current MSVC
    # toolchain yet. MSVC uses /std:c++latest for C++26 mode.
    add_compile_options(/std:c++latest)
else()
    set(CMAKE_CXX_STANDARD 26)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

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

if (MSVC)
    # LVGL 9.5 can inherit C++26 from the parent project. CMake 4.4
    # does not recognize CXX26 for MSVC 19.51, so use C++20 as the
    # CMake dialect while /std:c++latest enables the latest MSVC mode.
    if (TARGET lvgl)
        set_property(TARGET lvgl PROPERTY CXX_STANDARD 20)
    endif()
    if (TARGET lvgl_thorvg)
        set_property(TARGET lvgl_thorvg PROPERTY CXX_STANDARD 20)
    endif()
endif()

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
# Restore C++26 for the application project on non-MSVC compilers.
if (MSVC)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
else()
    set(CMAKE_CXX_STANDARD 26)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

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
