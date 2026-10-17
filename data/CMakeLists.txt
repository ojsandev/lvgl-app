add_library(data)

target_sources(
        data
        PRIVATE
        datasource/DemonSlayerDataSource.cpp
        http/HttpException.cpp
        mapper/CharacterMapper.cpp
        mapper/ErrorMapper.cpp
        repository/CharacterRepository.cpp
        repository/image/CharacterImageRepository.cpp
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

add_library(my_lvgl::data ALIAS data)
