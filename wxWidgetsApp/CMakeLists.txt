cmake_minimum_required(VERSION 3.27)

project(wxWidgetsApp)

set(PROJECT_HEADERS
	MainWindow.h
)

set(PROJECT_SOURCES
	MainWindow.cpp
	main.cpp
)

if(WIN32)
	set(PROJECT_SOURCES ${PROJECT_SOURCES} ${PROJECT_HEADERS} 
		wxWidgetsApp.rc
	)
endif(WIN32)

#set(NO_MANIFEST 0)
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${PROJECT_SOURCES})

if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST:NO")
endif(WIN32)
target_include_directories(${PROJECT_NAME} PRIVATE ${FTK_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${FTK_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} wx::base wx::core wx::aui mafBase)
