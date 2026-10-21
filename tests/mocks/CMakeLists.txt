add_library(test-mocks INTERFACE)

target_include_directories(test-mocks
        INTERFACE
        ${PROJECT_SOURCE_DIR}/tests/mocks
)

target_link_libraries(test-mocks
        INTERFACE
        GTest::gmock
)