import unittest
from MtomUpload import MtomUpload

class MtomUploadTest(unittest.TestCase):
    """"""
    def testUpload(self):
        testFile = "testData.zip"
        self.assertEqual(MtomUpload().Upload(testFile), True)

if __name__ == '__main__':
    unittest.main()
