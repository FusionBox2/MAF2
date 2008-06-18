#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import lhpXMLDictionaryParser
import sys, string
from xml.dom import minidom
from xml.dom import Node

import Debug
import unittest
import shutil
import difflib
import pickle
import os

class lhpXMLDictionaryParserTest(unittest.TestCase):
      
    def setUp(self):
        
        print "Beware:  In order to work run this est must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
            
    def est(self):
        """test parser facilities"""
        xmlDict = r'.\csv2XMLTestData\LHDL_Resources_Taxonomy_v7c.xml'

        lhpXMLDictionaryParserInstance = lhpXMLDictionaryParser.lhpXMLDictionaryParser()
        lhpXMLDictionaryParserInstance.LoadXMLDictionary(xmlDict)
        lhpXMLDictionaryParserInstance.PrintXMLDictionary()
        
        tags =  lhpXMLDictionaryParserInstance.GetXMLDictionaryNodeNamesList()
        if Debug:
            print tags
            num = 0
            for i in tags:
                print str(num) + " " + str(i)
                num += 1
                
        self.assertEqual(len(tags), 100)
        
        tatags = lhpXMLDictionaryParserInstance.GetVMETagArrayTagNamesList()
        self.assertEqual(len(tatags), 100)
        for i in tatags:
            print i
        
        autoTags = lhpXMLDictionaryParserInstance.GetXMLDictionaryAutoNodeNamesList()
        print "number of auto tags: " + str(len(autoTags))
        
        assert(len(autoTags) < 100)
        
        vmeAutoTags = lhpXMLDictionaryParserInstance.GetVMETagArrayAutoTagNamesList()
        print vmeAutoTags
        self.assertEqual(len(autoTags), len(vmeAutoTags))
        
        vmeManualTags = lhpXMLDictionaryParserInstance.GetVMETagArrayManualTagNamesList()
        print vmeManualTags
        self.assertEqual(len(autoTags) + len(vmeManualTags), 100)
    
    def estRunAutoTags(self):
        xmlDict = r'.\csv2XMLTestData\LHDL_Resources_Taxonomy_v7c.xml'
        lhpXMLDictionaryParser.run(xmlDict,"auto_tags", "auto_tags.txt")
        
    def testRunAutoTags2(self):
        xmlDict = r'.\csv2XMLTestData\lhpXMLDictionary.xml'
        lhpXMLDictionaryParser.run(xmlDict,"auto_tags", "auto_tags.txt")
    
    def estRunAutoTags(self):
        xmlDict = r'.\csv2XMLTestData\LHDL_Resources_Taxonomy_v7c.xml'
        lhpXMLDictionaryParser.run(xmlDict,"manual_tags", "manual_tags.txt")
        
if __name__ == '__main__':
    unittest.main()
    
