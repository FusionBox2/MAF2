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
from Debug import Debug

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
        if Debug:
            print "->"+ self.proxyHost + "<-"
            print "->"+ str(self.proxyPort) + "<-"
        
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setCredentials(self.userName, self.password)
        ws.setServer(self.URL)
        ws.ProxyURL = self.proxyHost
        ws.ProxyPort = self.proxyPort
        
        # timeout in seconds
        self.Timeout = 30
        socket.setdefaulttimeout(self.Timeout)
        
        try:
            out = ws.run('createresource')[1]
        except:
            if Debug:
                print "-----------Error calling createresource service------------"
            sys.exit(1)
           

        dom = xd.parseString(out)
        if dom.getElementsByTagName("fault"):
            if Debug:
                print "-----------Error in createresource service------------"
            for el in dom.getElementsByTagName("string"):
                for node in el.childNodes:  
                    error = node.data
            wx.MessageBox(error, wx.MessageBoxCaptionStr, wx.STAY_ON_TOP | wx.OK)
            if Debug:
                print error
            sys.exit(1)
        for el in dom.getElementsByTagName("string"):
            for node in el.childNodes:  
                XMLURI = node.data
        pass
    
        print XMLURI
        return XMLURI
  
def main():
    
    usage_msg = '''Usage: %s user, password, URL ''' % sys.argv[0]
    if(len(sys.argv) != 4): #for test
        print 'Error :\n' + usage_msg
        sys.exit(1)
       
       
    if(len(sys.argv) == 4):
        sys.argv = sys.argv[1:]
        #print sys.argv
        getURI = lhpGetXMLURI()
        getURI.XMLURI = getURI.getURI()
        
        print getURI.XMLURI

if __name__ == '__main__':
    main()
        