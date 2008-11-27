#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import httplib, urlparse, string
from base64 import encodestring, decodestring

import lhpCSVDictionaryDownloader
import lhpDictionaryVersionChecker
import dictionaryCSV2XML
from Debug import Debug
import StringIO

from datetime import *
from time import *


import os

import urllib, urllib2, base64, re, os, cookielib, sys

class lhpCSVDictionaryDownloader:
    """Download selected csv dictionary from biomedtown and convert it to xml\
    See main() comments for further details"""

    def __init__(self):
        
        # URL
        self.Host = "www.biomedtown.org"
        self.DictionaryFileSelector = "/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries/LHDL_dictionary.csv"
        
        # authentication
        self.Username = 'lhpparabuild'
        self.Password = '2bf5ZM'
        
        # filenames
        self.DownloadedCSVDictionaryFileName = "lhpCSVDictionary.csv"
        self.OutputXMLDictionaryFileName = "lhpXMLDictionary"
    
    def RemoveOldDictionariesFromDisk(self):
        # List files inside cwdir directory
        files = os.listdir(".")
        
        if Debug:
            print "\nCurrent Working Directory: " + os.getcwd() + " contains: "
            print files
        
        dictionariesFilesList= []
        
        for file in files:
            if re.search('^(' + self.OutputXMLDictionaryFileName + ').*(\.xml)$',file):
               dictionariesFilesList.append(os.getcwd() + '\\' + file)
            elif re.search(self.DownloadedCSVDictionaryFileName ,file):
               dictionariesFilesList.append(os.getcwd() + '\\' + file)
        
        # assert there is only one xml dict file
        # assert(len(dictionaryFilesList)  >= 2)
        
        for file in dictionariesFilesList:
            if Debug:
                print "Removing: " + file
        
#         dictionaryFileName = str(dictionaryFilesList[0])
        
            os.remove(file)
            
        
        # os.remove()
        # if Debug:      
          #   print "\nOk, found only one XML dictionary file named:" + dictionaryFileName
            
        
        
    def DownloadCSVDictionary(self):
        
        if Debug:
            print "Connecting to self.Host: " + self.Host            
            print "Retrieving: " + self.DictionaryFileSelector
        
        h = httplib.HTTPSConnection(self.Host)
        h.putrequest('POST', self.DictionaryFileSelector)
        h.putheader("AUTHORIZATION", "Basic %s" % string.replace(
                                encodestring("%s:%s" % (self.Username, self.Password)),
                                "\012", ""))
        h.endheaders()
        
        f = open(self.DownloadedCSVDictionaryFileName, 'w')
        
        f.write(h.getresponse().read())
        f.close()
        
        if Debug:    
            f2 = open(self.DownloadedCSVDictionaryFileName, 'r')
            for line in f2:
                print line
                
                
        return 0;
    
            
        
    def ConvertDownloadedCSV2XML(self):
        csv2xml = dictionaryCSV2XML.dictionaryCSV2XML()
        csv2xml.InputCSVDictionaryFileName = self.DownloadedCSVDictionaryFileName
        csv2xml.OutputXMLDictionaryFileName = self.OutputXMLDictionaryFileName

        csv2xml.BuildXMLDictionary()
        
        
def run(host, DictionaryFileSelector, outputXMLDictionaryFileName):                                            
    
    dictDownloader = lhpCSVDictionaryDownloader()    
    dictVersionChecker = lhpDictionaryVersionChecker.lhpDictionaryVersionChecker()
    dictDownloader.Host = host
    dictDownloader.OutputXMLDictionaryFileName = outputXMLDictionaryFileName
    
    dictDownloader.RemoveOldDictionariesFromDisk()
    
    # get the dictionary creation date
    date = dictVersionChecker.GetRemoteDictionaryDate()
    if date == -1:
        print " Cannot retrieve dictionary creation date: dictionary will not be created!"
        return -1 
    
    dictDownloader.DictionaryFileSelector = DictionaryFileSelector
    dictDownloader.OutputXMLDictionaryFileName = outputXMLDictionaryFileName  + "_" + str(date) + ".xml"
    
    dictDownloader.DownloadCSVDictionary()
    
    dictDownloader.ConvertDownloadedCSV2XML()
    
def main():
    args = sys.argv[1:]
    if len(args) != 3:
        print """
        usage: python.exe lhpCSVDictionaryDownloader.py
        host DictionaryFileSelector outputXMLDictionaryFileName
        
        For example:
        host = "www.biomedtown.org"
        DictionaryFileSelector = "/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries/LHDL_dictionary.csv"

        Notes:
        "DateTime" and ".xml" strings will be appended to outputXMLDictionaryFileName
        """
        sys.exit(-1)
    print args
    run(args[0],args[1],args[2])

    

if __name__ == '__main__':
    main()