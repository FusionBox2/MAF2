#
# Copyright and description to be placed here
#

PROJECT(lhpCommon)

# Set your list of sources here.
SET(PROJECT_SRCS
mafJointAnalysis.cpp
mafJointAnalysis.h
mafPlotMath.cpp
mafPlotMath.h
forarray.h
VecProc.h
vectors.h
splines.h
lhpUtils.h
lhpUtils.cpp
)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})

