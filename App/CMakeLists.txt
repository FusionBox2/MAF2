cmake_minimum_required(VERSION 3.27)

set(TARGET_NAME A)

set(A_HEADERS
	AppFrame.h
	Document.h
	DocumentContext.h
	DocumentManager.h
	DocumentSelection.h
	ISelectionController.h
	IStatusController.h
	IVTKViewModel.h
	IVTKViewNode.h
	OperationCreateT.h
	OperationCreateFactory.h
	OperationsRegistry.h
	OpReparentNode.h
	PropertyEditorFactory.h
	PropertyViewModel.h
	Resource.h
	ResourceId.h
	ResourceInfo.h
	ResourceManager.h
	TreeStatusViewModel.h
	TreeViewModel.h
	ViewManager.h
	VisibilityStatusController.h
	VTKAdapter.h
	VTKView.h
	VTKViewModel.h
)

set(A_SOURCES
	App.cpp
	Document.cpp
	DocumentContext.cpp
	DocumentManager.cpp
	DocumentSelection.cpp
	OldApp.cpp
	OperationCreateFactory.cpp
	OperationsRegistry.cpp
	OpReparentNode.cpp
	PropertyEditorFactory.cpp
	PropertyViewModel.cpp
	Resource.cpp
	ResourceId.cpp
	ResourceManager.cpp
	ViewManager.cpp
	VisibilityStatusController.cpp
	TreeStatusViewModel.cpp
	TreeViewModel.cpp
	VTKAdapter.cpp
	VTKView.cpp
	VTKViewModel.cpp
)

if(WIN32)
	SET(NO_WX_MANIFEST 0)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/App.rc.in ${CMAKE_CURRENT_BINARY_DIR}/App.rc @ONLY)
	set(A_SOURCES ${A_SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/App.rc")
endif(WIN32)


add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE  ${A_SOURCES} ${A_HEADERS})

if(WIN32)
	if(NO_WX_MANIFEST EQUAL 1)
		set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
	else(NO_WX_MANIFEST EQUAL 1)
		set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/MANIFEST:NO")
	endif(NO_WX_MANIFEST EQUAL 1)
endif(WIN32)


#set(Boost_DIR ${BST_DIR})
#find_package(Boost REQUIRED COMPONENTS filesystem CONFIG)

target_include_directories(${TARGET_NAME} PRIVATE ${FTK_SOURCE_DIR})
target_include_directories(${TARGET_NAME} PRIVATE ${FTK_BINARY_DIR})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
#target_link_directories(${TARGET_NAME} PRIVATE D:/dev/FTKBuild/Install/boost/lib)
#target_link_libraries(${TARGET_NAME} wx::base wx::core wx::aui wx::html wx::propgrid ftkIO mafBase mafCore ${Boost_LIBRARIES})# mafCommon mafCore mafGui mafOperations mafViews mafVME)
target_link_libraries(${TARGET_NAME} wx::base wx::core)
target_link_libraries(${TARGET_NAME} mafBase mafCore mafCommon mafGui mafOperations mafViews mafVME ftkIO ftkBase ftkCore ftkGuiWx)

vtk_module_autoinit(
	TARGETS ${TARGET_NAME}
	MODULES ${VTK_LIBRARIES})
