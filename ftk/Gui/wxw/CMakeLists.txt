cmake_minimum_required(VERSION 3.27)

PROJECT(wxWidgetsGui)

CONFIGURE_FILE("${FTK_SOURCE_DIR}/Gui/manifest.in" "${FTK_BINARY_DIR}/Gui/manifest.in" COPYONLY)

SET(PROJECT_SRCS
  MainFrame.h
)

add_library(${PROJECT_NAME} ${PROJECT_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_include_directories(${PROJECT_NAME} PRIVATE ${FTK_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${FTK_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} wx::base wx::core wx::aui)
