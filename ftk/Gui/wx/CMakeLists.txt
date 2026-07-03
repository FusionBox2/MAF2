cmake_minimum_required(VERSION 3.27)

set(TARGET_NAME ftkGuiWx)

SET(TARGET_HEADERS
	IView.h
)

SET(TARGET_SOURCES
	IView.cpp
)

add_library(${TARGET_NAME} ${TARGET_HEADERS} ${TARGET_SOURCES})
target_include_directories(${TARGET_NAME} PUBLIC ${FTK_SOURCE_DIR})
target_include_directories(${TARGET_NAME} PUBLIC ${FTK_BINARY_DIR})
target_link_libraries(${TARGET_NAME} ftkGui)
target_link_libraries(${TARGET_NAME} wx::base wx::core)
