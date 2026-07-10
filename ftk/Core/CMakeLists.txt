cmake_minimum_required(VERSION 3.27)

set(TARGET_NAME ftkCore)

SET(TARGET_HEADERS
	FilePath.h
	IPropertiesBuilder.h
	IProperty.h
	Operation.h
	OperationChangeProperty.h
	OperationManager.h
	OperationProperty.h
	WithProperties.h
)

SET(TARGET_SOURCES
	FilePath.cpp
	IPropertiesBuilder.cpp
	Operation.cpp
	OperationChangeProperty.cpp
	OperationManager.cpp
	OperationProperty.cpp
	WithProperties.cpp
)

add_library(${TARGET_NAME} ${TARGET_HEADERS} ${TARGET_SOURCES})
target_include_directories(${TARGET_NAME} PUBLIC ${FTK_SOURCE_DIR})
target_include_directories(${TARGET_NAME} PUBLIC ${FTK_BINARY_DIR})
