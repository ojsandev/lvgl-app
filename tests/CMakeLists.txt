if(BUILD_TESTING)
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
  target_compile_options(http-exception-test PRIVATE /W4)
  target_compile_options(async-repository-task-test PRIVATE /W4)
else()
  target_compile_options(http-exception-test PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(async-repository-task-test PRIVATE -Wall -Wextra -Wpedantic)
endif()
