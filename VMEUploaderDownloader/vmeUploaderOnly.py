#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni, Roberto Mucci
#-----------------------------------------------------------------------------

import fileUtilities
import sys, string
import msfParser
import shutil
import sets
import os
import re
import csv
import datetime, time
from Debug import Debug
from xml.dom import minidom
from xml.dom import Node

class vmeUploaderOnly:
    """upload a vme xml file from an MSF given its ID """
    
    def __init__(self):                       
        
        self.InputMSFDirectory = "No input msf"
        self.VmeToExtractID = 1    
        self.OutputVMEXMLName = "exportedVME.xml"
        self.OutputFolderName = "FolderToUpload"
        self.DatasetURI = ""
        self.hasLink = "";
        
    def Upload(self):
        self.__Parse()
        
    def __Parse(self):
       
        msfDOMParserInstance = msfParser.msfParser()
       
        # Search the MSF file inside given the given directory
        oldDir = os.getcwd()
        os.chdir(self.InputMSFDirectory)
        files = os.listdir(self.InputMSFDirectory)
        
        if Debug:
            print "Directory: " + self.InputMSFDirectory + " contains: "
            print files
        msfFileNameList = []
        for file in files:
            if re.search('\\.msf$',file):
               msfFileNameList.append(file)
        assert(len(msfFileNameList)  == 1)
        
        msfFileName = msfFileNameList[0]
        domDocument = minidom.parse(msfFileName)
        msfRootNode = domDocument.documentElement
       
        print "\ninput MSF Directory: " + self.InputMSFDirectory
        print "\ninput MSF filename: " + msfFileName
        print "\nExtracting vme with ID: " + str(self.VmeToExtractID) + '\n' 
        """parse the msf extracting tags from the UnhandledPlusManualTagsList file """
        
        rootNode = msfRootNode
        vmeId = self.VmeToExtractID
        
        # get the vme node
        outVmeNode = msfDOMParserInstance.GetVmeNodeById(rootNode, vmeId)
        
        # get the tagArray node
        outVmeTagArrayNode = msfDOMParserInstance.GetVmeTagArrayNode(outVmeNode)
        
        #AUTO TAGS SET  WITH  WEBSERVICE
        nodeURI = msfDOMParserInstance.GetTagNodeByTagName(outVmeTagArrayNode, "L0000_resource_data_Dataset_DatasetURI")
        msfDOMParserInstance.SetTagNodeText(nodeURI, self.DatasetURI)       
        text = msfDOMParserInstance.GetTagNodeText(nodeURI)
        print text
        
        today = datetime.date.today()
        nodeURI = msfDOMParserInstance.GetTagNodeByTagName(outVmeTagArrayNode, "L0000_resource_data_Dataset_UploadDate")
        msfDOMParserInstance.SetTagNodeText(nodeURI, today)       
        day = msfDOMParserInstance.GetTagNodeText(nodeURI)
        print day
        print self.hasLink
        
        if (self.hasLink == "true"):   
            newDir = os.getcwd()
            os.chdir(newDir + r"..\..\..")
            #ciclo su list URI
            list = ''
            file = open("listURI","r")
            for line in file.readlines():
                line = line.replace("\n", ' ')
                list += line                
                
            nodeURI = msfDOMParserInstance.GetTagNodeByTagName(outVmeTagArrayNode, "L0000_resource_MAF_Procedural_VMElinkURI1")
            msfDOMParserInstance.SetTagNodeText(nodeURI, list)       
            URI = msfDOMParserInstance.GetTagNodeText(nodeURI)
            print URI 
            file.close()
            os.chdir(newDir)

        # for the moment cannot remove anything
        # msfDOMParserInstance.RemoveTagsByList(outVmeTagArrayNode, a)
        
        # create output directory
        fileUtilities._mkdir(self.OutputFolderName)
        
        # save created XML to this directory
        os.chdir(self.OutputFolderName)
        
        newDoc = minidom.Document()
        newDoc.appendChild(outVmeNode)
        outFileXML = open(self.OutputVMEXMLName, 'w')
        newDoc.writexml(outFileXML)
        
        # Copy of VME binary file to this directory
        # get the file to be copied
        fileNameList = msfDOMParserInstance.GetVMEDataURLList(outVmeNode)
        
        
        if(len(fileNameList)  == 1):
           assert(fileNameList  != "") #if "" URL of the file not found
        
           os.chdir(self.InputMSFDirectory)
           shutil.copy2(fileNameList[0],self.OutputFolderName)
        
        print "\nWritten output XML file " + self.OutputVMEXMLName + " in directory " + self.OutputFolderName
        #time.sleep(10)
        
def run(inputMSFDirectory, vmeToExtractId, outputFolderName, outputVMEXMLName):                                            
    upl = vmeUploader()
    upl.InputMSFDirectory = inputMSFDirectory
    upl.VmeToExtractID = int(vmeToExtractId)
    upl.OutputFolderName = outputFolderName
    upl.OutputVMEXMLName = outputVMEXMLName
    upl.Parse()    

def main():
    args = sys.argv[1:]
    if len(args) != 4:
        print """
        usage: python.exe vmeUploader.py 
        inputMSFDirectory vmeToExtractId 
        autoTagsFile.csv manualTagsFile.csv
        outputFolderName outputVMEXMLName.xml
        """
        sys.exit(-1)
    run(args[0],args[1],args[2],args[3])


if __name__ == '__main__':
    main()
