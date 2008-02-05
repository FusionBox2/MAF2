#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import os
import sys, string, StringIO
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
        print self.StartDir
        
        
    def tearDown(self):
        os.chdir(self.StartDir)
        
        
    
    def testAnsysWriter(self):
        
        # create the ansys writer
        
        # set nodes file
        
        # set elements file
            
        # set materials file
        
        # set output file name
        
        exporter = ansysWriter.ansysWriter()
        # write
        exporter.NodesFileName = r"D:\vapps\openMAF_ITK_Parabuild\Testing\unittestData\FEM\ANSYS\tet10\NLISTMaterialsWritten.txt"
        
        exporter.ElementsFileName = r"D:\vapps\openMAF_ITK_Parabuild\Testing\unittestData\FEM\ANSYS\tet10\ELISTMaterialsWritten.txt"
        
        exporter.MaterialsFileName = r"D:\vapps\openMAF_ITK_Parabuild\Testing\unittestData\FEM\ANSYS\tet10\MPLISTMaterialsWritten.txt"
            
        exporter.AnsysOutputFileName = "AnsysOutput.inp"
         
        exporter.Write()
        
        
        
if __name__ == '__main__':
    unittest.main()
    
    
    