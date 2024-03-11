#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

import vmeDownloader
import os
import msfParser
import sys, string
import unittest
import shutil

class vmeDownloaderTest(unittest.TestCase):
      
    # testing data coming from Xia webservices    
 def testCreateIncomingMSFDirectory(self):
        
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
        
        msfBuilder = vmeDownloader.MSFBuilder()
        msfBuilder.InputVMEBinaryDataFileName = curDir + r'\testDownload\Data_549\Data_549.vtk'
        msfBuilder.InputVMEXMLFileName = curDir + r'\testDownload\Data_549\Data_549.xml'

        msfBuilder.FakeRootMSFFileName = curDir + r'\applicationData\fakeRoot.xml'
        msfBuilder.FakeMSFFileName = curDir + r'\applicationData\fakeMSF.xml'
        msfBuilder.OutputMSFFolderName = curDir + r'\testDownload\Data_549\Incoming'
        msfBuilder.OutputMSFFileName = msfBuilder.OutputMSFFolderName + r'\outputMSF.msf'
        msfBuilder.Build()
        
if __name__ == '__main__':
    unittest.main()
    
