add_project_test(
        async-usecase-and-viewmodel-test
        AsyncUseCaseAndViewModelTest.cpp
)

target_link_libraries(
        async-usecase-and-viewmodel-test
        PRIVATE
        app
        domain
        common
        test-mocks
)
