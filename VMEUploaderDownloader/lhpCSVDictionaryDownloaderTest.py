#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import sys
sys.path.append('./webServicesClient')

import httplib, urlparse, string
from base64 import encodestring, decodestring
import lhpCSVDictionaryDownloader
import Debug

import Debug
import unittest

import os


import urllib, urllib2, base64, re, os, cookielib, sys

class lhpCSVDictionaryDownloaderTest(unittest.TestCase):
      
    def setUp(self):
        
        print "Beware:  In order to work run this est must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
          
    
    def testDownloadDictionary(self):
        
        dd = lhpCSVDictionaryDownloader.lhpCSVDictionaryDownloader()
        dd.DownloadCSVDictionary()
        dd.ConvertDownloadedCSV2XML()
        
    def testRun(self):
        
        host = "www.biomedtown.org"
        selector = "/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries/LHDL_dictionary.csv"
        
        outputXMLDictionaryFileName = "lhpDictionary.xml"
        
        lhpCSVDictionaryDownloader.run(host,selector,outputXMLDictionaryFileName)
        
        
if __name__ == '__main__':
    unittest.main()
    
