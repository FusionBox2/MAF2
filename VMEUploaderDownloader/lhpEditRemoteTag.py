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

class lhpEditRemoteTag:
      
    def __init__(self):
                        
        self.userName = sys.argv[0]
        self.password = sys.argv[1]
        
        self.parameter = ""
        count = 0
        for i in sys.argv:
          if(count > 1):
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
        print "->"+ self.proxyHost + "<-"
        print "->"+ str(self.proxyPort) + "<-"
        
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setServer('https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2/' + self.parameter.split(',')[0])
        ws.setCredentials(self.userName, self.password)
        ws.ProxyURL = self.proxyHost
        ws.ProxyPort = self.proxyPort
        #print "->"+ self.currentUser + "<-"
        #print "->"+ self.currentPassword + "<-"
        print "->"+ ws.ProxyURL + "<-"
        print "->"+ str(ws.ProxyPort) + "<-"
        print self.parameter      
        
        out = ws.run("xmledit", self.parameter)[1]
        
        dom = xd.parseString(out)
        if dom.getElementsByTagName("fault"):
            print "Error editing tags on repository"
            return
       
  
  
def main():
    
    if(len(sys.argv) < 4): #for test
       sys.argv = []
       sys.argv.append("testuser") #substitute
       sys.argv.append("6w8DHF") #substitute
       sys.argv.append('dataresource-3976,L0000_resource_MAF_Procedural_VMElinkURI1,dataresource-3978 dataresource-3977 ') #xml test present in repository
       
       edit = lhpEditRemoteTag()
       edit.EditTag()
       
    if(len(sys.argv) >= 4):
        sys.argv = sys.argv[1:]
        #print sys.argv
        edit = lhpEditRemoteTag()
        edit.EditTag()

if __name__ == '__main__':
    main()
        