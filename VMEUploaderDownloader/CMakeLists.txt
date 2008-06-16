PROJECT (lhpVMEUPloaderDownloader_TEST)

MESSAGE(STATUS "entering ${CMAKE_CURRENT_SOURCE_DIR}")

OPTION(BUILD_VMEUPLOADERDOWNLOADER_TESTING "Enable building of testing for vme uploader downloader" ON)
IF (BUILD_VMEUPLOADERDOWNLOADER_TESTING)

    MESSAGE(STATUS "building tests")

    # MESSAGE("variable LHP_BINARY_DIR = " ${LHP_BINARY_DIR})

    SET (VME_UPLOADER_DOWNLOADER_DIR ${LHP_BINARY_DIR}/VMEUploaderDownloader)
    ADD_TEST("PythonWorks" "python.exe" "-c" "print 'hello world'")

    ADD_TEST("dictionaryCSV2XMLTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/dictionaryCSV2XMLTest.py")
    ADD_TEST("DownloadHandlerTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/DownloadHandlerTest.py")
    ADD_TEST("downloadSingleXMLTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/downloadSingleXMLTest.py")
    ADD_TEST("lhpAuthenticationControlTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/lhpAuthenticationControlTest.py")
    ADD_TEST("lhpbDictionaryTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/lhpbDictionaryTest.py")
    ADD_TEST("lhpCSVDictionaryDownloaderTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/lhpCSVDictionaryDownloaderTest.py")
    ADD_TEST("lhpDefinesTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/lhpDefinesTest.py")
    ADD_TEST("lhpDictionaryVersionCheckerTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/lhpDictionaryVersionCheckerTest.py")
    ADD_TEST("lhpEditVMETagTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/lhpEditVMETagTest.py")
    ADD_TEST("lhpXMLDictionariesBuilderTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/lhpXMLDictionariesBuilderTest.py")
    ADD_TEST("lhpXMLDictionaryParserTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/lhpXMLDictionaryParserTest.py")
    ADD_TEST("listBasketTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/listBasketTest.py")
    ADD_TEST("msfParserTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/msfParserTest.py")
    ADD_TEST("msfReconstructorTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/msfReconstructorTest.py")
    ADD_TEST("ServerTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/ServerTest.py")
    ADD_TEST("UploadHandlerTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/UploadHandlerTest.py")
    ADD_TEST("vmeDownloaderTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/vmeDownloaderTest.py")
    ADD_TEST("vmeUploaderDownloaderTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/vmeUploaderDownloaderTest.py")
    ADD_TEST("vmeUploaderTest" "python.exe" "${VME_UPLOADER_DOWNLOADER_DIR}/vmeUploaderTest.py")   
    
ELSE (BUILD_VMEUPLOADERDOWNLOADER_TESTING)
    
    MESSAGE(STATUS "not building tests")
    
ENDIF (BUILD_VMEUPLOADERDOWNLOADER_TESTING)
