import DownloadHandler
import unittest
import os , Queue

class DownloadHandlerTest(unittest.TestCase):
      
    def setUp(self):  
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"  
        self.curDir = os.getcwd()        
        print " current directory is: " + self.curDir
       
    def testCreateDownloadHandlerAndDownloadFromSRB(self):
        
        queue = Queue.Queue()
        observer = None
        dirCache = self.curDir + r'\msf_test_import_export_VME\\'  #dir  where read msf
        id = 5
        if(os.path.exists(dirCache) == False): os.mkdir(dirCache)
        urlServer = 'https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2'
        usr = 'testuser' #substitute 
        pwd = '6w8DHF' #substitute
        srbData = 'data_4085'
        fileSize = 437
        downloadHandler = DownloadHandler.DownloadHandler(queue, observer, dirCache, srbData\
                                                           , usr , pwd, urlServer, True , fileSize)
                    
        downloadHandler.download()
        self.assertTrue(True)
    
if __name__ == '__main__':
    unittest.main()