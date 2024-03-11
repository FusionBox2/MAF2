#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni  <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

from Debug import Debug
import csv, sys
import unittest
import Enum
import lhpFAOntoToLHPCSVConverter
from xml.dom.minidom import Document
import os

class lhpFAOntoToLHPCSVConverterTest(unittest.TestCase):
    """Tests conversion from FA ontology csv  dictionary to LHP format csv dictionary"""
    
    def testConvertFAOntology(self):
         """main test drive for lhpFAOntoToLHPCSVConverter: 
         parse Onto csv dictionary and convert it to LHP format"""
         
         print "current path:"
         print os.getcwd()
         
         # Application Experts note
         # your CSV to validate here! ( replace sample data file name )
         inCSV = r'.\FAOnto2LHPCSVTestData\FA_onto.csv'
         assert(os.path.exists(inCSV))

         # Application Experts note
         # this is the output to open in Firefox to check it`s valid XML ( replace output sample data )   
         outCSV = r'.\FAOnto2LHPCSVTestData\OutputCSV.csv'

         lhpFAOntoToLHPCSVConverter.run(inCSV,outCSV)
                
if __name__ == '__main__':
    unittest.main()
