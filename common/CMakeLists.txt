add_library(common INTERFACE)

target_include_directories(common
        INTERFACE
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
        common
        INTERFACE
        Boost::random
)