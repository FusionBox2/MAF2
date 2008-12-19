#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import sys
sys.path.append('./webServicesClient')

import httplib, urlparse, string
from base64 import encodestring, decodestring
import lhpDictionaryVersionChecker

import Debug
import unittest

from datetime import *
from time import *

import webbrowser
import StringIO
import os


import urllib, urllib2, base64, re, os, cookielib, sys

class lhpDictionaryVersionCheckerTest(unittest.TestCase):
      
    def setUp(self):
        
        print "Beware:  In order to work run this est must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        
        print " current directory is: " + curDir
          
    
    def testRunLHPBuilder(self):            
        lhpDictionaryVersionChecker.run("LHPBuilder")
    
    def testRunPSLoader(self):        
        lhpDictionaryVersionChecker.run("PSLoader")
    
    def testGetDictionaryCreationDate(self):
        dd = lhpDictionaryVersionChecker.lhpDictionaryVersionChecker()
        dt = dd.GetRemoteDictionaryDate()
        print dt
       #  self.assertTrue(isinstance(dt, long))
        
        
        self.assertTrue(dt > 10000)
    
if __name__ == '__main__':
     unittest.main()
    
