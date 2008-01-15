import unittest , os
import MtomSRBSize

class MtomSRBSizeTest(unittest.TestCase):
    """"""
    def testSRBSize(self):
        instance = MtomSRBSize.MtomSize()
        result = instance.ListSrbDir("Data_34")
        #self.assertEqual(, True)
        print "Test Size"
        print result
        print os.stat("testData.zip").st_size

if __name__ == '__main__':
    unittest.main()