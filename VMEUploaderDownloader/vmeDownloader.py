#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

import fileUtilities
import sys, string
import msfParser
import shutil
import sets
import os
import time
from Debug import Debug
from xml.dom import minidom
from xml.dom import Node

class vmeDownloader:
    """download an xml file representing a vme and build an MSF that can be imported in lhpbuilder 
    with the import msf operation"""
    
    def __init__(self):                       
        
        self.InputXMLFileName = "No input xml"
        
    def Parse(self):
	
        domP = msfParser.msfParser()
        domDocument = minidom.parse(self.InputXMLFileName)
        xmlRootNode = domDocument.documentElement
       
        if Debug:
            print "\ninput XML vme filename: " + self.InputXMLFileName
        
        vmeId = self.VmeToExtractID
        
        # get the vme node
        domP.GetVmeNodeById(xmlRootNode, vmeId)
        outVmeNode = domP.__OutputVme
        
        msfOutputDoc = minidom.Document()
        msfOutputDoc.appendChild(outVmeNode)
        outFileXML = open('importedVME.msf', 'w')
        msfOutputDoc.writexml(outFileXML)
        
        if Debug:
            print "\nWritten downloaded MSF file importedVME.msf in directory " + os.getcwd()
        
        
class MSFBuilder:
    """Build an msf from an XML representing a vme"""
    
    def __init__(self):                       
        self.InputVMEXMLFileName = "No input xml"
	self.InputVMEBinaryDataFileName = "No vme binary data"
        self.FakeRootMSFFileName = "No fake root specified"
        self.FakeMSFFileName = "No fake msf specified"
        self.OutputMSFFolderName = "No output folder specified"
	self.OutputMSFFileName = "No output file specified"
   
    def Build(self):
        
        domP = msfParser.msfParser()
	
        #  Import the fake root tree
        importedMSFDocument = minidom.parse(self.FakeRootMSFFileName)
        fakeRootNode = importedMSFDocument.documentElement
	
	if Debug:
	    
	    outFile = open('fakeRootNode.txt', 'w')
	    domP.PrintDOMTree(fakeRootNode,outFile)
       
        # Open the imported VME XML
        importedVMEDocument = minidom.parse(self.InputVMEXMLFileName)
        importedVmeNode = importedVMEDocument.documentElement
        isRoot = False
        isRoot = domP.IsARoot(importedVmeNode)
        if Debug:
            print "Is a Root? " + str(isRoot)
               
	
	if Debug:
	    
	    outFile = open('importedVMENode.txt', 'w')
	    domP.PrintDOMTree(importedVmeNode,outFile)
	
        # create the output msf containing the fake root with imported vme appended
        msfOutputDoc = minidom.Document()
        if(isRoot == False):
            msfOutputDoc.appendChild(fakeRootNode)
            
	outputDoc = msfOutputDoc.documentElement
	
	#f Debug:    
	#   outFile = open('newDocumentToStoreAsMSF.txt', 'w')
	#   domP.PrintDOMTree(outputDoc,outFile)
        
        if(isRoot == False):
            # create the msfParser
            childrenNode = domP.GetNodeByNodeName(outputDoc,"Children")
            # append the imported XML to the Children node
            childrenNode.appendChild(importedVmeNode)
        else:
            importedMSFDocument = minidom.parse(self.FakeMSFFileName)
            fakeMSFNode = importedMSFDocument.documentElement
            msfOutputDoc.appendChild(fakeMSFNode)
            outputDoc = msfOutputDoc.documentElement
            # create the msfParser
            childrenNode = domP.GetMSFNode(outputDoc) 
            childrenNode.appendChild(importedVmeNode)
	             
	        
        # create output directory
        fileUtilities._mkdir(self.OutputMSFFolderName)
        
        # save created XML to this directory
        os.chdir(self.OutputMSFFolderName)
        
        # write to XML
        outMSFFile = open(self.OutputMSFFileName, 'w')
        msfOutputDoc.writexml(outMSFFile)
            
        shutil.copy2(self.InputVMEBinaryDataFileName,self.OutputMSFFolderName)
        
        if Debug:
            print "\nWritten output MSF file " + self.OutputMSFFileName + " in directory " + self.OutputMSFFolderName
        
def run(inputVMEXMLFileName, inputVMEBinaryDataFileName, fakeRootMSFFileName, fakeMSFFileName, outputMSFFolderName,outputMSFFileName):                                            
    msfBuilder = MSFBuilder()
    
    msfBuilder.InputVMEBinaryDataFileName = inputVMEBinaryDataFileName
    msfBuilder.InputVMEXMLFileName = inputVMEXMLFileName
    msfBuilder.FakeRootMSFFileName = fakeRootMSFFileName
    msfBuilder.FakeMSFFileName = fakeMSFFileName
    msfBuilder.OutputMSFFolderName = outputMSFFolderName
    msfBuilder.OutputMSFFileName = outputMSFFileName
    msfBuilder.Build()
    
def main():
    args = sys.argv[1:]
    if len(args) != 5:
        print """
	usage: python.exe vmeDownloader.py 
	inputVMEXMLFile.xml inputVMEBinaryDataFileName inputFakeRoot.xml 
	outputMSFFolderName outputMSFFileName.msf"""
        sys.exit(-1)
    run(args[0],args[1],args[2],args[3],args[4])
    


if __name__ == '__main__':
    main()
