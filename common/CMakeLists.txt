add_library(common STATIC
        logger/Logger.cpp
        logger/LoggerFactory.cpp
)

target_include_directories(
        common
        PUBLIC
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
        common
        PUBLIC
        Boost::random
)

target_compile_definitions(common
        PRIVATE
        $<$<CONFIG:Debug>:LOGGER_DEBUG>
)