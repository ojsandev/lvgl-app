add_executable(
        tls-test
        tls/TlsTest.cpp
)

target_link_libraries(
        tls-test
        PRIVATE
        infra
)

add_test(NAME tls-test COMMAND tls-test)

set_tests_properties(
        tls-test
        PROPERTIES
        LABELS "integration;network"
        TIMEOUT 30
        RUN_SERIAL TRUE
)