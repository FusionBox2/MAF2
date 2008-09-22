#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------
from string import split
from platform import node
from inspect import isclass

import lhpXMLDictionaryParser
import sys, string
from xml.dom import minidom

from xml.dom.minidom import Node

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
            
        
    def testGetTagListsMethods(self):      
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
        
        assert(len(autoTags) == 69)
        
        vmeAutoTags = lhpXMLDictionaryParserInstance.GetVMETagArrayAutoTagNamesList()
        print vmeAutoTags
        
        self.assertEqual(len(autoTags), len(vmeAutoTags))
        
        vmeManualTags = lhpXMLDictionaryParserInstance.GetVMETagArrayManualTagNamesList()
        print vmeManualTags
        assert(len(vmeManualTags) == 31)   
        
        self.assertEqual(len(autoTags) + len(vmeManualTags), 100)

    def testRunForAutoTagsFileGeneration(self):
        xmlDict = r'.\csv2XMLTestData\LHDL_Resources_Taxonomy_v7c.xml'
        lhpXMLDictionaryParser.run(xmlDict,"auto_tags", "auto_tags.txt")
        
    def testRunForAutoTagsFileGeneration2(self):
        xmlDict = r'.\csv2XMLTestData\lhpXMLDictionary.xml'
        lhpXMLDictionaryParser.run(xmlDict,"auto_tags", "auto_tags.txt")
    
    def testRunForManualTagsFileGeneration(self):
        xmlDict = r'.\csv2XMLTestData\LHDL_Resources_Taxonomy_v7c.xml'
        lhpXMLDictionaryParser.run(xmlDict,"manual_tags", "manual_tags.txt")
        
    def testPrintDictionary(self):
        
        xmlDict = r'.\metadataEditorTestData\xmlDictionaries\LHDL_dictionary.xml'

        lhpXMLDictionaryParserInstance = lhpXMLDictionaryParser.lhpXMLDictionaryParser()
        lhpXMLDictionaryParserInstance.LoadXMLDictionary(xmlDict)
        lhpXMLDictionaryParserInstance.PrintXMLDictionary()
            
    def testGetChildNodeByName(self):
        xmlDict = r'.\metadataEditorTestData\xmlDictionaries\LHDL_dictionary.xml'

        lhpXMLDictionaryParserInstance = lhpXMLDictionaryParser.lhpXMLDictionaryParser()
        lhpXMLDictionaryParserInstance.LoadXMLDictionary(xmlDict)
        
        pi = lhpXMLDictionaryParserInstance
                
        domDocumentNode  = lhpXMLDictionaryParserInstance.DictionaryDOMDocument
        
        """ domDocumentNode
            ElementNode Name: L0000
                ElementNode Name: resource
                        ElementNode Name: data
                            ElementNode Name: DataType
                        ElementNode Name: service    
        """
        
        self.assertTrue(pi.GetChildNodeByName(domDocumentNode, "L0000") != None)
        self.assertTrue(pi.GetChildNodeByName(domDocumentNode, "pippo")  == None)
        
        L0000Node = pi.GetChildNodeByName(domDocumentNode, "L0000")
        
        self.assertTrue(pi.GetChildNodeByName(L0000Node, "resource") != None)
        self.assertTrue(pi.GetChildNodeByName(L0000Node, "data") == None)
        
        resourceNode = pi.GetChildNodeByName(L0000Node, "resource")
        self.assertTrue(pi.GetChildNodeByName(resourceNode, "data") != None)
        self.assertTrue(pi.GetChildNodeByName(resourceNode, "service") != None)
        self.assertTrue(pi.GetChildNodeByName(resourceNode, "DataType") == None)
        
    def testGetAttributesDictionary(self):
        xmlDict = r'.\metadataEditorTestData\xmlDictionaries\LHDL_dictionary.xml'

        lhpXMLDictionaryParserInstance = lhpXMLDictionaryParser.lhpXMLDictionaryParser()
        lhpXMLDictionaryParserInstance.LoadXMLDictionary(xmlDict)
        
        pi = lhpXMLDictionaryParserInstance
                
        domDocumentNode  = lhpXMLDictionaryParserInstance.DictionaryDOMDocument
        
        """ domDocumentNode
            ElementNode Name: L0000
                ElementNode Name: resource
                        ElementNode Name: data
                            ElementNode Name: DataType
                        ElementNode Name: service    
        """
        
        self.assertTrue(pi.GetChildNodeByName(domDocumentNode, "L0000") != None)
        self.assertTrue(pi.GetChildNodeByName(domDocumentNode, "pippo")  == None)
        
        L0000Node = pi.GetChildNodeByName(domDocumentNode, "L0000")
        d = pi.GetAttributesDictionary(L0000Node)
        self.assertTrue(d == {})
        
        self.assertTrue(pi.GetChildNodeByName(L0000Node, "resource") != None)
        self.assertTrue(pi.GetChildNodeByName(L0000Node, "data") == None)
        
        resourceNode = pi.GetChildNodeByName(L0000Node, "resource")
        
        d = pi.GetAttributesDictionary(resourceNode)
        print d
        self.assertTrue(d["Expert"]  ==  "Viceconti") 
        self.assertTrue(d["Searchable"]  ==  "n") 
        self.assertTrue(d["Multiplicity"]  ==  "1") 
        self.assertTrue(d["DefaultValue"]  ==  "") 
        self.assertTrue(d["Editable"]  ==  "n") 
        self.assertTrue(d["ValueList"]  ==  "") 
        self.assertTrue(d["Notes"]  ==  "Root concept; everything stored on LHDL is a resource") 
        self.assertTrue(d["ValueType"]  ==  "tag")
                

    def testGetNodeFromVMETagArrayTagName(self):
        xmlDict = r'.\metadataEditorTestData\xmlDictionaries\LHDL_dictionary.xml'

        lhpXMLDictionaryParserInstance = lhpXMLDictionaryParser.lhpXMLDictionaryParser()
        lhpXMLDictionaryParserInstance.LoadXMLDictionary(xmlDict)
        
        pi = lhpXMLDictionaryParserInstance
        
        node = None
        
        node = pi.GetNodeFromVMETagArrayTagName("L0000")
        self.assertTrue(node != None)
        self.assertTrue(pi.GetNodeName(node) == "L0000")
        
        node = pi.GetNodeFromVMETagArrayTagName("L0000_resource")
        self.assertTrue(node != None)
        self.assertTrue(pi.GetNodeName(node) == "resource")
        
        node = pi.GetNodeFromVMETagArrayTagName("L0000_resource_MAF")
        self.assertTrue(node != None)
        self.assertTrue(pi.GetNodeName(node) == "MAF")
        
        self.assertTrue(pi.GetNodeFromVMETagArrayTagName("L0000_resource")  != None)
        self.assertTrue(pi.GetNodeFromVMETagArrayTagName("L0000_resourcz")   == None)
        self.assertTrue(pi.GetNodeFromVMETagArrayTagName("L0000_resource_MAF")  != None)
        self.assertTrue(pi.GetNodeFromVMETagArrayTagName("L0000_resource_MAF_TreeInfo")  != None)
        self.assertTrue(pi.GetNodeFromVMETagArrayTagName("L0000_resource_MAF_TreeInfo_VmeRootURI")  != None)
        
if __name__ == '__main__':
    unittest.main()
    
