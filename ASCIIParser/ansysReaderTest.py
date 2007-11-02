#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import os
import sys, string
import unittest


import ansysReader
import shutil
from pyparsing import Word, alphas, nums, ZeroOrMore, ParseException, Group, delimitedList, alphanums, Literal,Dict
import pprint

class ansysReaderTest(unittest.TestCase):
    
    def testReader(self):
        
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = r'D:\vapps\LHPBuilder_Parabuild\ASCIIParser\testData\ansys\tetra10_ansys_commented_ASCII_Converted.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber = cargo
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementDeclarationSectionsNumber, 2)
        self.assertEqual(elementTypeSectionsNumber, 2)
        self.assertEqual(materialsSectionsNumber,2)
       
       #nodes test data
       #1   3   3   3   0   1      5     2     4     3    11    10    14    13    9    12
       #2   2   2   3   0   1      3     1     2     4     7     6     9    12    8    10
    
    
    def estHugeAnsysFile(self):
        # approximately 4 minutes running for a 14 MB ansys file On a dual core Number 6600 with four gigabytes ram
        
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = r'D:\vapps\LHPBuilder_Parabuild\ASCIIParser\testData\ansys\2505_meshBV3_morgan.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        #nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               #materialsSectionsNumber = cargo
        
        #self.assertEqual(nodeSectionsNumber,1)
        #self.assertEqual(elementDeclarationSectionsNumber, 2)
        #self.assertEqual(elementTypeSectionsNumber, 2)
        #self.assertEqual(materialsSectionsNumber,2)
       
    
if __name__ == '__main__':
    unittest.main()
    
    
    