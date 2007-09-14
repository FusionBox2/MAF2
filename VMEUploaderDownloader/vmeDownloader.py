#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

import sys, string
import domParser
import shutil
import sets
import os
from xml.dom import minidom
from xml.dom import Node

class vmeDownloader:
    """download an xml file representing a vme and build an MSF that can be imported in lhpbuilder 
    with the import msf operation"""
    
    def __init__(self):                       
        
        self.InputXMLFileName = "No input xml"
        
    def Parse(self):
	
        domP = domParser.domParser()
        domDocument = minidom.parse(self.InputXMLFileName)
        xmlRootNode = domDocument.documentElement
       
        print "\ninput XML vme filename: " + self.InputXMLFileName
        
        vmeId = self.VmeToExtractID
        
        # get the vme node
        domP.GetVmeNodeById(xmlRootNode, vmeId)
        outVmeNode = domP.OutputVme
        
        msfOutputDoc = minidom.Document()
        msfOutputDoc.appendChild(outVmeNode)
        outFileXML = open('importedVME.msf', 'w')
        msfOutputDoc.writexml(outFileXML)
        
        print "\nWritten downloaded MSF file importedVME.msf in directory " + os.getcwd()
        
        
class MSFBuilder:
    """Build an msf from an XML representing a vme"""
    
    def __init__(self):                       
        self.InputVMEXMLFileName = "No input xml"
        self.FakeRootMSFFileName = "No fake root specified"
        self.OutputMSFFileName = "No output specified"
   
    def Build(self):
        
        domP = domParser.domParser()
	
        #  Import the fake root tree
        importedMSFDocument = minidom.parse(self.FakeRootMSFFileName)
        fakeRootNode = importedMSFDocument.documentElement
	
	outFile = open('fakeRootNode.txt', 'w')
	domP.PrintDOMTree(fakeRootNode,outFile,0)
       
        # Open the imported VME XML
        importedVMEDocument = minidom.parse(self.InputVMEXMLFileName)
        importedVmeNode = importedVMEDocument.documentElement
	
	outFile = open('importedVMENode.txt', 'w')
	domP.PrintDOMTree(importedVmeNode,outFile,0)
	
        # create the output msf containing the fake root with imported vme appended
        msfOutputDoc = minidom.Document()
        msfOutputDoc.appendChild(fakeRootNode)
	outputDoc = msfOutputDoc.documentElement
	
	outFile = open('newDocumentToStoreAsMSF.txt', 'w')
	domP.PrintDOMTree(outputDoc,outFile,0)
	
        # create the domparser
        domP.GetNodeByNodeName(outputDoc,"Children")
	childrenNode = domP.OutputNode
	     
        # append the imported XML to the Children node
        childrenNode.appendChild(importedVmeNode)
	        
        # write to XML
        outMSFFile = open(self.OutputMSFFileName, 'w')
        msfOutputDoc.writexml(outMSFFile)
    
def run(inputVMEXMLFileName, fakeRootMSFFileName,outputMSFFileName):                                            
    msfBuilder = MSFBuilder()
    msfBuilder.InputVMEXMLFileName = inputVMEXMLFileName
    msfBuilder.FakeRootMSFFileName = fakeRootMSFFileName
    msfBuilder.OutputMSFFileName = outputMSFFileName
    msfBuilder.Build()
    
def main():
    args = sys.argv[1:]
    if len(args) != 2:
        print 'usage: python.exe vmeDownloader.py inputVMEFile.xml inputFakeRoot.xml outputMSFName.msf'
        sys.exit(-1)
    run(args[0],args[1],args[2])
    


if __name__ == '__main__':
    main()
