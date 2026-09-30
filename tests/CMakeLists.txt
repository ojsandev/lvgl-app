add_executable(
        tls-test
        tls/TlsTest.cpp
)

target_link_libraries(
        tls-test
        PRIVATE
        infra
)

set_property(TARGET tls-test PROPERTY CXX_STANDARD 26)
set_property(TARGET tls-test PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET tls-test PROPERTY CXX_EXTENSIONS OFF)
