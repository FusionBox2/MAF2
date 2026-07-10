cmake_minimum_required(VERSION 3.27)

set(TARGET_NAME ftkGuiWx)

SET(TARGET_HEADERS
	IOldGuiEditor.h
	IPropertyEditor.h
	IView.h
	OldGuiAdapter.h
	OldGuiView.h
	PaneFrame.h
	PropertyAdapter.h
	PropertyView.h
	TreeAdapter.h
	TreeView.h)

SET(TARGET_SOURCES
	IPropertyEditor.cpp
	IView.cpp
	OldGuiAdapter.cpp
	OldGuiView.cpp
	PropertyAdapter.cpp
	PropertyView.cpp
	TreeAdapter.cpp
	TreeView.cpp
)

add_library(${TARGET_NAME} ${TARGET_HEADERS} ${TARGET_SOURCES})
target_include_directories(${TARGET_NAME} PUBLIC ${FTK_SOURCE_DIR})
target_include_directories(${TARGET_NAME} PUBLIC ${FTK_BINARY_DIR})
target_link_libraries(${TARGET_NAME} ftkGui)
target_link_libraries(${TARGET_NAME} wx::base wx::core)
