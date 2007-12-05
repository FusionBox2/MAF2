#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import httplib, urlparse, string
from base64 import encodestring, decodestring

import lhpCSVDictionaryDownloader
import dictionaryCSV2XML
import Debug

import os


import urllib, urllib2, base64, re, os, cookielib, sys

class lhpCSVDictionaryDownloader:
      
    def __init__(self):
        
        # URL
        self.Host = "www.biomedtown.org"
        self.Selector = "/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries/LHDL_dictionary.csv"
        
        # authentication
        self.Username = 'lhpparabuild'
        self.Password = '2bf5ZM'
        
        # filenames
        self.DownloadedCSVDictionaryFileName = "lhpDictionary.csv"
        self.OutputXMLDictionaryFileName = "lhpDictionary.xml"
        
    def DownloadCSVDictionary(self):
        
        print "Connecting to self.Host: " + self.Host            
        print "Retrieving: " + self.Selector
        
        h = httplib.HTTPConnection(self.Host)
        h.putrequest('POST', self.Selector)
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
                
    def ConvertDownloadedCSV2XML(self):
        csv2xml = dictionaryCSV2XML.dictionaryCSV2XML()
        csv2xml.InputCSVDictionaryFileName = self.DownloadedCSVDictionaryFileName
        csv2xml.OutputXMLDictionaryFileName = self.OutputXMLDictionaryFileName
        csv2xml.BuildXMLDictionary()
        
        
def run(host, selector, outputXMLDictionaryFileName):                                            
    
    dictDownloader = lhpCSVDictionaryDownloader()    

    dictDownloader.Host = host
    
    dictDownloader.Selector = selector
    dictDownloader.OutputXMLDictionaryFileName = outputXMLDictionaryFileName 
    
    dictDownloader.DownloadCSVDictionary()
    dictDownloader.ConvertDownloadedCSV2XML()
    
def main():
    args = sys.argv[1:]
    if len(args) != 3:
        print """
        usage: python.exe lhpCSVDictionaryDownloader.py
        host selector outputXMLDictionaryFileName.xml 
        
        For example:
        host = "www.biomedtown.org"
        selector = "/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries/LHDL_dictionary.csv"

        """
        sys.exit(-1)
    print args
    run(args[0],args[1],args[2])

if __name__ == '__main__':
    main()
    
