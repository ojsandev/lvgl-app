add_executable(
        tls-test
        tls/TlsTest.cpp
)

target_link_libraries(
        tls-test
        PRIVATE
        infra
)

if(BUILD_TESTING)
  add_test(NAME tls-test COMMAND tls-test)
  set_tests_properties(tls-test PROPERTIES LABELS "integration;network" TIMEOUT 30)

  add_executable(
          http-exception-test
          core/HttpExceptionTest.cpp
  )
  target_link_libraries(http-exception-test PRIVATE data)
  add_test(NAME http-exception-test COMMAND http-exception-test)

  add_executable(
          async-repository-task-test
          core/AsyncRepositoryFutureTest.cpp
  )
  target_link_libraries(async-repository-task-test PRIVATE data)
  add_test(NAME async-repository-task-test COMMAND async-repository-task-test)
endif()

if (MSVC)
    set_property(TARGET tls-test PROPERTY CXX_STANDARD 20)
else()
    set_property(TARGET tls-test PROPERTY CXX_STANDARD 26)
endif()
set_property(TARGET tls-test PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET tls-test PROPERTY CXX_EXTENSIONS OFF)
