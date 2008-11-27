#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

from Debug import Debug

import sys
sys.path.append('./webServicesClient')

import lhpFAOntoCSVDictionaryDownloader
import unittest
import msfParser
import os
from xml.dom import minidom

class lhpFAOntoCSVDictionaryDownloaderTest(unittest.TestCase):
      
    def setUp(self):
        
        print "Beware:  In order to work run this est must be launched from VMEUploaderDownloader dir!"
        
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
          
    def testDownloadFAOntologySubdictionaryHTTPS(self):
        
        host = "www.biomedtown.org"
        selector = "/biomed_town/LHDL/users/swclient/dictionaries/FA_onto"
        
        outputXMLDictionaryFileName = "lhpXMLFAOntologySourceSubdictionary"
        
        lhpFAOntoCSVDictionaryDownloader.run(host, selector, outputXMLDictionaryFileName)
        
    def testRun(self):
        
        host = "www.biomedtown.org"
        selector = "/biomed_town/LHDL/users/swclient/dictionaries/FA_onto"
        
        outputXMLDictionaryFileName = "lhpXMLFAOntologySourceSubdictionary"
        
        lhpFAOntoCSVDictionaryDownloader.run(host, selector, outputXMLDictionaryFileName)
    
            
if __name__ == '__main__':
     unittest.main()
    
