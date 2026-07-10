cmake_minimum_required(VERSION 3.27)

set(TARGET_NAME ftkIO)

SET(TARGET_HEADERS
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

SET(TARGET_SOURCES
	StorageElement.cpp
	StorageElement.h
	JsonRapid.cpp
	XmlRapid.cpp
	XmlXerces.cpp
)

set(PROJECT_LIBS mafBase)

add_library(${TARGET_NAME} ${TARGET_HEADERS} ${TARGET_SOURCES})
target_include_directories(${TARGET_NAME} PRIVATE ${FTK_SOURCE_DIR})
target_include_directories(${TARGET_NAME} PRIVATE ${FTK_BINARY_DIR})
target_link_libraries(${TARGET_NAME} mafBase)

if(FTK_USE_XercesC)
	target_link_libraries(${TARGET_NAME} XercesC::XercesC)
endif(FTK_USE_XercesC)

if(FTK_USE_LIBJSON)
	target_link_libraries(${TARGET_NAME} libjson)
endif(FTK_USE_LIBJSON)
