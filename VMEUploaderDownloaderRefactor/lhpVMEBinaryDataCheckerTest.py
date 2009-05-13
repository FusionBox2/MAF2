import lhpVMEBinaryDataChecker
import unittest
import os 

class DownloadHandlerTest(unittest.TestCase):
      
    def setUp(self):  
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"  
        self.curDir = os.getcwd()        
        print " current directory is: " + self.curDir
        
        self.msfFileName = self.curDir + r'\msf_test_import_export_VME\msf_test_import_export_VME.msf'
        print self.msfFileName
        self.assertTrue(os.path.exists(self.msfFileName))
        
    def testVMEVolumeGray(self):
        
        binaryName = lhpVMEBinaryDataChecker.run(self.msfFileName , 1)
        self.assertEquals(binaryName, "TESTmsf.1.vtk")
    
    def testRoot(self):
        
        binaryName = lhpVMEBinaryDataChecker.run(self.msfFileName , 5000)
        self.assertEquals(binaryName, "")
    
    def testNonExistingVMEId(self):
        
        binaryName = lhpVMEBinaryDataChecker.run(self.msfFileName , -1)
        self.assertEquals(binaryName, "")
    
    def testVMEMesh(self):
        
        binaryName = lhpVMEBinaryDataChecker.run(self.msfFileName , 4)
        self.assertEquals(binaryName, "TESTmsf.4.vtk")
        
if __name__ == '__main__':
    unittest.main()