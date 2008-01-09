#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import lhpXMLDictionariesBuilder
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
            
    
    def testBuildMotionAnalysis(self):
        
        dictionaryBuilder = lhpXMLDictionariesBuilder.lhpXMLDictionariesBuilder()
        dictionaryBuilder.masterDictionaryFileName = "lhpXMLDictionary_200712181319.xml"
        dictionaryBuilder.subDictionaryFileName = "lhpXMLMotionAnalysisSourceSubdictionary_200712181319.xml"
#         dicomDictionaryFileName = "lhpXMLDicomSourceSubdictionary_200712181319.xml"
        dictionaryBuilder.outputDictionaryFileName = "dictionaryPlusSubdictionary.xml"
        dictionaryBuilder.BuildMotionAnalysisDictionary()
        
if __name__ == '__main__':
    unittest.main()
    
