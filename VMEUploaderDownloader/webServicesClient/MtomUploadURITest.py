import unittest
import MtomUploadURI

class MtomUploadURITest(unittest.TestCase):
    """"""
    def testUploadURI(self):
        instance = MtomUploadURI.MtomUploadURI()
        result = instance.ListSrbDir()
        #self.assertEqual(, True)
        print "Test Upload URI"
        print result

if __name__ == '__main__':
    unittest.main()