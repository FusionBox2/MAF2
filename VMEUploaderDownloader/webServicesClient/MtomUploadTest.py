import unittest, shutil
import MtomUpload,MtomUploadURI
import time

class MtomUploadTest(unittest.TestCase):
    """"""
    def testUpload(self):
        testFile = "..\\vmeUploaderTestData\\uploadSpeedProbeData\\uploadSpeedProbeData.vtk"
        testFileSize = 69.462
        instanceURI = MtomUploadURI.MtomUploadURI()
        serviceUrl = 'https://ws-lhdl.cineca.it/mafSRBUploadURI.cgi'
        binaryURI = "NOT PRESENT"
        proxyHost = ""
        proxyPort = 0
         
        freename = instanceURI.ListSrbDir(serviceUrl, proxyHost, proxyPort)
        print "testFile: " + testFile
        print "freeName: " + freename
        
        shutil.copyfile(testFile, freename)
        instanceUP = MtomUpload.MtomUpload()
        
        startT =  time.time()
       
        result = instanceUP.Upload(freename,'https://ws-lhdl-dev.cineca.it:12443/mafSRBUpload.cgi',proxyHost, proxyPort)
        
        endT = time.time()
        
        tElapsed = endT-startT
        
        speed = testFileSize / tElapsed
        
        print str(speed)
        
        print "result: " + str(result)
        
        shutil.move(freename, testFile)
        
        checksum = result.chksum
        uri = result.uriFile

        print "cheksum: " + str(checksum)
        print "uri: " + str(uri)

if __name__ == '__main__':
    unittest.main()
