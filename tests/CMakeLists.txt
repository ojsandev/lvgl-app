add_executable(
        tls-test
        tls/TlsTest.cpp
)

target_link_libraries(
        tls-test
        PRIVATE
        infra
)
