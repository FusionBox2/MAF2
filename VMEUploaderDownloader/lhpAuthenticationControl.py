#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import httplib, urlparse, string
from base64 import encodestring, decodestring

from Debug import Debug
import StringIO

from datetime import *
from time import *

import os

import socket
import urllib, urllib2, base64, re, os, cookielib, sys
from HttpsProxy import *

class lhpAuthenticationControl:
      
    def __init__(self, userName = "", password = ""):
        
        # URL
        self.Host = "www.biomedtown.org"
        self.Repository = ""

        self.AuthenticationHTMLPageSelector = "https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2"
        #self.AuthenticationHTMLPageSelector = "http://devel.fec.cineca.it:12680/town/biomed_town/LHDL/users/repository/lhprepository2/"
        self.RemoteWarningPage = r"https://www.biomedtown.org/biomed_town/LHDL/users/swclient/DictionaryCheck/"
        
        # authentication
        self.Username = userName
        self.Password = password

        #result
        self.AuthenticationHTMLPageFileName = "AuthenticationControlHTMLPage.htm"    

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
        
        # build opener
        self.cj = cookielib.CookieJar()
        
        proxy_url = self.ProxyURL
        proxy_port = self.ProxyPort        
        
        p = '%s:%s' % (proxy_url, proxy_port)        
        
        if proxy_url != '' and proxy_port != '':
            
            if Debug:
                print "You are using proxy: " + p

            self.opener = \
              urllib2.build_opener(
              ConnectHTTPHandler(proxy=p),
              ConnectHTTPSHandler(proxy=p),
              urllib2.HTTPCookieProcessor(self.cj))
        else:
            self.opener = \
              urllib2.build_opener(urllib2.HTTPCookieProcessor(self.cj))


        urllib2.install_opener(self.opener)  
        
        if Debug:
            print "Connecting to self.Host: " + self.Host            
            print "Retrieving: " + self.AuthenticationHTMLPageSelector 
        
        url = self.AuthenticationHTMLPageSelector 

        req = urllib2.Request(url)

        # Basic authentication code
        base64string = encodestring('%s:%s' % (self.Username, self.Password))[:-1]
        authheader =  "Basic %s" % base64string
        req.add_header("Authorization", authheader)
        
        # timeout in seconds
        self.Timeout = 15
        socket.setdefaulttimeout(self.Timeout)
        
        try:
            fd = urllib2.urlopen(req)
        except urllib2.URLError:
            print "Timeout Error!" 
            print "More than " + str(self.Timeout) + " seconds to connect to www.biomedtown.org..."
            print "Please  check your Internet connection"
            return False
            
       
        file = open(self.AuthenticationHTMLPageFileName, 'w' )       
        for content in fd:
            file.write(content)
        file.close()

        if Debug:
            f2 = open(self.AuthenticationHTMLPageFileName , 'r')
            for line in f2:
                print line
            f2.close()
        
        diskFile = open(self.AuthenticationHTMLPageFileName, 'r')
        file = StringIO.StringIO()
        fileLinesNumber = 0
        # read all the file in memory
        for curr in diskFile.readlines() :
            fileLinesNumber += 1
            file.write(curr)
        
        if Debug:           
            print "Input HTML file contains: " + str(fileLinesNumber) + " lines" 
        
        # go to the beginning of file
        file.seek(0)
        
        parsedLineNumber = 0
         
        # parse the file structure and gather informations
        #
        result = True
        while 1:
            # file first-line
            line = file.readline() 
            if re.search("You are not authorized to access this resource.", line):
              result = False
  
            if not line:
              break
        
        return result
            
        
def run(userName, password, proxyURL = '', proxyPort = ''):                                            
    
    # get the dictionary creation date
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
 
