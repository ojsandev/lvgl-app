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

add_subdirectory(mocks)
add_subdirectory(common)
add_subdirectory(app)
add_subdirectory(data)
add_subdirectory(infra)
