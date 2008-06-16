PROJECT (OPERATIONS_TEST)

LINK_LIBRARIES (lhpVME)
LINK_LIBRARIES(lhpOperations C3D_Reader)
LINK_LIBRARIES(lhpCommon)

#-----------------------------------------------------------------------------
# tests using cppunit testing framework
#-----------------------------------------------------------------------------

# ADD_EXECUTABLE(myTest myTest.h myTest.cpp)
# ADD_TEST(myTest ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH}/myTest)

ADD_EXECUTABLE(lhpOpMultiscaleExploreTest lhpOpMultiscaleExploreTest.h lhpOpMultiscaleExploreTest.cpp)
ADD_TEST(lhpOpMultiscaleExploreTest ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH}/lhpOpMultiscaleExploreTest)

ADD_EXECUTABLE(lhpFactoryTagHandlerTest lhpFactoryTagHandlerTest.h lhpFactoryTagHandlerTest.cpp)
ADD_TEST(lhpFactoryTagHandlerTest ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH}/lhpFactoryTagHandlerTest)

# ADD_EXECUTABLE(lhpTagHandlerContainerTest lhpTagHandlerContainerTest.h lhpTagHandlerContainerTest.cpp)
# ADD_TEST(lhpTagHandlerContainerTest ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH}/lhpTagHandlerContainerTest)

ADD_EXECUTABLE(lhpTagHandlerDICOMContainerTest lhpTagHandlerDICOMContainerTest.h lhpTagHandlerDICOMContainerTest.cpp)
ADD_TEST(lhpTagHandlerDICOMContainerTest ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH}/lhpTagHandlerDICOMContainerTest)


ADD_EXECUTABLE(lhpOpImporterC3DTest lhpOpImporterC3DTest.h lhpOpImporterC3DTest.cpp)
ADD_TEST(lhpOpImporterC3DTest ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH}/lhpOpImporterC3DTest)

ADD_EXECUTABLE(lhpOpBonematTest lhpOpBonematTest.h lhpOpBonematTest.cpp)
ADD_TEST(lhpOpBonematTest ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH}/lhpOpBonematTest)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
# IF (MAF_USE_VTK)
# ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)