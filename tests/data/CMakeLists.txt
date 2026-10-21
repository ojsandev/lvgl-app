add_project_test(
        http-exception-test
        http/HttpExceptionTest.cpp
)

target_link_libraries(http-exception-test
        PRIVATE
        data
)

add_project_test(
        async-repository-task-test
        repository/AsyncRepositoryFutureTest.cpp
)

target_link_libraries(async-repository-task-test
        PRIVATE
        data
        test-mocks
)
