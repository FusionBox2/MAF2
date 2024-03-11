#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

import vmeUploader
import os
import msfParser
import sys, string
import unittest
import shutil

class vmeUploaderTest(unittest.TestCase):
    
    def testCreateOutgoingDirectoryAndUpload(self):

        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
        
        upl = vmeUploader.vmeUploader()
        upl.InputMSFDirectory = curDir + r'\InputMSFForUploadTestData'
        upl.UnhandledPlusManualTagsListFileName = curDir + r'\unhandledPlusManualTagsList.csv'
        upl.HandledAutoTagsListFileName = curDir + r'\handledAutoTagsList.csv'
        upl.OutputFolderName = curDir + r'\Outgoing'
        upl.VmeToExtractID = 1    
        upl.Upload()
        
        
if __name__ == '__main__':
    unittest.main()
    
