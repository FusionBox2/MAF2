cmake_minimum_required(VERSION 3.20)

project(App)

set(PROJECT_HEADERS
    #MainFrame.h
    #MainWindow.h
    #Model.h
)

set(PROJECT_SOURCES
  wxApp.cpp
#  MainFrame.cpp
#	Sample.cpp
#    MainWnd.cpp
#    MainWindow.cpp
#    Model.cpp
#    NewMainWindow.cpp
#    main.cpp
)

if(WIN32)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${PROJECT_HEADERS} 
        wxApp.rc
    )
endif(WIN32)

#set(NO_MANIFEST 0)
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${PROJECT_SOURCES})

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST:NO")
endif(WIN32)
target_link_libraries(${PROJECT_NAME} wx::base wx::core wx::aui wx::html wx::propgrid libjson IO vtkMAF mafCommon mafCore mafGui mafOperations mafViews mafVME vtkMAF)
#target_link_libraries(${PROJECT_NAME} wx::base wx::core)
vtk_module_autoinit(
  TARGETS ${PROJECT_NAME}
  MODULES ${VTK_LIBRARIES})
