add_library(data)

target_sources(
        data
        PRIVATE
        datasource/DemonSlayerDataSource.cpp
        mapper/CharacterMapper.cpp
        mapper/ErrorMapper.cpp
        repository/CharacterRepository.cpp
)

target_include_directories(
        data
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
        data
        PUBLIC
        domain
        common
        Boost::json
)