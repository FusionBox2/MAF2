import DownloadHandler
import unittest
import os , Queue

class DownloadHandlerTest(unittest.TestCase):
      
    def setUp(self):  
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"  
        self.curDir = os.getcwd()        
        print " current directory is: " + self.curDir
       
    def testCreateDownloadHandler(self):
        
        queue = Queue.Queue()
        observer = None
        dirCache = self.curDir + r'\msf_test_import_export_VME\\'  #dir  where read msf
        id = 5
        if(os.path.exists(dirCache) == False): os.mkdir(dirCache)
        urlServer = 'http://devel.fec.cineca.it:12680/town/Members/portal_admin/test-lhp2'
        usr = 'testuser' #substitute 
        pwd = '6w8DHF' #substitute
        srbData = 'Data_1'
        fileSize = 667
        downloadHandler = DownloadHandler.DownloadHandler(queue, observer, dirCache, srbData , usr , pwd, urlServer, fileSize)
        downloadHandler.download()
    
if __name__ == '__main__':
    unittest.main()