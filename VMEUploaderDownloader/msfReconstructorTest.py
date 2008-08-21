import msfReconstructor
import unittest
import os

class DownloadHandlerTest(unittest.TestCase):
      
    def setUp(self):  
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"  
        self.curDir = os.getcwd()        
        print " current directory is: " + self.curDir
       
    def testMSFReconstructor(self):
        
        msfR = msfReconstructor.MSFReconstructor()
        msfR.currentDir = os.getcwd()
        msfR.InputVMEBinaryDataFileName = msfR.currentDir + r'\msf_test_import_export_VME\TESTmsf.1.vtk'
        dataBinary = 'TESTmsf.1.vtk'
        msfR.InputVMEXMLFileName = msfR.currentDir + r'\vmeUploaderTestData\exportedVME.xml'
        msfR.FakeRootMSFFileName = msfR.currentDir + r'\applicationData\fakeRoot.xml'
        msfR.FakeMSFFileName = msfR.currentDir + r'\applicationData\fakeMSF.xml'
        msfR.OutputMSFFileName = msfR.currentDir + r'\testDownload\\outputMAF.msf'
        msfR.OutputMSFFolderName = msfR.currentDir + r'\testDownload'
        msfR.build()
        
    
if __name__ == '__main__':
    unittest.main()