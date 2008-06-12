#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import sys
sys.path.append('./webServicesClient')

import httplib, urlparse, string
from base64 import encodestring, decodestring
import lhpAuthenticationControl

import Debug
import unittest

from datetime import *
from time import *

import StringIO
import os


import urllib, urllib2, base64, re, os, cookielib, sys

class lhpAuthenticationControlTest(unittest.TestCase):
      
    def setUp(self):
        
        print "Beware:  In order to work run this est must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
          
    
    def testRun(self):
        if Debug:
            # authentication grab that
            username = 'lhpparabuild'
            password = '2bf5ZM'
            print "trying to authenticate user: " + username + " with password: " + password 
       
        
        lhpAuthenticationControl.run(username, password)
                
if __name__ == '__main__':
     unittest.main()
    
