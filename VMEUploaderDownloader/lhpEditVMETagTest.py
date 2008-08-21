#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Roberto Mucci
#-----------------------------------------------------------------------------

import lhpEditVMETag
import Debug
import os
import msfParser
import sys, string
import unittest
import shutil

class lhpEditVMETagTest(unittest.TestCase):
    
    def testSaveXMLWithTagModified(self):

        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
        
        
        tag = lhpEditVMETag.lhpEditVMETag()
        tag.InputMSFDirectory = curDir + r'\msf_test_import_export_VME'
        #tag.OutputFolderName = curDir + r'\msf_test_import_export_VME'
        #tag.OutputFolderName = curDir + r'\Outgoing\0'
        tag.OutputFolderName = curDir + r'\msf_test_import_export_VME'
        tag.UnhandledPlusManualTagsListFileName = curDir + r'\unhandledPlusManualTagsList.csv'
        tag.HandledAutoTagsListFileName = curDir + r'\handledAutoTagsList.csv'
        tag.FakeRootMSFFileName = curDir + r'\applicationData\fakeRoot.xml'
        tag.FakeMSFFileName = curDir + r'\applicationData\fakeMSF.xml'
        tag.VmeToExtractID = 1   
        tag.ParseInput()
        tag.ParseOutput()
        
        
if __name__ == '__main__':
    unittest.main()
    
