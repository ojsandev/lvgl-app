add_executable(
        tls-test
        tls/TlsTest.cpp
)

target_link_libraries(
        tls-test
        PRIVATE
        infra
)

target_compile_features(
        tls-test
        PRIVATE
        cxx_std_26
)