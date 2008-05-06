#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import os
import sys, string
from cStringIO import StringIO
import unittest

from Debug  import Debug
import ansysReader
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
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = self.StartDir + r'\testData\ansys\tetra10_ansys_commented_ASCII_Converted.inp'
    
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
    
    def testReaderV0(self):
        
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName =  self.StartDir + r'\testData\ansys\tet10_v0.inp'
    
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

    
    def testReaderV1(self):
        
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName =  self.StartDir + r'\testData\ansys\tet10_v1.inp'
    
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
        ar.InputAnsysFileName =  self.StartDir + r'\testData\ansys\tet10_v2.inp'
    
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
        ar.InputAnsysFileName = self.StartDir + r'\testData\ansys\tet10_v3.inp'
    
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
        ar.InputAnsysFileName = self.StartDir + r'\testData\ansys\tet10_v4.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber , materialsMPTEMP_MPSectionsNumber = cargo
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementDeclarationSectionsNumber, 2)

    def testReaderV5(self):
        
        curDir =  os.getcwd()
        if Debug:
            print curDir
            
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = self.StartDir + r'\testData\ansys\tet10_v5.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber , materialsMPTEMP_MPSectionsNumber = cargo
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementDeclarationSectionsNumber, 2)
    
    def testTetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar(self):
        
        curDir =  os.getcwd()
        if Debug:
            print curDir
            
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = self.StartDir + r'\testData\ansys\Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber , materialsMPTEMP_MPSectionsNumber = cargo
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementDeclarationSectionsNumber, 5)
        self.assertEqual(elementTypeSectionsNumber, 2)
        self.assertEqual(materialsSectionsNumber, 0)
        self.assertEqual(materialsMPTEMP_MPSectionsNumber, 4)
    
    def testTetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarRotatedNodes(self):
        
        curDir =  os.getcwd()
        if Debug:
            print curDir
            
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName = self.StartDir + r'\testData\ansys\Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevarRotatedNodes.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()
        
        nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber , materialsMPTEMP_MPSectionsNumber = cargo
        
        self.assertEqual(nodeSectionsNumber,1)
        self.assertEqual(elementDeclarationSectionsNumber, 5)
        self.assertEqual(elementTypeSectionsNumber, 2)
        self.assertEqual(materialsSectionsNumber, 0)
        self.assertEqual(materialsMPTEMP_MPSectionsNumber, 4)
    

    def estHugeAnsysFile(self):
        # approximately 2 minutes running for a 14 MB ansys file On a dual core Number 6600 with four gigabytes ram
        
        # create the reader
        ar = ansysReader.ansysReader()
        ar.InputAnsysFileName =  self.StartDir + r'\testData\ansys\2505_meshBV3_morgan.inp'
    
        # set the file name
        # read
        
        cargo = ar.Read()

if __name__ == '__main__':
    unittest.main()
    
    
    