add_library(sdl_runtime STATIC
        App.cpp
        SDLDisplay.cpp
)

target_link_libraries(
        sdl_runtime
        PUBLIC
        app
        project_headers
        SDL2::SDL2
        lv::lv
)

add_library(my_lvgl::sdl_runtime ALIAS sdl_runtime)
