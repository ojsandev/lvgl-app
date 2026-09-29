cmake_minimum_required(VERSION 3.20)

project(
        my-lvl-app
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

# LVGL itself
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_USE_THORVG_INTERNAL ON CACHE BOOL "" FORCE)

# =========================================================
# Dependencies
# =========================================================

set(LIBS SDL2 libuuid kangaru)

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

# LVGL SDL backend requires SDL2.
target_link_libraries(
        lvgl
        PUBLIC
        SDL2::SDL2
)

# =========================================================
# aptumfr/lv - C++ wrapper
# =========================================================

# The wrapper enables these by default.
# We don't need its examples/demos/tests in our application.
set(LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(LV_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        lv_cpp
        GIT_REPOSITORY https://github.com/aptumfr/lv.git
        GIT_TAG e6b0adbd33b63d617334dc536245e5fe42c2518e
)

FetchContent_MakeAvailable(lv_cpp)

# aptumfr/lv currently sets CMAKE_CXX_STANDARD to C++20
# globally. Restore C++26 for our project.
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =========================================================
# Application
# =========================================================

add_subdirectory(app)
add_subdirectory(domain)

add_executable(my-lvl-app
        app/main.cpp
        app/App.cpp
        app/App.h
)

target_link_libraries(
        my-lvl-app
        PRIVATE
        app
        lv::lv
        libuuid::libuuid
        kangaru
)

target_compile_features(
        my-lvl-app
        PRIVATE
        cxx_std_26
)

target_compile_features(
        my-lvl-app
        PRIVATE
        cxx_std_26
)

# =========================================================
# Warnings
# =========================================================

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
            my-lvl-app
            PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif ()
