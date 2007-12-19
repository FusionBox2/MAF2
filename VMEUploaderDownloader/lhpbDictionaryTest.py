#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

import msfParser
import sys, string
import unittest
import shutil
import os

class lhpTagsListStorageTest(unittest.TestCase):
      
    def setUp(self):
        
        print "\nBeware:  In order to work this test must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
       
        self.inFileName = curDir + r'\msf_test_import_export_VME\testDictionary.txt'
        self.copyFileName = curDir + r'\msf_test_import_export_VME\testDictionaryCopy.txt'
        # shutil.copy(self.inFileName, self.copyFileName)
        self.dictionaryInstance = msfParser.lhpTagsListStorage()
        
    def testSave(self):
        testDictionary = ['pippo','pluto','topolino']
        self.dictionaryInstance.DictionaryFileName = "testSaveDictionary.txt"
        self.dictionaryInstance.DictionaryTagsList = testDictionary
        self.dictionaryInstance.Save()
        
    def testLoad(self):
        newDict = msfParser.lhpTagsListStorage()
        newDict.DictionaryFileName = "testSaveDictionary.txt"
        newDict.Load()
        for tag in newDict.DictionaryTagsList:
            print tag
        
    def testPrint(self):
        newDict = msfParser.lhpTagsListStorage()
        newDict.DictionaryFileName = "testSaveDictionary.txt"
        newDict.Load()
        newDict.Print()
    
    def testSaveAndReload(self):
        testDictionary = ['pippo','pluto','topolino']
        self.dictionaryInstance.DictionaryFileName = "testSaveDictionary.txt"
        self.dictionaryInstance.DictionaryTagsList = testDictionary
        self.dictionaryInstance.Save()
        self.dictionaryInstance.Clear()
        
        self.dictionaryInstance.DictionaryFileName = "testSaveDictionary.txt"
        self.dictionaryInstance.Load()
        self.assertEqual(self.dictionaryInstance.DictionaryTagsList, testDictionary)

if __name__ == '__main__':
    unittest.main()
    
