project(libjson CXX)

set(Header_Files
    include/libjson/allocator.h
    include/libjson/assertions.h
    include/libjson/config.h
    include/libjson/forwards.h
    include/libjson/historic_map.h
    include/libjson/json.h
    include/libjson/json_features.h
    include/libjson/reader.h
    include/libjson/value.h
    include/libjson/version.h
    include/libjson/writer.h
    source/json_tool.h
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    source/json_reader.cpp
    source/json_value.cpp
    source/json_writer.cpp
)
source_group("Source_Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

add_library(${PROJECT_NAME} ${ALL_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME};"
    "${CMAKE_CURRENT_SOURCE_DIR}/include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/source"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    "_CRT_SECURE_NO_WARNINGS;"
    "UNICODE;"
    "_UNICODE"
)
