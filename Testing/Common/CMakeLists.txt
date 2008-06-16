PROJECT (VME_TEST)

LINK_LIBRARIES(lhpVME)
LINK_LIBRARIES(lhpCommon)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

# ADD_EXECUTABLE(myTest myTest.h myTest.cpp)
# ADD_TEST(myTest ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH}/myTest)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
# IF (MAF_USE_VTK)
# ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)