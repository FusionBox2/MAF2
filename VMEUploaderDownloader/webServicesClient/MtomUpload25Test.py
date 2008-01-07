import unittest
import MtomUpload25

class MtomUpload25Test(unittest.TestCase):
    """"""
    def testUpload(self):
        testFile = "testData.zip"
        instance = MtomUpload25.MtomUpload()
        result = instance.Upload(testFile)
        #self.assertEqual(, True)
        cheksum = result.chksum
        uri = result.uriFile

        print checksum
        print uri

if __name__ == '__main__':
    unittest.main()
