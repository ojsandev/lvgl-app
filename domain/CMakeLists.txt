if(ESP_PLATFORM)
  # The firmware currently consumes only domain contracts and models. Desktop
  # use-case implementations remain out of the ESP-IDF graph until their
  # filesystem and HTTP dependencies are replaced with embedded adapters.
  idf_component_register(
          INCLUDE_DIRS
          "${CMAKE_CURRENT_LIST_DIR}/.."
          REQUIRES
          common
  )
  return()
endif()

add_library(domain STATIC
        usecase/UpdateTitleUseCase.cpp
        usecase/characters/GetCharactersUseCase.cpp
)

target_link_libraries(domain
        PUBLIC
        common
)

add_library(my_lvgl::domain ALIAS domain)
