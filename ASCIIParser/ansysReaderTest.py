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
        
        
        
if __name__ == '__main__':
    unittest.main()
    
    
    