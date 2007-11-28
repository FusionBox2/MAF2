#
# Copyright and description to be placed here
#
PROJECT(lhpCore)

# Set your list of sources here.
SET(PROJECT_SRCS
  lhpFactoryTagHandler.cpp
  lhpFactoryTagHandler.h
  )

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

