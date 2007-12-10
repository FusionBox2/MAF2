import UploadHandler
import unittest
import os , Queue

class UploadHandlerTest(unittest.TestCase):
      
    def setUp(self):  
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"  
        self.curDir = os.getcwd()        
        print " current directory is: " + self.curDir
       
    def testCreateUploadHandler(self):
        queue = Queue.Queue()
        observer = None
        dirCache = self.curDir + r'\msf_test_import_export_VME'  #dir  where read msf
        dirOutgoing = self.curDir + r'\Outgoing' #dir  where write xml and binary
        id = 5
        if(os.path.exists(dirCache) == False): os.mkdir(dirCache)
        if(os.path.exists(dirOutgoing) == False ): os.mkdir(dirOutgoing)
        uploadHandler = UploadHandler.UploadHandler(queue,observer, dirCache, id)
        uploadHandler.upload()
    
if __name__ == '__main__':
    unittest.main()