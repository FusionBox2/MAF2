cmake_minimum_required(VERSION 3.27)

project(IO)

set(PROJECT_SRCS
	StorageElement.cpp
	StorageElement.h
	To.h
)

set(PROJECT_LIBS mafBase)

if(FTK_USE_XercesC)
  set(PROJECT_LIBS ${PROJECT_LIBS} XercesC::XercesC)
endif(FTK_USE_XercesC)

add_library(${PROJECT_NAME} ${PROJECT_SRCS})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})
