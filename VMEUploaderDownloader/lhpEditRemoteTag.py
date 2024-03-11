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

class lhpEditRemoteTag:
      
    def __init__(self):
                        
        self.userName = sys.argv[0]
        self.password = sys.argv[1]
        self.URL = sys.argv[2]
        
        self.parameter = ""
        count = 0
        for i in sys.argv:
          if(count > 2):
            if(self.parameter == ""):
              self.parameter = i
            else:
              self.parameter = self.parameter + ' ' + i #vme name if UPLOAD, dataURI if DOWNLOAD
          count = count + 1
  
        # proxy
        self.proxyHost = ''
        self.ProxyPort = ''
        
    def EditTag(self):
        """ 
           Edit tag of a remote resource
        """
        
        self.proxyHost, self.proxyPort = retriveProxyParameters()
        if Debug:
            print "->"+ self.proxyHost + "<-"
            print "->"+ str(self.proxyPort) + "<-"
        
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setServer(self.URL + self.parameter.split(',')[0])
        ws.setCredentials(self.userName, self.password)
        ws.ProxyURL = self.proxyHost
        ws.ProxyPort = self.proxyPort
        if Debug:
            print "->"+ ws.ProxyURL + "<-"
            print "->"+ str(ws.ProxyPort) + "<-"
            print self.parameter      
        
        # timeout in seconds
        self.Timeout = 30
        socket.setdefaulttimeout(self.Timeout)
        try:
            out = ws.run("xmledit", self.parameter)[1]
        except:
            if Debug:
                print "-----------Error in xmledit service------------"
            sys.exit(1)    
            
               
        dom = xd.parseString(out)
        if dom.getElementsByTagName("fault"):
            if Debug:
                print "Error editing tags on repository"
            sys.exit(1)
   
  
  
def main():
    
    usage_msg = '''Usage: %s user, password, URL, resource, tag ''' % sys.argv[0]
    if(len(sys.argv) < 5): #for test
        print 'Error :\n' + usage_msg
        sys.exit(1)
      
       
    if(len(sys.argv) >= 5):
        sys.argv = sys.argv[1:]
        #print sys.argv
        edit = lhpEditRemoteTag()
        edit.EditTag()

if __name__ == '__main__':
    main()
        