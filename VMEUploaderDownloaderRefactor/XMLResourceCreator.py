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

class XMLResourceCreator:
    """Creates an empty resource on repository"""
  
    def __init__(self):
                        
        self.userName = sys.argv[0]
        self.password = sys.argv[1]
        self.URL = sys.argv[2]
        

          
        # proxy
        self.proxyHost = ''
        self.ProxyPort = ''
        
    def CreateEmptyXMLResource(self):
        """ 
           Creates an empty XML resource on repository and return the URI
        """
        maxTry = 5
        self.proxyHost, self.proxyPort = retriveProxyParameters()
        if Debug:
            print "->"+ self.proxyHost + "<-"
            print "->"+ str(self.proxyPort) + "<-"
        
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setCredentials(self.userName, self.password)
        ws.setServer(self.URL)
        ws.ProxyURL = self.proxyHost
        ws.ProxyPort = self.proxyPort
        XMLURI = -1
        
        # timeout in seconds
        self.Timeout = 90
        socket.setdefaulttimeout(self.Timeout)
        
        for c in range(0,maxTry):
            try:
                out = ws.run('createresource')[1]
            except:
                if Debug:
                    print "-----------Error calling createresource service------------"
                time.sleep(3)
                continue
        
            dom = xd.parseString(out)
            if dom.getElementsByTagName("fault"):
                if Debug:
                    print "-----------Error in createresource service------------"
                for el in dom.getElementsByTagName("string"):
                    for node in el.childNodes:  
                        error = node.data
                    if (error.find('Over Quota') != -1):
                        overQuota = "OverQuota"
                        print overQuota
                        return overQuota
                    
                if Debug:
                    print error
                    
            for el in dom.getElementsByTagName("string"):
                for node in el.childNodes:  
                    XMLURI = node.data
            if XMLURI == -1:
                continue
            else:
                break
    
        print XMLURI
        if  XMLURI == -1:
            sys.exit(1)
            #todo best
        return XMLURI
  
def main():
    
    usage_msg = '''Usage: %s user, password, URL ''' % sys.argv[0]
    if(len(sys.argv) != 4): #for test
        print 'Error :\n' + usage_msg
        sys.exit(1)
       
       
    if(len(sys.argv) == 4):
        sys.argv = sys.argv[1:]
        #print sys.argv
        getURI = XMLResourceCreator()
        getURI.XMLURI = getURI.CreateEmptyXMLResource()
        
        print getURI.XMLURI

if __name__ == '__main__':
    main()
        