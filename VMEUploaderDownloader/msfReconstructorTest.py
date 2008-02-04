import msfReconstructor
import unittest
import os

class DownloadHandlerTest(unittest.TestCase):
      
    def setUp(self):  
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"  
        self.curDir = os.getcwd()        
        print " current directory is: " + self.curDir
       
    def testMSFReconstructor(self):
        #reconstructMSF handle arguments
        #0 program
        #1 current dir
        #2 xml file name
        #3 lhpbuilder msf data directory
        msfReconstructor.reconstructMSF()
        
    
if __name__ == '__main__':
    unittest.main()