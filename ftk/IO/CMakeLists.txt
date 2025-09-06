cmake_minimum_required(VERSION 3.27)

project(IO)

set(PROJECT_HEADERS
    Parse.h
    ParseContainers.h
    ParseTo.h
    Serialize.h
    SerializeContainers.h
    SerializeTo.h
    JsonRapid.h
    XmlRapid.h
    XmlXerces.h
)

set(PROJECT_SOURCES
	StorageElement.cpp
	StorageElement.h
    JsonRapid.cpp
    XmlRapid.cpp
    XmlXerces.cpp
)

set(PROJECT_LIBS mafBase)

if(FTK_USE_XercesC)
  set(PROJECT_LIBS ${PROJECT_LIBS} XercesC::XercesC libjson)
endif(FTK_USE_XercesC)

add_library(${PROJECT_NAME} ${PROJECT_HEADERS} ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_include_directories(${PROJECT_NAME} PRIVATE ${FTK_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${FTK_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBS})
