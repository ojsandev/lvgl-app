idf_component_register(
        SRCS
        "main.cpp"

        INCLUDE_DIRS
        "."

        REQUIRES
        display
        app_ui
        lv_cpp
        lvgl__lvgl
)