cmake_minimum_required(VERSION 3.27)

set(TARGET_NAME ftkBase)

SET(TARGET_HEADERS
	Connection.h
	FastBimap.h
	FastPImpl.h
	Meta.h
	Signal.h
	SpinLock.h
	String.h
	ThreadPool.h
)

SET(TARGET_SOURCES
	Connection.cpp
	String.cpp
)

add_library(${TARGET_NAME} ${TARGET_HEADERS} ${TARGET_SOURCES})
target_include_directories(${TARGET_NAME} PUBLIC ${FTK_SOURCE_DIR})
target_include_directories(${TARGET_NAME} PUBLIC ${FTK_BINARY_DIR})
