#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Roberto Mucci <r.mucci@cineca.it>
#-----------------------------------------------------------------------------

from webServicesClient import xmlrpcDemoWS
from Debug import Debug
import xml.dom.minidom as xd
import os, time
from lhpDefines import *
import StringIO

import urllib, urllib2, base64, re, os, cookielib, sys
from HttpsProxy import *

class lhpDownloadVmeCheck:
      
    def __init__(self):
                        
        self.userName = sys.argv[0]
        self.password = sys.argv[1]
        self.URL = sys.argv[2]
        
        self.parameter = sys.argv[3]
  
        # proxy
        self.proxyHost = ''
        self.ProxyPort = ''
        
    def VmeCheck(self):
        """ 
           Send information about VME correctly downloaded
        """
        
        self.proxyHost, self.proxyPort = retriveProxyParameters()
        if Debug:
            print "->"+ self.proxyHost + "<-"
            print "->"+ str(self.proxyPort) + "<-"
        
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setServer(self.URL)
        ws.setCredentials(self.userName, self.password)
        ws.ProxyURL = self.proxyHost
        ws.ProxyPort = self.proxyPort
        
        if Debug:
            print "->"+ ws.ProxyURL + "<-"
            print "->"+ str(ws.ProxyPort) + "<-"
            print self.parameter      
        
        out = ws.run("xmldownloadcheck", self.parameter)[1]
        
        dom = xd.parseString(out)
        if dom.getElementsByTagName("fault"):
            print "Error sending check information to repository"
            return
       
  
  
def main():
    
    if(len(sys.argv) < 5): #for test
       sys.argv = []
       sys.argv.append("testuser") #substitute
       sys.argv.append("10&6w8DHF!") #substitute
       sys.argv.append('http://devel.fec.cineca.it:12680/town/biomed_town/LHDL/users/repository/lhprepository2/') #substitute
       sys.argv.append('dataresource-1901,success') #xml test present in repository
       
       check = lhpDownloadVmeCheck()
       check.VmeCheck()
       
    if(len(sys.argv) >= 5):
        sys.argv = sys.argv[1:]
        #print sys.argv
        check = lhpDownloadVmeCheck()
        check.VmeCheck()

if __name__ == '__main__':
    main()
        