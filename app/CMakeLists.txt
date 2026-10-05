add_library(app
        ui/MainViewModel.cpp
        ui/MainView.cpp
        ui/MainViewUIState.cpp
        App.cpp
)

target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(app PRIVATE domain lv::lv)
