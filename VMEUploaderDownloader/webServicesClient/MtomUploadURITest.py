import unittest
import MtomUploadURI

class MtomUploadURITest(unittest.TestCase):
    """"""
    def testUploadURI(self):
        proxy = ""
        port = 0
        instance = MtomUploadURI.MtomUploadURI()
        result = instance.ListSrbDir('https://ws-lhdl-dev.cineca.it:12443/mafSRBUploadURI.cgi', proxy, port)
        #self.assertEqual(, True)
        print "Test Upload URI"
        print result

if __name__ == '__main__':
    unittest.main()