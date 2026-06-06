cmake_minimum_required(VERSION 3.20)

project(App)

set(PROJECT_HEADERS
    #MainFrame.h
    #MainWindow.h
    #Model.h
tree.hpp
)

set(PROJECT_SOURCES
#tree.cpp
#  wxApp.cpp
  MainFrame.cpp
#	Sample.cpp
#    MainWnd.cpp
#    MainWindow.cpp
#    Model.cpp
#    NewMainWindow.cpp
    main.cpp
)

if(WIN32)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${PROJECT_HEADERS} 
        wxApp.rc
    )
endif(WIN32)

set(Boost_DIR ${BST_DIR})
find_package(Boost REQUIRED COMPONENTS filesystem CONFIG)

#set(NO_MANIFEST 0)
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${PROJECT_SOURCES})

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST:NO")
endif(WIN32)
target_include_directories(${PROJECT_NAME} PRIVATE ${FTK_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${FTK_BINARY_DIR})
target_link_directories(${PROJECT_NAME} PRIVATE D:/dev/FTKBuild/Install/boost/lib)
target_link_libraries(${PROJECT_NAME} wx::base wx::core wx::aui wx::html wx::propgrid IO mafBase mafCore ${Boost_LIBRARIES})# mafCommon mafCore mafGui mafOperations mafViews mafVME)
#target_link_libraries(${PROJECT_NAME} wx::base wx::core)
vtk_module_autoinit(
  TARGETS ${PROJECT_NAME}
  MODULES ${VTK_LIBRARIES})

