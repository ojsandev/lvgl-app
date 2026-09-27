add_library(app
        ui/MainViewModel.cpp
        App.cpp
        App.h
        ui/MainView.cpp
        ui/MainView.h
)

target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(app PRIVATE domain lv::lv)
