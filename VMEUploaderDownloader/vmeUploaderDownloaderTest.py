#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Roberto Mucci
#-----------------------------------------------------------------------------

import vmeUploader
import vmeDownloader
import os
import msfParser
import sys, string
import unittest
import shutil

class vmeUploaderDownloaderTest(unittest.TestCase):
    
    def setUp(self):
        counter = 0;
    
    
    def CreateOutgoingDirectoryAndUpload(self, vmeID):
        #extract the VME with "vmeID" ID e and save its xml part 
        #and its binary part in directory \Outgoing

        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()   
        
            
        print " current directory is: " + curDir
        
        upl = vmeUploader.vmeUploader()
        upl.InputMSFDirectory = curDir + r'\InputMSFForUploadTestData'
        upl.UnhandledPlusManualTagsListFileName = curDir + r'\unhandledPlusManualTagsList.csv'
        upl.HandledAutoTagsListFileName = curDir + r'\handledAutoTagsList.csv'
        upl.OutputFolderName = curDir + r'\Outgoing'
        upl.VmeToExtractID = vmeID
        upl.Upload()
    
    def CreateIncomingMSFDirectory(self, binaryName, outputMSFFileName):
        #takes the xml part and the binary part of a VME form directory \Outgoing
        #and save the corresponding msf and binary part in directory \Incoming
        
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
        
        msfBuilder = vmeDownloader.MSFBuilder()
        msfBuilder.InputVMEBinaryDataFileName = curDir + r'\..\Outgoing' + binaryName
        msfBuilder.InputVMEXMLFileName = curDir + r'\..\Outgoing\exportedVME.xml'
        msfBuilder.FakeRootMSFFileName = curDir + r'\..\applicationData\fakeRoot.xml'
        msfBuilder.OutputMSFFileName = outputMSFFileName
        msfBuilder.OutputMSFFolderName = curDir + r'\..\Incoming'
        msfBuilder.Build()
        os.chdir(curDir + r'\..') #set the working directory to the original one (VMEUploaderDownloader)
        
    
    def test(self):
        self.CreateOutgoingDirectoryAndUpload(1)
        self.CreateIncomingMSFDirectory('\TESTmsf.1.vtk', 'VolumeGray_ID_1.msf')
        
        self.CreateOutgoingDirectoryAndUpload(4)
        self.CreateIncomingMSFDirectory('\TESTmsf.4.vtk', 'Mesh_ID_4.msf')
        
        self.CreateOutgoingDirectoryAndUpload(5)
        self.CreateIncomingMSFDirectory('\TESTmsf.6.vtk', 'Surface_ID_5.msf')
        
        self.CreateOutgoingDirectoryAndUpload(11)
        self.CreateIncomingMSFDirectory('\TESTmsf.18.zvtk', 'Vector_ID_11.msf')
        
        self.CreateOutgoingDirectoryAndUpload(12)
        self.CreateIncomingMSFDirectory('\msf_test_import_export_VME.50.vtk', 'Landmark_ID_12.msf')
        
        self.CreateOutgoingDirectoryAndUpload(16)
        self.CreateIncomingMSFDirectory('\msf_test_import_export_VME.52.sca', 'Analog_ID_16.msf')
        
        self.CreateOutgoingDirectoryAndUpload(23)
        self.CreateIncomingMSFDirectory('\ExternalFile.txt', 'ExternalData_ID_23.msf')
        
        self.CreateOutgoingDirectoryAndUpload(24)
        self.CreateIncomingMSFDirectory('\msf_test_import_export_VME.54.vtk', 'Image_ID_24.msf')
        
        self.CreateOutgoingDirectoryAndUpload(25)
        self.CreateIncomingMSFDirectory('\msf_test_import_export_VME.56.vtk', 'Polyline_ID_25.msf')
           
        self.CreateOutgoingDirectoryAndUpload(28)
        self.CreateIncomingMSFDirectory('\msf_test_import_export_VME.57.zvtk', 'RAWLandmark_ID_28.msf')
        
            
    #def testVMEDownloader(self):
        #self.assertEqual()
        
        
if __name__ == '__main__':
    unittest.main()
    
