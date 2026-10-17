if(ESP_PLATFORM)
  idf_component_register(
          SRCS
          "ui/MainViewModel.cpp"
          "ui/MainView.cpp"
          "ui/MainViewUIState.cpp"
          "ui/components/CharacterCard.cpp"
          "ui/components/CharacterList.cpp"
          "ui/theme/Styles.cpp"
          INCLUDE_DIRS
          "${CMAKE_CURRENT_LIST_DIR}/.."
          "${CMAKE_CURRENT_LIST_DIR}"
          REQUIRES
          common
          domain
          lv_cpp
          lvgl__lvgl
  )
  return()
endif()

add_library(app
        ui/MainViewModel.cpp
        ui/MainView.cpp
        ui/MainViewUIState.cpp
        ui/components/CharacterCard.cpp
        ui/components/CharacterList.cpp
        ui/theme/Styles.cpp
)

target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
        app
        PUBLIC
        domain
        lv::lv
        project_headers
)

add_library(my_lvgl::ui ALIAS app)
