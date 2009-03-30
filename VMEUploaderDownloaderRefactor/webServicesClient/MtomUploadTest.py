import unittest, shutil
import MtomUpload,MtomUploadURI

class MtomUploadTest(unittest.TestCase):
    """"""
    def testUpload(self):
        testFile = "exportedVME.xml"
        instanceURI = MtomUploadURI.MtomUploadURI()
        freename = instanceURI.ListSrbDir('https://ws-lhdl-dev.cineca.it:12443/mafSRBUploadURI.cgi')
        print testFile
        shutil.move(testFile, freename)
        print freename
        instanceUP = MtomUpload.MtomUpload()
        result = instanceUP.Upload(freename,'https://ws-lhdl-dev.cineca.it:12443/mafSRBUpload.cgi')
        
        shutil.move(freename, testFile)
        
        #self.assertEqual(, True)
        checksum = result.chksum
        uri = result.uriFile

        print checksum
        print uri

if __name__ == '__main__':
    unittest.main()
