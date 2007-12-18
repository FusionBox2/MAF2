#
# Copyright and description to be placed here
#
PROJECT(lhpCore)

# Set your list of sources here.
 SET(PROJECT_SRCS
	# need to solve some linking problems: for the moment moved save to operations
  # lhpFactoryTagHandler.cpp
  # lhpFactoryTagHandler.h
  lhpUser.cpp
  lhpUser.h
 )

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

