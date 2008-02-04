import os
import downloadSingleXML
import unittest

class downloadSingleXMLTest(unittest.TestCase):
    
    def setUp(self):  
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"        
        self.curDir = os.getcwd()
        
        print " current directory is: " + self.curDir
       
    def testDownloadSingleXML(self):
        #main handle arguments
        #0 program
        #1 user
        #2 pwd
        #3 xml file name to download
        #4 where download it
        downloadSingleXML.main()
        
        pass
    
    
if __name__ == '__main__':
    unittest.main()