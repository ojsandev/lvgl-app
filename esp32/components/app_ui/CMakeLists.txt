
set(REPO_ROOT "${PROJECT_DIR}/..")
set(APP_UI_DIR "${REPO_ROOT}/app/ui")

idf_component_register(
        SRCS
        "${APP_UI_DIR}/MainView.cpp"
        "${APP_UI_DIR}/MainViewModel.cpp"
        "${APP_UI_DIR}/MainViewUIState.cpp"
        "${APP_UI_DIR}/components/CharacterCard.cpp"
        "${APP_UI_DIR}/components/CharacterList.cpp"
        "${APP_UI_DIR}/theme/Styles.cpp"

        INCLUDE_DIRS
        "${REPO_ROOT}"
        "${REPO_ROOT}/app"
        "${APP_UI_DIR}"

        REQUIRES
        common
        display
        lv_cpp
        lvgl__lvgl
)
