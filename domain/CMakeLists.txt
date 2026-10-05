add_library(domain STATIC
        usecase/UpdateTitleUseCase.cpp
        usecase/characters/GetCharactersUseCase.cpp
)

target_link_libraries(domain
        PUBLIC
        common
)