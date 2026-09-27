add_library(domain STATIC usecase/UpdateTitleUseCase.cpp)

target_include_directories(domain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})