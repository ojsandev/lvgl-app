add_library(domain STATIC
        usecase/UpdateTitleUseCase.cpp
        usecase/GetCharactersUseCase.cpp
)

target_link_libraries(domain
        PUBLIC
        common
)