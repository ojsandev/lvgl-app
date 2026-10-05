add_library(infra)

target_sources(
        infra
        PRIVATE
        http/BoostBeastHttpClient.cpp
        tls/CertificateStoreFactory.cpp
        http/exception/HttpException.cpp
)

if(APPLE)
  target_sources(
          infra
          PRIVATE
          tls/MacOSCertificateStore.cpp
  )
endif()

if(WIN32)
  target_compile_definitions(
          infra
          PRIVATE
          _WIN32_WINNT=0x0A00
          WINVER=0x0A00
  )

  target_sources(
          infra
          PRIVATE
          tls/WindowsCertificateStore.cpp
  )

  target_link_libraries(
          infra
          PRIVATE
          Crypt32
  )
endif()

if(UNIX AND NOT APPLE)
  target_sources(
          infra
          PRIVATE
          tls/LinuxCertificateStore.cpp
  )
endif()

target_include_directories(
        infra
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
        infra
        PUBLIC
        data
        Boost::boost
        Boost::url
        openssl::openssl
)
