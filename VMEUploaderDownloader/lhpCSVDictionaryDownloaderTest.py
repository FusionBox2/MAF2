#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import sys
sys.path.append('./webServicesClient')

import httplib, urlparse, string
from base64 import encodestring, decodestring
import lhpCSVDictionaryDownloader

from Debug import Debug
import unittest
import msfParser

from xml.dom import minidom
from xml.dom import Node
from datetime import *
from time import *

import StringIO
import os


import urllib, urllib2, base64, re, os, cookielib, sys

class lhpCSVDictionaryDownloaderTest(unittest.TestCase):
      
    def setUp(self):
        
        print "Beware:  In order to work run this est must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
          
    
    def testDownloadMasterDictionaryHTTPS(self):
        
        # https://www.biomedtown.org/biomed_town/LHDL/users/swclient/dictionaries/LHDL_dictionary
        host = "www.biomedtown.org"
        selector = "/biomed_town/LHDL/users/swclient/dictionaries/LHDL_dictionary"
        
        outputXMLDictionaryFileName = "lhpXMLDictionary"
        
        lhpCSVDictionaryDownloader.run(host,selector,outputXMLDictionaryFileName)
    
        
        
    def testDownloadFAOntologySubdictionaryHTTPS(self):
        
        # https://www.biomedtown.org/biomed_town/LHDL/users/swclient/dictionaries/LHDL_dictionary
        host = "www.biomedtown.org"
        selector = "/biomed_town/LHDL/users/swclient/dictionaries/FA_onto"
        
        outputXMLDictionaryFileName = "lhpXMLFAOntologySourceySubdictionary"
        
        lhpCSVDictionaryDownloader.run(host,selector,outputXMLDictionaryFileName)
        
    def testRun(self):
        
        host = "www.biomedtown.org"
        selector = "/biomed_town/LHDL/users/swclient/dictionaries/LHDL_dictionary"
        
        outputXMLDictionaryFileName = "lhpXMLDictionary"
        
        lhpCSVDictionaryDownloader.run(host,selector,outputXMLDictionaryFileName)
    
    # 
    # TODO: these tests needs little editing to work... 
    # 
    
    def estDownloadDictionary(self):
        
        dd = lhpCSVDictionaryDownloader.lhpCSVDictionaryDownloader()
        dd.RemoveOldDictionariesFromDisk()
        dd.DownloadCSVDictionary()
        dd.ConvertDownloadedCSV2XML()
      
    def estDownloadMultipleDictionaries(self):
  
        #host = "www.biomedtown.org"
        
        #selector = "/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries/LHDL_dictionary.csv"    
        #outputXMLMasterDictionaryFileName = "lhpXMLDictionary"
        
        #lhpCSVDictionaryDownloader.run(host,selector,outputXMLMasterDictionaryFileName)
        
        #selector = "/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries/DicomSource.csv"
        #outputXMLDicomDictionaryFileName = "lhpXMLDicomSourceSubdictionary"

        #lhpCSVDictionaryDownloader.run(host,selector,outputXMLDicomDictionaryFileName)
        
        #selector = "/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries/MotionAnalysis.csv"
        #outputXMLMotionAnalysisDictionaryFileName = "lhpXMLMotionAnalysisSourceSubdictionary"

        #lhpCSVDictionaryDownloader.run(host,selector,outputXMLMotionAnalysisDictionaryFileName)
        
        # append subdictionary to master
        
        # find dictionary filenames...
        # TODO!!!!!
        masterDictionaryFileName = "lhpXMLDictionary_200712181319.xml"
        motionAnalysisDictionaryFileName = "lhpXMLMotionAnalysisSourceSubdictionary_200712181319.xml"
        dicomDictionaryFileName = "lhpXMLDicomSourceSubdictionary_200712181319.xml"
        
        # load master dictionary
        msfDOMParserInstance = msfParser.msfParser()        
        masterDomDocument = minidom.parse(masterDictionaryFileName)
        masterRootNode = masterDomDocument.documentElement
        
        # get the master subdir node
        sourceDirNode = msfDOMParserInstance.GetNodeByNodeName(masterRootNode,"SourceDir")
        
        self.assertTrue(masterRootNode != None)
        
        # load motionAnalysys dictionary
        motionAnalysisSubictionaryDomDocument = minidom.parse(motionAnalysisDictionaryFileName)
        motionAnalysisRootNode = motionAnalysisSubictionaryDomDocument.documentElement
        
        # get motionAnalysys parent node
        motionAnalysisTypeNode = msfDOMParserInstance.GetNodeByNodeName(motionAnalysisRootNode,"Type")
        
        self.assertTrue(motionAnalysisTypeNode != None)
       
        sourceDirNode.appendChild(motionAnalysisTypeNode)
        
        newDoc = minidom.Document()
        newDoc.appendChild(masterRootNode)
        
        
        outputVMEXMLName = "dictionaryPlusSubdictionary.xml"
        outFileXML = open(outputVMEXMLName, 'w')
        newDoc.writexml(outFileXML)
     
        
        
if __name__ == '__main__':
     unittest.main()
    
