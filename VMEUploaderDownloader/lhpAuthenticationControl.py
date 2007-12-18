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

import urllib, urllib2, base64, re, os, cookielib, sys

class lhpAuthenticationControl:
      
    def __init__(self, userName = "", password = ""):
        
        # URL
        self.Host = "www.biomedtown.org"
        self.Repository = ""

        self.AuthenticationHTMLPageSelector = "http://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository-test"
        self.RemoteWarningPage = r"http://www.biomedtown.org/biomed_town/LHDL/users/swclient/DictionaryCheck/"
        
        # authentication
        self.Username = userName
        self.Password = password

        #result
        self.AuthenticationHTMLPageFileName = "AuthenticationControlHTMLPage.htm"    

    def IsAuthenticated(self):
        """
           return True if user and password correspond to user that can write into repository
        """
        
        return self.GetRemoteAuthentication()


    def GetRemoteAuthentication(self):
        """ 
           Authentication for user and password
        """
        
        print "Connecting to self.Host: " + self.Host            
        print "Retrieving: " + self.AuthenticationHTMLPageSelector
        
        h = httplib.HTTPConnection(self.Host)
        h.putrequest('POST', self.AuthenticationHTMLPageSelector)
        h.putheader("AUTHORIZATION", "Basic %s" % string.replace(
                                encodestring("%s:%s" % (self.Username, self.Password)),
                                "\012", ""))
        h.endheaders()
        
        # 
        f = open(self.AuthenticationHTMLPageFileName, 'w')        
        f.write(h.getresponse().read())
        f.close()
    
        if Debug:
            f2 = open(self.AuthenticationHTMLPageFileName, 'r')
            for line in f2:
                print line
        
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
            
        
def run(userName, password):                                            
    
    # get the dictionary creation date
    auth = lhpAuthenticationControl(userName, password)
    if auth.IsAuthenticated() == True:
        print "Authenticated"
    else:
        print "Rejected"
    
    
def main():
    args = sys.argv[1:]
    if len(args) != 2:
        print 
        """
        usage: python.exe lhpAuthenticationControl.py username password
        """
        sys.exit(-1)
    run(args[0],args[1])


if __name__ == '__main__':    
    main()
 
