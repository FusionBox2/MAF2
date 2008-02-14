import unittest
from MtomDownload import MtomDownload
import os, commands, md5

class MtomDownloadTest(unittest.TestCase):
    """"""
    def testDownload(self):
        testFile = "testData.zip"
        testFileOLD = testFile + "OLD"
        isHere = False
        try:
            os.stat(testFile)
            commands.getoutput("mv %s %s" % (testFile, testFileOLD))
            isHere = True
        except:
            pass
        serviceUrl = 'https://ws-lhdl-dev.cineca.it:12443/mafSRBDownload.cgi'
        self.assertEqual(MtomDownload().Download(testFile,serviceUrl), True)
        
        if isHere:
            f1 = open(testFile, 'rb')
            f2 = open(testFileOLD, 'rb')
            self.assertEqual(md5.new(f1.read()).digest(), md5.new(f2.read()).digest())
            f1.close()
            f2.close()

if __name__ == '__main__':
    unittest.main()
