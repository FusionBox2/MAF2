#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import os
import sys, string

from cStringIO import StringIO
import unittest
import ansysWriter

from Debug  import Debug
import ansysReader
import shutil
from pyparsing import Word, alphas, nums, ZeroOrMore, ParseException, Group, delimitedList, alphanums, Literal,Dict, Suppress
import pprint

class ansysWriterTest(unittest.TestCase):
    
    def setUp(self):
        self.StartDir =  os.getcwd()
        if Debug:            
            print self.StartDir
        
        
    def tearDown(self):
        os.chdir(self.StartDir)
        
        
        
    def testAnsysWriterTetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar(self):
        
        
        exporter = ansysWriter.ansysWriter()
        # write
        
        exporter.NodesFileName = self.StartDir + r'\testData\ansys\Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar\NLIST.txt'
        
        exporter.ElementsFileName = self.StartDir + r'\testData\ansys\Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar\ELIST.txt'
        
        exporter.MaterialsFileName = self.StartDir + r'\testData\ansys\Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar\MPLIST.txt'
        
        exporter.AnsysOutputFileName = self.StartDir + r'\testData\ansys\Tetra10NodesIdJumpingMaterialsIdJumpingMaterialsGroupingNoTimevar\AnsysOutput.inp'
         
        exporter.Write()
        
    
    def estAnsysWriterTetra10TrivialMesh(self):
        
        # create the ansys writer
        
        # set nodes file
        
        # set elements file
            
        # set materials file
        
        # set output file name
        
        exporter = ansysWriter.ansysWriter()
        # write
        exporter.NodesFileName = self.StartDir + r'\testData\ansys\Tetra10TrivialMesh\NLIST.lis'
        
        exporter.ElementsFileName = self.StartDir + r'\testData\ansys\Tetra10TrivialMesh\ELIST.lis'
        
        exporter.MaterialsFileName = self.StartDir + r'\testData\ansys\Tetra10TrivialMesh\MPLIST.lis'
            
        exporter.AnsysOutputFileName = self.StartDir + r'\testData\ansys\Tetra10TrivialMesh\AnsysOutput.inp'
         
        exporter.Write()
        
if __name__ == '__main__':
    unittest.main()
    
    
    