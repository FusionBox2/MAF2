import UploadHandler
import unittest
import os , Queue

class UploadHandlerTest(unittest.TestCase):
      
    def setUp(self):  
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"  
        self.curDir = os.getcwd()        
        print " current directory is: " + self.curDir
       
    def testCreateUploadHandler(self):
        #TODO to make the test fail otherwise  it's an infinite loop: needs fixing
        self.assertFalse(True)
        
        
        queue = Queue.Queue()
        observer = None
        dirCache = self.curDir + r'\InputMSFForUploadTestData'  #dir  where read msf.
        dirOutgoing = self.curDir + r'\Outgoing' #dir  where write xml and binary
        id = 1
        if(os.path.exists(dirCache) == False): os.mkdir(dirCache)
        if(os.path.exists(dirOutgoing) == False ): os.mkdir(dirOutgoing)
        server = 'http://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2'
        uploadHandler = UploadHandler.UploadHandler(\
            queue,observer, dirCache, id , "testuser", "GRDPt8",server, "unhandledPlusManualTagsList.csv"\
        , False, "pippo")
        uploadHandler.upload()
    
if __name__ == '__main__':
    unittest.main()
    