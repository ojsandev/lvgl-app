add_project_test(
        task-cxx-17-compatibility-test
        async/TaskCxx17CompatibilityTest.cpp
)

# This target intentionally verifies that Task.h remains usable from C++17.
set_target_properties(
        task-cxx-17-compatibility-test
        PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

add_project_test(
        worker-pool-test
        async/WorkerPoolTest.cpp
)

target_link_libraries(task-cxx-17-compatibility-test PRIVATE common)
target_link_libraries(worker-pool-test PRIVATE common)
