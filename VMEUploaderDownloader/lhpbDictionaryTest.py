
import domParser
import sys, string
import unittest
import shutil

class lhpbDictionaryTest(unittest.TestCase):
      
    def setUp(self):
        self.inFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\msf_test_import_export_VME\testDictionary.txt'
        self.copyFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\msf_test_import_export_VME\testDictionaryCopy.txt'
        # shutil.copy(self.inFileName, self.copyFileName)
        self.dictionaryInstance = domParser.LHPBDictionary()
        
    def testSave(self):
        testDictionary = ['pippo','pluto','topolino']
        self.dictionaryInstance.DictionaryFileName = "testSaveDictionary.txt"
        self.dictionaryInstance.DictionaryTagsList = testDictionary
        self.dictionaryInstance.Save()
        
    def testLoad(self):
        newDict = domParser.LHPBDictionary()
        newDict.DictionaryFileName = "testSaveDictionary.txt"
        newDict.Load()
        for tag in newDict.DictionaryTagsList:
            print tag
        
    def testPrint(self):
        newDict = domParser.LHPBDictionary()
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
    
