set(LV_CPP_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../.deps/lv_cpp-src")

if (NOT EXISTS "${LV_CPP_SOURCE_DIR}/include/lv/core/display.hpp")
  message(FATAL_ERROR "aptumfr/lv not found ${LV_CPP_SOURCE_DIR}")
endif ()

idf_component_register(INCLUDE_DIRS "${LV_CPP_SOURCE_DIR}/include" REQUIRES lvgl__lvgl)
