#
# Copyright and description to be placed here
#

PROJECT(lhpCommon)

# Set your list of sources here.
SET(PROJECT_SRCS
mafPlotMath.cpp
mafPlotMath.h
)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})

