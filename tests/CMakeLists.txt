include(GoogleTest)

function(add_project_test target source)
  add_executable(${target} ${source})

  target_link_libraries(
          ${target}
          PRIVATE
          GTest::gmock
          GTest::gtest_main
  )

  gtest_discover_tests(${target})
endfunction()
  add_executable(
          task-cxx17-compatibility-test
          core/TaskCxx17CompatibilityTest.cpp
  )
  target_include_directories(task-cxx17-compatibility-test PRIVATE ${CMAKE_SOURCE_DIR})
  set_property(TARGET task-cxx17-compatibility-test PROPERTY CXX_STANDARD 17)
  set_property(TARGET task-cxx17-compatibility-test PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET task-cxx17-compatibility-test PROPERTY CXX_EXTENSIONS OFF)
  add_test(NAME task-cxx17-compatibility-test COMMAND task-cxx17-compatibility-test)

  add_executable(
          worker-pool-test
          core/WorkerPoolTest.cpp
  )
  target_link_libraries(worker-pool-test PRIVATE common)
  add_test(NAME worker-pool-test COMMAND worker-pool-test)
endif()

add_subdirectory(mocks)
add_subdirectory(common)
add_subdirectory(app)
add_subdirectory(data)
add_subdirectory(infra)
