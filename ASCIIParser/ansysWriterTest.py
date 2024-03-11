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
        
    
    def testAnsysWriterTetra10TrivialMesh(self):
        
        
        exporter = ansysWriter.ansysWriter()
        # write
        exporter.NodesFileName = self.StartDir + r'\testData\ansys\testExport\AnsysExporterNodes.lis'
        
        exporter.ElementsFileName = self.StartDir + r'\testData\ansys\testExport\AnsysExporterElements.lis'
        
        exporter.MaterialsFileName = self.StartDir + r'\testData\ansys\testExport\AnsysExporterMaterials.lis'
            
        exporter.AnsysOutputFileName = self.StartDir + r'\testData\ansys\testExport\AnsysOutput.inp'
         
        exporter.Write()
   
   
    def testAnsysWriterTetra10TrivialMeshWithSpacesInPath(self):
        
        
        exporter = ansysWriter.ansysWriter()
        # write
        exporter.NodesFileName = self.StartDir + r'\testData\ansys\test export with spaces\AnsysExporterNodes.lis'
        
        exporter.ElementsFileName = self.StartDir + r'\testData\ansys\test export with spaces\AnsysExporterElements.lis'
        
        exporter.MaterialsFileName = self.StartDir + r'\testData\ansys\test export with spaces\AnsysExporterMaterials.lis'
            
        exporter.AnsysOutputFileName = self.StartDir + r'\testData\ansys\test export with spaces\AnsysOutput.inp'
         
        exporter.Write()
        
        fileName = self.StartDir + r'\log.txt'
        print fileName
        f = open(fileName, 'w')
        f.write(str(exporter.NodesFileName))
        f.write('\n')
        f.write(str(exporter.ElementsFileName))
        f.write('\n')       
        f.write(str(exporter.MaterialsFileName))
        f.write('\n')
        f.write(str(exporter.AnsysOutputFileName))
        f.close()
        
   
if __name__ == '__main__':
    unittest.main()
    
    
    