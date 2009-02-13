#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------


import os
import downloadSingleXML
import unittest
import sys

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
        d = downloadSingleXML.downloadSingleXML()
        
        d.user = "testuser"
        d.password = "6w8DHF"
        d.fileToDownload = 'dataresource-8660'
        d.incomingCacheDir = str(os.getcwd()+"\\testDownload\\")
      
        d.downloadXMLFromBasket()
        
        d.datasetSRBURI = d.retrieveTagValue('L0000_resource_data_Dataset_DatasetURI')
        d.datasetFileSize = d.retrieveTagValue('L0000_resource_data_Size_FileSize')
        
        d.moveFileInIncomingCacheDirectory()

        assert(d.datasetSRBURI == "data_4085")
        assert(d.datasetFileSize == "437")

        self.assertTrue(True)
        
    def testMain(self):
          
       sys.argv = []
       sys.argv.append("downloadSingleXML.py")
       sys.argv.append("testuser") #substitute
       sys.argv.append("6w8DHF") #substitute
       sys.argv.append('dataresource-8660') #xml test present in repository
       sys.argv.append(os.getcwd()+"\\testDownload\\")
       sys.argv.append(os.getcwd()+"\\testDownload\\")
       result = downloadSingleXML.main()
       
       
if __name__ == '__main__':
    unittest.main()