cmake_minimum_required(VERSION 3.27)

set(TARGET_NAME ftkGui)

SET(TARGET_HEADERS
	IPropertyViewModel.h
	ITreeViewModel.h
	ITreeViewNode.h
	IViewModel.h
)

SET(TARGET_SOURCES
	IViewModel.cpp
)

add_library(${TARGET_NAME} ${TARGET_HEADERS} ${TARGET_SOURCES})
target_include_directories(${TARGET_NAME} PUBLIC ${FTK_SOURCE_DIR})
target_include_directories(${TARGET_NAME} PUBLIC ${FTK_BINARY_DIR})

target_include_directories(${TARGET_NAME} PUBLIC ${FTK_SOURCE_DIR}/Base)

if(MAF_USE_WX)
	add_subdirectory(wx)
endif(MAF_USE_WX)
