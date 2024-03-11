PROJECT (webServicesClient_TEST)

MESSAGE(STATUS "entering ${CMAKE_CURRENT_SOURCE_DIR}")

OPTION(BUILD_VMEUPLOADERDOWNLOADER_TESTING "Enable building of testing for vme uploader downloader" ON)
IF (BUILD_VMEUPLOADERDOWNLOADER_TESTING)

    MESSAGE(STATUS "building tests")

    # MESSAGE("variable LHP_BINARY_DIR = " ${LHP_BINARY_DIR})

    SET (WEB_SERVICES_CLIENT_DIR ${VME_UPLOADER_DOWNLOADER_DIR}/webServicesClient)
    # ADD_TEST("PythonWorks" "python.exe" "-c" "print 'hello world'")

    ADD_TEST("BasketServicesTest" "python.exe" "${WEB_SERVICES_CLIENT_DIR}/BasketServicesTest.py")
    ADD_TEST("MtomDownloadTest" "python.exe" "${WEB_SERVICES_CLIENT_DIR}/MtomDownloadTest.py")
    ADD_TEST("MtomSRBSizeTest" "python.exe" "${WEB_SERVICES_CLIENT_DIR}/MtomSRBSizeTest.py")
    ADD_TEST("MtomUploadTest" "python.exe" "${WEB_SERVICES_CLIENT_DIR}/MtomUploadTest.py")
    ADD_TEST("MtomUploadURITest" "python.exe" "${WEB_SERVICES_CLIENT_DIR}/MtomUploadURITest.py")
    ADD_TEST("XmlUploadDownloadTest" "python.exe" "${WEB_SERVICES_CLIENT_DIR}/XmlUploadDownloadTest.py")
    
ELSE (BUILD_VMEUPLOADERDOWNLOADER_TESTING)
    
    MESSAGE(STATUS "not building tests")
    
ENDIF (BUILD_VMEUPLOADERDOWNLOADER_TESTING)