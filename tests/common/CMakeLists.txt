add_project_test(
        task-cxx-17-compatibility-test
        async/TaskCxx17CompatibilityTest.cpp
)

add_project_test(
        worker-pool-test
        async/WorkerPoolTest.cpp
)

target_link_libraries(task-cxx-17-compatibility-test PRIVATE common)
target_link_libraries(worker-pool-test PRIVATE common)
