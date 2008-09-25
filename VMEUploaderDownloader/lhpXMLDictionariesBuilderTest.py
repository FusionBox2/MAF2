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

class lhpXMLDictionariesBuilderTest(unittest.TestCase):
      
    def setUp(self):
        
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
            
    
    def testBuildMotionAnalysis(self):
         
        curDir = os.getcwd()
        absPath = curDir
        curDir  +=  "\lhpXMLDictionariesBuilderTestData"
        print curDir  
        master = curDir + "\LHDL_dictionary.xml"
        exist = os.path.exists(master)
        self.assertTrue(exist)
        
        motionSub = curDir + "\MotionAnalysis.xml"
        exist = os.path.exists(motionSub)
        self.assertTrue(exist)
        
        output  = curDir + "\masterDictionaryPlusMotionSubdictionary.xml"
        exist = os.path.exists(output)
        # self.assertFalse(exist)
        
        dictionaryBuilder = lhpXMLDictionariesBuilder.lhpXMLDictionariesBuilder()
        dictionaryBuilder.masterDictionaryFileName = master
        print "master: " + master
        dictionaryBuilder.subDictionaryFileName = motionSub
        print "motionSub: " + motionSub
        dictionaryBuilder.outputDictionaryFileName = output
        print "output: " + output
        dictionaryBuilder.BuildMotionAnalysisDictionary()
        
    def testBuildDicom(self):
        
        curDir = os.getcwd()
        absPath = curDir
        curDir  +=  "\lhpXMLDictionariesBuilderTestData"
        print curDir  
        master = curDir + "\LHDL_dictionary.xml"
        exist = os.path.exists(master)
        self.assertTrue(exist)
        
        dicomSub = curDir + "\DicomSource.xml"
        exist = os.path.exists(dicomSub)
        self.assertTrue(exist)
        
        output  = curDir + "\masterDictionaryPlusDicomSubdictionary.xml"
        exist = os.path.exists(output)
        # self.assertFalse(exist)
            
        dictionaryBuilder = lhpXMLDictionariesBuilder.lhpXMLDictionariesBuilder()
        dictionaryBuilder.masterDictionaryFileName = master
        dictionaryBuilder.subDictionaryFileName = dicomSub
        dictionaryBuilder.outputDictionaryFileName = output
        dictionaryBuilder.BuildDicomDictionary()
    
    def testBuildFA(self):
        
        curDir = os.getcwd()
        absPath = curDir
        curDir  +=  "\lhpXMLDictionariesBuilderTestData"
        print curDir  
        master = curDir + "\LHDL_dictionary.xml"
        exist = os.path.exists(master)
        self.assertTrue(exist)
        
        FASub = curDir + "\FA.xml"
        exist = os.path.exists(FASub)
        self.assertTrue(exist)
        
        output  = curDir + "\masterDictionaryPlusFASubdictionary.xml"
        exist = os.path.exists(output)
        # self.assertFalse(exist)
            
        dictionaryBuilder = lhpXMLDictionariesBuilder.lhpXMLDictionariesBuilder()
        dictionaryBuilder.masterDictionaryFileName = master
        dictionaryBuilder.subDictionaryFileName = FASub
        dictionaryBuilder.outputDictionaryFileName = output
        dictionaryBuilder.BuildFunctionalAnatomyDictionary()
        
if __name__ == '__main__':
    unittest.main()
    
