import unittest , os, time
import MtomSRBSize

class MtomSRBSizeTest(unittest.TestCase):
    """"""
    def testSRBSize(self):
          instance = MtomSRBSize.MtomSize()
          serviceUrl = 'https://ws-lhdl-dev.cineca.it:12443/mafSRBSize.cgi'
          result = instance.ListSrbDir("testData.zip",serviceUrl)
          #self.assertEqual(, True)
          print "Test Size"
          print result
          print os.stat("testData.zip").st_size

if __name__ == '__main__':
    unittest.main()