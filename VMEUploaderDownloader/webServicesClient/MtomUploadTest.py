import unittest
import MtomUpload

class MtomUploadTest(unittest.TestCase):
    """"""
    def testUpload(self):
        testFile = "testData.zip"
        instance = MtomUpload.MtomUpload()
        result = instance.Upload(testFile)
        #self.assertEqual(, True)
        cheksum = result.chksum
        uri = result.uriFile

        print checksum
        print uri

if __name__ == '__main__':
    unittest.main()
