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
import urllib

import urllib, urllib2, base64, re, os, cookielib, sys
from HttpsProxy import *

class lhpRemoveResource:
      
    def __init__(self):
        
        self.userName = sys.argv[0]
        self.password = sys.argv[1]
        self.resourceToRemove = sys.argv[2]
         
        self.proxyHost = ""
        self.proxyPort = 0
        pass
        
    def removeResource(self):
        """ 
           Removes a XML resource on repository
        """
        self.proxyHost, self.proxyPort = retriveProxyParameters()
        if Debug:
            print "->"+ self.proxyHost + "<-"
            print "->"+ str(self.proxyPort) + "<-"
        
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setCredentials(self.userName, self.password)
        ws.setServer('https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2/')
        #ws.setServer('http://devel.fec.cineca.it:12680/town/biomed_town/LHDL/users/repository/lhprepository2/')
        ws.ProxyURL = self.proxyHost
        ws.ProxyPort = self.proxyPort
        
        # timeout in seconds
        self.Timeout = 30
        socket.setdefaulttimeout(self.Timeout)
        
        #check if resource exists on repository
        url = 'https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2/' + self.resourceToRemove
        
        page = urllib.urlopen(url)
        pagedata = page.read()
        result = pagedata.find('Resource not found')
        if(result != -1):
            return
        try:
            out = ws.run('xmldelete', self.resourceToRemove)[1]
        except:
            if Debug:
                print "-----------Error in xmldelete service------------"
            sys.exit(1)
        
        dom = xd.parseString(out)
        if dom.getElementsByTagName("fault"):
            if Debug:
                print "-----------Error in xmldelete service------------"
            for el in dom.getElementsByTagName("string"):
                for node in el.childNodes:  
                    error = node.data
            if Debug:
                print error
            sys.exit(1)

  
def main():
    
    if(len(sys.argv) != 4): #for test
        sys.argv = []
        sys.argv.append("testuser") #substitute
        sys.argv.append("6w8DHF") #substitute
        sys.argv.append("dataresource-3775") #substitute
        
        remove = lhpRemoveResource()
        remove.removeResource()
        #add code to remove resource created
        return
       
    if(len(sys.argv) == 4):
        sys.argv = sys.argv[1:]
        #print sys.argv
        remove = lhpRemoveResource()
        remove.removeResource()

if __name__ == '__main__':
    main()
        