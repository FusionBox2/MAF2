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

import os

import urllib, urllib2, base64, re, os, cookielib, sys

class lhpDictionaryVersionChecker:
      
    def __init__(self):
        
        # URL
        self.Host = "www.biomedtown.org"
        self.DictionaryDownloadHTMLPageSelector = "http://www.biomedtown.org/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries/plfng_view"
        self.RemoteWarningPage = r"http://www.biomedtown.org/biomed_town/LHDL/users/swclient/DictionaryCheck/"
        
        # authentication
        self.Username = 'lhpparabuild'
        self.Password = '2bf5ZM'
        
        # init stuff
        self.DictionaryDownloadHTMLPageFileName = "DictionaryDownloadHTMLPage.htm"
        self.DictionaryCreationDate = "YYYYMMDDHHMM"
    
    def IsDictionaryUpToDate(self):
        """
           return True if lhp dictionary is updated otherwise return false
        """
        
        localDate = self.GetLocalDictionaryDate()
        remoteDate = self.GetRemoteDictionaryDate()
        
        if remoteDate > localDate:
            print "Your LHPBuilder software is not up to date and you're not allowed to upload with it! Please download the latest version."
            webbrowser.open(self.RemoteWarningPage)
            return False
        else:
            print "Your LHPBuilder software is up to date! You can safely upload your VME!"
            return  True
        
        
        
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
        
        print "Connecting to self.Host: " + self.Host            
        print "Retrieving: " + self.DictionaryDownloadHTMLPageSelector
        
        h = httplib.HTTPConnection(self.Host)
        h.putrequest('POST', self.DictionaryDownloadHTMLPageSelector)
        h.putheader("AUTHORIZATION", "Basic %s" % string.replace(
                                encodestring("%s:%s" % (self.Username, self.Password)),
                                "\012", ""))
        h.endheaders()
        
        #     
        f = open(self.DictionaryDownloadHTMLPageFileName, 'w')        
        f.write(h.getresponse().read())
        f.close()
    
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
        
            if re.search("(/>&nbsp;LHDL_dictionary.csv)$", line):
                if Debug:                   
                    print "Found LHDL_dictionary.csv at line: " + line
                
                while 1:
                    
                    line = file.readline() 
                    if re.search("^ *(<td>20)", line):
                        if Debug:                            
                            print "Found date line: " + line
                        date = line.strip().replace("<td>","").replace("</td>","")
                        if Debug:   
                            print date
                        dt = datetime(*strptime(date, "%Y-%m-%d %H:%M")[0:5])
                        st = ""
                        for num in dt.timetuple()[0:5]:
                            st = st + str(num)
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
            
        
def run():                                            
    
    # get the dictionary creation date
    dictVC = lhpDictionaryVersionChecker()
    if dictVC.IsDictionaryUpToDate() == True:
        print "UpToDate"
    else:
        print "NotUpToDate"
    
    
def main():
    args = sys.argv[1:]
    if len(args) != 1:
        print 
        """
        usage: python.exe lhpDictionaryVersionChecker.py
        """
        sys.exit(-1)
    print args
    run(args[0],args[1],args[2])


if __name__ == '__main__':    
    run()
 
