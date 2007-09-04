import vmeUploader
import os
import domParser
import sys, string
import unittest
import shutil

class vmeUploaderTest(unittest.TestCase):
      
    def setUp(self):
        # self.inFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\msf_test_import_export_VME\msf_test_import_export_VME.msf'
        # self.sourceMSFFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\msf_test_import_export_VME\copyied_msf_test_VME_Uploader.msf'
        # shutil.copy(self.inFileName, self.copyFileName)
        
        self.VmeUploader = vmeUploader.vmeUploader()
    
    def testParse(self):
        print os.getcwd()
        upl = self.VmeUploader
        upl.Parse()
        
    # def testUploader(self):
        # create the uploader
        # set the msf file name
        # set the dictionary file name
        # upload
        
        
        
if __name__ == '__main__':
    unittest.main()
    
