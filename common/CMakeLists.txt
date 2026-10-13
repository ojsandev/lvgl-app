if(ESP_PLATFORM)
  idf_component_register(
          SRCS
          "logger/Logger.cpp"
          "logger/LoggerFactory.cpp"
          INCLUDE_DIRS
          "${CMAKE_CURRENT_LIST_DIR}/.."
          REQUIRES
          log
  )
  return()
endif()

add_library(common STATIC
        logger/Logger.cpp
        logger/LoggerFactory.cpp
)

target_link_libraries(
        common
        PUBLIC
        project_headers
        Boost::random
)

add_library(my_lvgl::common ALIAS common)

target_compile_definitions(common
        PRIVATE
        $<$<CONFIG:Debug>:LOGGER_DEBUG>
)
