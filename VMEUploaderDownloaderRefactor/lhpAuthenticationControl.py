#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

from webServicesClient import xmlrpcDemoWS

import httplib, urlparse, string
from base64 import encodestring, decodestring

from Debug import Debug
import StringIO

from datetime import *
from time import *

import socket
import os

import urllib, urllib2, base64, re, os, cookielib, sys
from HttpsProxy import *

Debug = 1

class lhpAuthenticationControl:
      
    def __init__(self, userName = "", password = ""):
                
        # authentication
        self.Username = userName
        self.Password = password

        # proxy
        self.ProxyURL = ''
        self.ProxyPort = ''
      
    def IsAuthenticated(self):
        """
           return True if user and password correspond to user that can write into repository
        """
        
        return self.GetRemoteAuthentication()


    def GetRemoteAuthentication(self):
        """ 
           Authentication for user and password
        """
                 
        proxy_url = self.ProxyURL
        proxy_port = self.ProxyPort        
        
        p = '%s:%s' % (proxy_url, proxy_port)        
        
        if proxy_url != '' and proxy_port != '':
            
            if Debug:
                print "You are using proxy: " + p  
        
        if Debug:
            print "Performing user authentication:"
            
         # timeout in seconds
        self.Timeout = 30
        socket.setdefaulttimeout(self.Timeout)
           
        try:

            ws = xmlrpcDemoWS.xmlrpc_demoWS()
            ws.setServer("https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2/")
            ws.setCredentials(self.Username , self.Password)
            
            if proxy_url != '' and proxy_port != '':
                ws.setProxy(proxy_url, proxy_port)
                
            res = "<?xml version='1.0'?>\n<methodResponse>\n<params>\n<param>\n<value><boolean>1</boolean></value>\n</param>\n</params>\n</methodResponse>\n"
            out = ws.run('authservice')
           
        except urllib2.URLError:
            
            print "Authorization problem!" 
            print "Please check your connection settings!"
            return False
        
        return True          
        
def run(userName, password, proxyURL = '', proxyPort = ''):                                            
    
    auth = lhpAuthenticationControl(userName, password)
    auth.ProxyURL = proxyURL
    auth.ProxyPort = proxyPort
    
    if auth.IsAuthenticated() == True:
        print "Authenticated"
    else:
        print "Rejected"
    
    
if __name__ == '__main__':    
    
    if len(sys.argv) != 3 and len(sys.argv) != 5:
        print """
        usage: python.exe lhpAuthenticationControl.py username password proxyURL proxyPort
        The last two arguments are optional: if a proxy is not provided it will not be used
        """
        sys.exit(-1)
   
    username = sys.argv[1]
    password = sys.argv[2]
    
    if len(sys.argv) == 3:
        proxyURL = ''
        proxyPort = ''
    else:
        proxyURL = sys.argv[3]
        proxyPort = sys.argv[4]
    
    run(username,password,proxyURL,proxyPort)
 
