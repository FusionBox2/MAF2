#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Roberto Mucci <r.mucci@cineca.it>
#-----------------------------------------------------------------------------

from webServicesClient import xmlrpcDemoWS
import xml.dom.minidom as xd
import os, time
from lhpDefines import *
import StringIO

import urllib, urllib2, base64, re, os, cookielib, sys
from HttpsProxy import *

class lhpGetXMLURI:
      
    def __init__(self):
                        
        self.userName = sys.argv[0]
        self.password = sys.argv[1]
        self.URL = sys.argv[2]

          
        # proxy
        self.proxyHost = ''
        self.ProxyPort = ''
        
    def getURI(self):
        """ 
           Creates an empty resource on repository and return the URI
        """
        self.proxyHost, self.proxyPort = retriveProxyParameters()
        print "->"+ self.proxyHost + "<-"
        print "->"+ str(self.proxyPort) + "<-"
        
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setCredentials(self.userName, self.password)
        ws.setServer(self.URL)
        ws.ProxyURL = self.proxyHost
        ws.ProxyPort = self.proxyPort
        try:
            out = ws.run('createresource')[1]
        except:
            print "-----------Error calling createresource service------------"
            return
            
        
        dom = xd.parseString(out)
        if dom.getElementsByTagName("fault"):
            print "-----------Error in createresource service------------"
            for el in dom.getElementsByTagName("string"):
                for node in el.childNodes:  
                    error = node.data
            print error
            return
        for el in dom.getElementsByTagName("string"):
            for node in el.childNodes:  
                XMLURI = node.data
        pass
    
        print XMLURI
        return XMLURI
  
def main():
    
    if(len(sys.argv) != 4): #for test
        sys.argv = []
        sys.argv.append("testuser") #substitute
        sys.argv.append("6w8DHF") #substitute
        sys.argv.append("http://devel.fec.cineca.it:12680/town/biomed_town/LHDL/users/repository/lhprepository2/") #substitute
        
        getURI = lhpGetXMLURI()
        getURI.XMLURI = getURI.getURI()
        #add code to remove resource created
        return
       
    if(len(sys.argv) == 4):
        sys.argv = sys.argv[1:]
        #print sys.argv
        getURI = lhpGetXMLURI()
        getURI.XMLURI = getURI.getURI()
        
        print getURI.XMLURI

if __name__ == '__main__':
    main()
        