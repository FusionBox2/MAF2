#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Daniele Giunchi <d.giunchi@scsolutions.it>
#-----------------------------------------------------------------------------

import os
import sys, string, time
from cStringIO import StringIO
import unittest

from Debug  import Debug
import ansysCDBReader
import shutil
from pyparsing import Word, alphas, nums, ZeroOrMore, ParseException, Group, delimitedList, alphanums, Literal,Dict, Suppress
import pprint

class ansysReaderTest(unittest.TestCase):
    
    def setUp(self):
        self.StartDir =  os.getcwd()
        print self.StartDir
        
    def tearDown(self):
        os.chdir(self.StartDir)
        
    def testReader(self):
        
        
        # create the reader
        ar = ansysCDBReader.ansysReader()
        ar.CDBAnsysFileName = self.StartDir + r'\testData\ansys\tet10_2elem.cdb'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementsSectionNumber, componentSectionNumbers, materialsMPDATASectionsNumber = cargo
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementsSectionNumber, 1)
        self.assertEqual(componentSectionNumbers, 1)
        self.assertEqual(materialsMPDATASectionsNumber,4)
        time.sleep(1)
        
    def testReaderMidSizeFile(self):
        
        
        # create the reader
        ar = ansysCDBReader.ansysReader()
        ar.CDBAnsysFileName = self.StartDir + r'\testData\ansys\mid_one.cdb'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementsSectionNumber, componentSectionNumbers, materialsMPDATASectionsNumber = cargo     
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementsSectionNumber, 1)
        self.assertEqual(componentSectionNumbers, 0)
        self.assertEqual(materialsMPDATASectionsNumber,1455) #485 materials * 3 (EX,NUXY,DENS)
        time.sleep(1)
    
    def _testReaderBigSizeFile(self):
        
        
        # create the reader
        ar = ansysCDBReader.ansysReader()
        ar.CDBAnsysFileName = self.StartDir + r'\testData\ansys\big_one.cdb'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        
if __name__ == '__main__':
    unittest.main()
    
    
    