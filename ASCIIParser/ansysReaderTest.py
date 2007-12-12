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
    
    def estReader(self):
        
        
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = r'D:\vapps\LHPBuilder_Parabuild\ASCIIParser\testData\ansys\tetra10_ansys_commented_ASCII_Converted.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber , materialsMPTEMP_MPSectionsNumber = cargo
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementDeclarationSectionsNumber, 2)
        self.assertEqual(elementTypeSectionsNumber, 2)
        self.assertEqual(materialsSectionsNumber,2)
        self.assertEqual(materialsMPTEMP_MPSectionsNumber,0)
    
    def estReaderV0(self):
        
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = r'D:\vapps\LHPBuilder_Parabuild\ASCIIParser\testData\ansys\tet10_v0.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber , materialsMPTEMP_MPSectionsNumber = cargo
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementDeclarationSectionsNumber, 2)
        #self.assertEqual(elementTypeSectionsNumber, 2)
        #self.assertEqual(materialsSectionsNumber,2)
        #self.assertEqual(materialsMPTEMP_MPSectionsNumber,0)

    
    def estReaderV1(self):
        
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = r'D:\vapps\LHPBuilder_Parabuild\ASCIIParser\testData\ansys\tet10_v1.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber , materialsMPTEMP_MPSectionsNumber = cargo
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementDeclarationSectionsNumber, 2)
    
    
    def estReaderV2(self):
        
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = r'D:\vapps\LHPBuilder_Parabuild\ASCIIParser\testData\ansys\tet10_v2.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber , materialsMPTEMP_MPSectionsNumber = cargo
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementDeclarationSectionsNumber, 2)

    def testReaderV3(self):
        
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = r'D:\vapps\LHPBuilder_Parabuild\ASCIIParser\testData\ansys\tet10_v3.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber , materialsMPTEMP_MPSectionsNumber = cargo
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementDeclarationSectionsNumber, 2)
 
    
    def testReaderV4(self):
        
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = r'D:\vapps\LHPBuilder_Parabuild\ASCIIParser\testData\ansys\tet10_v4.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber , materialsMPTEMP_MPSectionsNumber = cargo
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementDeclarationSectionsNumber, 2)

    def testReaderV5(self):
        
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = r'D:\vapps\LHPBuilder_Parabuild\ASCIIParser\testData\ansys\tet10_v5.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber , materialsMPTEMP_MPSectionsNumber = cargo
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementDeclarationSectionsNumber, 2)
        
    def estHugeAnsysFile(self):
        # approximately 2 minutes running for a 14 MB ansys file On a dual core Number 6600 with four gigabytes ram
        
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = r'D:\vapps\LHPBuilder_Parabuild\ASCIIParser\testData\ansys\2505_meshBV3_morgan.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
       
    
if __name__ == '__main__':
    unittest.main()
    
    
    