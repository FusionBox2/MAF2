import os
import unittest
from lhpDefines import *

class lhpDefinesTest(unittest.TestCase):
    
    def setUp(self):  
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"  
        
        self.curDir = os.getcwd()
        
        print " current directory is: " + self.curDir
       
    def testRetriveProxyParameters(self):
        retriveProxyParameters();     
        pass
    
if __name__ == '__main__':
    unittest.main()