#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import httplib, urlparse, string
from base64 import encodestring, decodestring

import lhpDictionaryVersionChecker
import dictionaryCSV2XML
from Debug import Debug
import StringIO
import webbrowser

from datetime import *
from time import *
from HttpsProxy import *

import os

import urllib, urllib2, base64, re, os, cookielib, sys

class lhpDictionaryVersionChecker:
      
    def __init__(self):
        
        # URL
        self.Host = "www.biomedtown.org"
        self.DictionaryDownloadHTMLPageSelector = "https://www.biomedtown.org/biomed_town/LHDL/users/swclient/dictionaries"
        self.RemoteWarningPage = r"http://www.biomedtown.org/biomed_town/LHDL/users/swclient/DictionaryCheck/"
        
        
        # authentication
        self.Username = 'lhpparabuild'
        self.Password = '2bf5ZM'
        
        # init stuff
        self.DictionaryDownloadHTMLPageFileName = "DictionaryDownloadHTMLPage.htm"
        self.DictionaryCreationDate = "YYYYMMDDHHMM"
    
        # proxy
        self.ProxyURL = ''
        self.ProxyPort = ''
        
    def IsDictionaryUpToDate(self):
        """
           return True if lhp dictionary is updated otherwise return false
        """
        
        # remove older dictionaries
        self.RemoveOldDictionaries("lhpXMLDictionary_")
        self.RemoveOldDictionaries("lhpXMLDicomSourceSubdictionary_")
        self.RemoveOldDictionaries("lhpXMLMotionAnalysisSourceSubdictionary_")
                
        localDate = self.GetLocalDictionaryDate()
        
        
        remoteDate = self.GetRemoteDictionaryDate()
        
        if remoteDate > localDate:
            print "Your LHPBuilder software is not up to date and you're not allowed to upload with it! Please download the latest version."
            webbrowser.open(self.RemoteWarningPage)
            return False
        else:
            print "Your LHPBuilder software is up to date! You can safely upload your VME!"
            return  True
        
    def RemoveOldDictionaries(self, prefix):
        
        files = os.listdir(".")
        
        if Debug:
            print "\nCurrent Working Directory: " + os.getcwd() + " contains: "
            print files
        
        dictionariesFilesList= []
        
        for file in files:
            if re.search('^(' + prefix + ').*(\.xml)$',file):
               dictionariesFilesList.append(os.getcwd() + '\\' + file)
            
        timeSortedDictionaries = sorted(dictionariesFilesList)
        timeSortedDictionaries.reverse()
        
        if Debug:
            print timeSortedDictionaries
            
        for file in timeSortedDictionaries[1:]:
            if Debug:
                print "Removing: " + file
            os.remove(file)      
        
        if Debug:
            print "Local dictionary is: " + str(timeSortedDictionaries[0])
            
    def GetLocalDictionaryDate(self):
        """
           search for a 
        """
        
        # List files inside cwdir directory
        files = os.listdir(".")
        
        if Debug:
            print "\nCurrent Working Directory: " + os.getcwd() + " contains: "
            print files
            
        dictionaryFilesList = []
        
        for file in files:
            if re.search('^(lhpXMLDictionary_)',file):
               dictionaryFilesList.append(file)
        
        # assert there is only one xml dict file
        assert(len(dictionaryFilesList)  == 1)
        
        dictionaryFileName = str(dictionaryFilesList[0])
        
        if Debug:      
            print "\nOk, found only one XML dictionary file named:" + dictionaryFileName
            
        startDatePos = dictionaryFileName.find("_") + 1
        
        # retrieve its date
        date = dictionaryFileName[startDatePos:len(dictionaryFileName)-len(".xml")]
        
        if Debug:           
            print "Returning: " + date
        
        
        return int(date)
        
            
    def GetRemoteDictionaryDate(self):
        """ 
           Parse dictionary webpage and return dictionary creation datetime as a long, for example from 2007-12-13 17:56
           returns 200712131756, if successful otherwise return -1
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
            print "Retrieving: " + self.DictionaryDownloadHTMLPageSelector
        
        url = self.DictionaryDownloadHTMLPageSelector

        req = urllib2.Request(url)

        # Basic authentication code
        base64string = encodestring('%s:%s' % (self.Username, self.Password))[:-1]
        authheader =  "Basic %s" % base64string
        req.add_header("Authorization", authheader)

        # open the url
        fd = urllib2.urlopen(req)
       
        file = open(self.DictionaryDownloadHTMLPageFileName, 'w')       
        for content in fd:
            file.write(content)
        file.close()

        if Debug:
        
            f2 = open(self.DictionaryDownloadHTMLPageFileName, 'r')
            for line in f2:
                print line
        
        diskFile = open(self.DictionaryDownloadHTMLPageFileName, 'r')
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
        while 1:
            
            # file first-line
            line = file.readline() 
            
            # if Debug:
            #     print "main loop is parsing: " + str(parsedLineNumber) + " > " + line
        
            if re.search("\">LHDL_dictionary</a>", line):
                if Debug:                   


                    print "Found LHDL_dictionary.csv at line: " + line
                    print line
                
                while 1:
                    
                    line = file.readline() 
                    if re.search("(19|20)\d\d", line):
                        if Debug:                            

                            print "Found date line: " + line

                        date = line.strip()
                        if Debug:   
                            print date
                      
                        dt = datetime(*strptime(date, "%Y-%m-%d %H:%M")[0:5])
                        st = ""
                        for num in dt.timetuple()[0:5]:
                            if Debug:
                                print "current date number:"  + str(num)
                            
                            # add padding 0 for dates like:
                            # 2008-03-06 01:01 to obtain 200803060101 instead of 200803611 
                            if num >=  10:                                
                                st = st + str(num)
                            else:
                                st = st + "0" + str(num)
                                
                            if Debug:
                                print st
                     
                        
                        int_val = -1
                        try:
                            int_val = int(st)                           
                        except ValueError:
                            print "error converting to int"
                            return
                        self.DictionaryCreationDate = int_val
                        return int_val
                    parsedLineNumber += 1
                    
                    # when you reach the end of file... 
                    if not line:
                        return -1
                
            # when you reach the end of file... 
            if not line: 
                return -1
            
            parsedLineNumber += 1
        
def run(proxyURL = '', proxyPort = ''):                                            
    
    # get the dictionary creation date
    dictVC = lhpDictionaryVersionChecker()
    dictVC.ProxyURL = proxyURL
    dictVC.ProxyPort = proxyPort
    if dictVC.IsDictionaryUpToDate() == True:
        print "UpToDate"
    else:
        print "NotUpToDate"
    
if __name__ == '__main__':    

    if len(sys.argv) != 1 and len(sys.argv) != 3:
        print """
        usage: python.exe lhpDictionaryVersionChecker.py proxyURL proxyPort
        The last two arguments are optional: if a proxy is not provided it will not be used
        """
        sys.exit(-1)
        
    if len(sys.argv) == 1:
        proxyURL = ''
        proxyPort = ''
    else:
        proxyURL = sys.argv[1]
        proxyPort = sys.argv[2]
    
    run(proxyURL,proxyPort)