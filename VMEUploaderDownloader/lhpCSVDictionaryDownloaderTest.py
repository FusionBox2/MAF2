#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import sys
sys.path.append('./webServicesClient')

import lhpCSVDictionaryDownloader
import Debug

import Debug
import unittest

import os

class lhpCSVDictionaryDownloaderTest(unittest.TestCase):
      
    def setUp(self):
        
        print "Beware:  In order to work run this est must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
            
    def testRun(self):
        """This is what I have to do...Not working with the following log
        
======================================================================
FAIL: testRun (__main__.lhpCSVDictionaryDownloaderTest)
----------------------------------------------------------------------
Traceback (most recent call last):
  File "d:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\lhpCSVDictionaryDownloaderTest.py", line 37, in testRun
    self.assertEqual(out, True)
AssertionError: "<?xml version='1.0'?>\n<methodResponse>\n<fault>\n<value><struct>\n<member>\n<name>faultCode</name>\n<value><int>-1</int></value>\n</member>\n<member>\n<name>faultString</name>\n<value><string>Unexpected Zope exception: zExceptions.NotFound -  \n \n\n \n \n \n\n \n   Site Error \n   An error was encountered while publishing this resource.\n   \n    Debugging Notice  \n\n  Zope has encountered a problem publishing your object. \nThe object at http://www.biomedtown.org/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries/XMLDownload has an empty or missing docstring. Objects must have a docstring to be published.\n   \n\n   Troubleshooting Suggestions \n\n   \n   The URL may be incorrect. \n   The parameters passed to this resource may be incorrect. \n   A resource that this resource relies on may be\n      encountering an error. \n   \n\n   For more detailed information about the error, please\n  refer to error log.\n   \n\n   If the error p
ersists please contact the site maintainer.\n  Thank you for your patience.\n   \n  \n </string></value>\n</member>\n</struct></value>\n</fault>\n</methodResponse>\n" != True

----------------------------------------------------------------------

        """
        ws = lhpCSVDictionaryDownloader.lhpCSVDictionaryDownloader()

        testFile = "LHDL_dictionary.dic"
        
        #download
        print "download remote " + testFile + "..."
        out = ws.run('xmldownload', testFile)
        self.assertEqual(out, True)
        print "done!"
        
        #md5
        #print "md5 checking..."
        #f1 = open(testFile, 'rb')
        #f2 = open(testFileOLD, 'rb')
        #self.assertEqual(md5.new(f1.read()).digest(), md5.new(f2.read()).digest())
        #f1.close()
        #f2.close()
        #print "done!"

    
    def testRun2(self):
        """This is just for comparison and it`s working... """
        ws = lhpCSVDictionaryDownloader.lhpCSVDictionaryDownloader()
        
        testFile = "testXml.msf"
        
        #download
        print "download remote " + testFile + "..."
        out = ws.run2('xmldownload', testFile)
        self.assertEqual(out, True)
        print "done!"
        
        #md5
        #print "md5 checking..."
        #f1 = open(testFile, 'rb')
        #f2 = open(testFileOLD, 'rb')
        #self.assertEqual(md5.new(f1.read()).digest(), md5.new(f2.read()).digest())
        #f1.close()
        #f2.close()
        #print "done!"

if __name__ == '__main__':
    unittest.main()
    
