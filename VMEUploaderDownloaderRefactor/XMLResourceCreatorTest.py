#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Roberto Mucci <r.mucci@cineca.it>
#-----------------------------------------------------------------------------

import lhpXMLResourceCreator
import os, time, shutil
from lhpDefines import *
import StringIO
import unittest
import sys

class lhpGetXMLURITest(unittest.TestCase):
      
    def testGetURI(self):
        
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
        
        sys.argv = []        
        sys.argv.append("testuser") #substitute
        sys.argv.append("6w8DHF") #substitute
        sys.argv.append("http://devel.fec.cineca.it:12680/town/biomed_town/LHDL/users/repository/lhprepository2/") #substitute
        resourceCreator = lhpXMLResourceCreator.XMLResourceCreator()
        resourceCreator.CreateEmptyXMLResource()



if __name__ == '__main__':
    unittest.main()
        