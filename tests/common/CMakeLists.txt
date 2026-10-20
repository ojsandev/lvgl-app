add_project_test(
        task-cxx-17-compatibility-test
        async/TaskCxx17CompatibilityTest.cpp
)

target_link_libraries(task-cxx-17-compatibility-test
        PRIVATE
        common
)