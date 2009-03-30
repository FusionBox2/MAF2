#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Roberto Mucci <r.mucci@cineca.it>
#-----------------------------------------------------------------------------

import lhpReadRemoteTag
import os, time, shutil
from lhpDefines import *
import StringIO
import unittest
import sys

class lhpReadRemoteTagTest(unittest.TestCase):
      
    def testReadTag(self):
        
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
        
        sys.argv = []
        sys.argv.append("testuser") #substitute
        sys.argv.append("6w8DHF") #substitute
        sys.argv.append('http://devel.fec.cineca.it:12680/town/biomed_town/LHDL/users/repository/lhprepository2/') #substitute
        sys.argv.append('dataresource-7197,L0000_resource_MAF_TreeInfo_VmeChildURI1') #xml test present in repository    
        read = lhpReadRemoteTag.lhpReadRemoteTag()
        read.ReadTag()
        
        
if __name__ == '__main__':
    unittest.main()
        