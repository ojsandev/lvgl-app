idf_component_register(
        SRCS
        "main.cpp"

        INCLUDE_DIRS
        "."

        REQUIRES
        display
        app
        demo_usecases
        lv_cpp
        lvgl__lvgl
)
