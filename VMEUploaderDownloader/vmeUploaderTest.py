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
      
    # def setUp(self):
        # self.inFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\msf_test_import_export_VME\msf_test_import_export_VME.msf'
        # self.sourceMSFFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\msf_test_import_export_VME\copyied_msf_test_VME_Uploader.msf'
        # shutil.copy(self.inFileName, self.copyFileName)
        
    
    def testParse(self):
        print os.getcwd()
        
        upl = vmeUploader.vmeUploader()
        upl.InputMSFFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\msf_test_import_export_VME\msf_test_import_export_VME.msf'
        upl.DictionaryFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\testDictionaries\testDictionary.txt'
        upl.VmeToExtractID = 1    
        upl.Parse()
        
if __name__ == '__main__':
    unittest.main()
    
