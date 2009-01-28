#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Roberto Mucci <r.mucci@cineca.it>
#-----------------------------------------------------------------------------

import binaryImporter
import os, time, shutil
from lhpDefines import *
import StringIO
import unittest
import sys
#import urllib
#import zipfile




#import urllib, urllib2, base64, re, os, cookielib, sys
#from HttpsProxy import *

class binaryImporterTest(unittest.TestCase):
      
    def testCopyBinary(self):

        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
        
        sys.argv = []        
        sys.argv.append("true")
        sys.argv.append(curDir + r'\msf_test_import_export_VME\msf_test_import_export_VME.57.zvtk')
        sys.argv.append(curDir + r'\msf_test_import_export_VME\newVME.1.zvtk')
        importer = binaryImporter.binaryImporter()
        
        importer.CopyBinary()
        
if __name__ == '__main__':
    unittest.main()
                