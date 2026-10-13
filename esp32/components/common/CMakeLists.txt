set(REPO_ROOT "${PROJECT_DIR}/..")

idf_component_register(
        SRCS
        "${REPO_ROOT}/common/logger/Logger.cpp"
        "${REPO_ROOT}/common/logger/LoggerFactory.cpp"

        INCLUDE_DIRS
        "${REPO_ROOT}"

        REQUIRES
        log
)
