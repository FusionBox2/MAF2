#
# Copyright and description to be placed here
#

PROJECT(lhpCommon)

# Set your list of sources here.
SET(PROJECT_SRCS
arrays.h
bbasis.h
brent.cpp
covsrt.cpp
createSplineSurf.h
f1dim.cpp
frprmn.cpp
gaussj.cpp
lfit.cpp
linmin.cpp
lubksb.cpp
ludcmp.cpp
mnbrak.cpp
nr.h
nrtypes.h
nrtypes_nr.h
nrutil.h
nrutil_nr.h
polynom.h
powell.cpp
pythag.cpp
svdcmp.cpp
splineobjs.h
mafJointAnalysis.cpp
mafJointAnalysis.h
mafPlotMath.cpp
mafPlotMath.h
forarray.h
VecProc.h
splines.h
lhpUtils.h
lhpUtils.cpp
)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})

