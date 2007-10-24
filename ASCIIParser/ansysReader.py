#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

# creating template for general rader
# this one is using regular expressions, grammars and exception handling to perform parsing
# code is very clean but I guess it could be improved by using a finite state machine
# experimenting...

import os
import sys, string
import unittest
import shutil
import re
from pyparsing import Word, alphas, nums, ZeroOrMore, ParseException, Group, delimitedList, alphanums, Literal,Dict
import pprint
import fileUtilities
            
class ansysReader:
    """"""
    
    def __init__(self):                       
        
        self.InputAnsysFileName = "No ansys input file defined"
        self.CacheFolderName = "AnsysReaderCache"
        
    def Read(self):
        return self.Parse()
        
    def Parse(self):
        
        # create the cache folder directory
        fileUtilities._mkdir(self.CacheFolderName)
          
        file = open(self.InputAnsysFileName)
        
        lineNumber = 0
        
        nodeSectionsNumber = 0
        materialsSectionsNumber = 0
        elementTypeSectionsNumber = 0
        elementDeclarationSectionsNumber = 0
        
        # parse the file structure and gather informations
        while 1:
           
            line = file.readline() 
            print str(lineNumber) + " > " + line
                       
            # NODES section
            # Find the NODES section
            if re.search("^(N,)", line):
                print "Found NODES section at line " + str(lineNumber)                
                nodeSectionsNumber += 1
                
                # create the NODES matrix
                cargo = file, line
                self.ReadNodes(cargo)
                
            # Elements type section
            # find the elements type section
            if re.search("^(ET,)", line):
                print "Found elements type section at line " + str(lineNumber)
                elementTypeSectionsNumber += 1
                # self.ReadElementsType()
                
            # Elements declaration section
            # find the elements declaration section
            if re.search("^(TYPE,)", line):
                print "Found elements declaration at line " + str(lineNumber)
                elementDeclarationSectionsNumber += 1
                # self.ReadElementsDeclaration()
            
            
            # Materials section
            # find the materials section
            if re.search("^(MPTEMP,)", line):
                print "Found materials section at line " + str(lineNumber)
                materialsSectionsNumber += 1
                
                # create the materials matrix
#                 self.ReadMaterials()
                
            if not line: break
            # output files into the cache directory
        
        
        print "nodeSectionsNumber " + str(nodeSectionsNumber)
        print "materialsSectionsNumber " + str(materialsSectionsNumber)
        print "elementTypeSectionsNumber " + str(elementTypeSectionsNumber)
        print "elementDeclarationSectionsNumber " + str(elementDeclarationSectionsNumber)
        
        return nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber
        
    def ReadNodes(self, cargo):
        """Generate Nodes matrix"""
        
        file, line = cargo
            
        while 1:
            
            print " > " + line
            # start grammar
            nodeHeader = "N,"
            nodeId = Word( alphanums ) + ","
            nodeCoords =  Group(delimitedList(Word( alphanums + ".")))
            nodeTextLine = nodeHeader + nodeId + nodeCoords
            # end grammar
            try: 
                print nodeTextLine.parseString( line )
            except ParseException:
                print "cannot parse line " + str(line)
                return
            
            line = file.readline()

    
    def ReadElementsDeclaration(self,file):
        """Generates element declaration stuff"""
        pass

    def ReadElementsType(self,file):
        """Generates elements type stuff"""
        pass

    def ReadMaterials(self,file):
        """Generate materials matrix"""
        pass
        
def Run(inputMSFDirectory, vmeToExtractId, inDictionaryFileName, outputFolderName, outputVMEXMLName):                                            
    upl = vmeUploader()
    upl.InputMSFDirectory = inputMSFDirectory
    upl.VmeToExtractID = int(vmeToExtractId)
    upl.DictionaryFileName = inDictionaryFileName
    upl.OutputFolderName = outputFolderName
    upl.OutputVMEXMLName = outputVMEXMLName
    upl.Parse()    

def main():
    args = sys.argv[1:]
    if len(args) != 5:
        print """
        usage: python.exe vmeUploader.py 
        inputMSFDirectory vmeToExtractId inputDictionaryFile.txt
        outputFolderName outputVMEXMLName.xml
        """
        sys.exit(-1)
    Run(args[0],args[1],args[2],args[3],[4])

if __name__ == '__main__':
    main()
    