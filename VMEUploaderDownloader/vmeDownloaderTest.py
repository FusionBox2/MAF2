#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

import vmeDownloader
import os
import domParser
import sys, string
import unittest
import shutil

class vmeDownloaderTest(unittest.TestCase):
      
    def setUp(self):
        self.InputXMLFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\vmeUploaderTestData\exportedVME.xml'
        
    
    def testMSFBuilder(self):
        msfBuilder = vmeDownloader.MSFBuilder()
        msfBuilder.InputVMEXMLFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\vmeUploaderTestData\exportedVME.xml'
        msfBuilder.FakeRootMSFFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\applicationData\fakeRoot.xml'
        msfBuilder.OutputMSFFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\outputMAF.msf'
        msfBuilder.Build()
        

    # def testParse(self):
        #print "a"
        #print os.getcwd()
        
        #upl = vmeDownloader.vmeDownloader()
        #upl.InputXMLFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\vmeUploaderTestData\exportedVME.xml'
        #upl.Parse()

if __name__ == '__main__':
    unittest.main()
    
