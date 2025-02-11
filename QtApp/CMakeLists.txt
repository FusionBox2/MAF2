cmake_minimum_required(VERSION 3.20)

project(QtApp)

set(PROJECT_SRCS
    main.cpp
)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${PROJECT_SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${Qt6Widgets_LIBRARIES} ${VTK_LIBRARIES} vtkMAF)

vtk_module_autoinit(
  TARGETS ${PROJECT_NAME}
  MODULES ${VTK_LIBRARIES})

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND echo [Paths] > $<TARGET_FILE_DIR:${PROJECT_NAME}>/qt.conf
        COMMAND echo Prefix=${Qt6_DIR}/../../../ >> $<TARGET_FILE_DIR:${PROJECT_NAME}>/qt.conf
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND echo @echo off > $<TARGET_FILE_DIR:${PROJECT_NAME}>/start.bat
        COMMAND echo SET PATH=${Qt6_DIR}/../../../bin$<SEMICOLON>%%%%PATH%%%% >> $<TARGET_FILE_DIR:${PROJECT_NAME}>/start.bat
        COMMAND echo qtApp >> $<TARGET_FILE_DIR:${PROJECT_NAME}>/start.bat
    )
endif(WIN32)

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set_target_properties(${PROJECT_NAME} PROPERTIES
                          VS_DEBUGGER_ENVIRONMENT "PATH=${Qt6_DIR}/../../../bin;$(PATH)")
endif(CMAKE_GENERATOR MATCHES "Visual Studio")
