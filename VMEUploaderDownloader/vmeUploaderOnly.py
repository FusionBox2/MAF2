#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni, Roberto Mucci
#-----------------------------------------------------------------------------

import fileUtilities
import sys, string
import msfParser
import UploadHandler
import shutil
import sets
import os
import re
import csv
import datetime, time
import hashlib
from Debug import Debug
from xml.dom import minidom
from xml.dom import Node

class vmeUploaderOnly:
    """upload a vme xml file from an MSF given its ID """
    
    def __init__(self):                       
        
        self.InputMSFDirectory = "No input msf"
        self.VmeToExtractID = 1    
        self.originalId = 1
        self.OutputVMEXMLName = "exportedVME.xml"
        self.OutputFolderName = "FolderToUpload"
        self.DatasetURI = "NOT PRESENT"
        self.vmeName = ""
        self.hasLink = ""
        self.withChild = ""
        self.hasBinary = False
        self.IsUploadMSF = False
        self.binaryName = ''
        self.localChksum = ""
        
    def Upload(self):
        self.__Parse()
        return self.localChksum
        
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
       
        if Debug:
            print "\ninput MSF Directory: " + self.InputMSFDirectory
            print "\ninput MSF filename: " + msfFileName
            print "\nExtracting vme with ID: " + str(self.VmeToExtractID) + '\n' 
            """parse the msf extracting tags from the UnhandledPlusManualTagsList file """
        
        rootNode = msfRootNode
        vmeId = self.VmeToExtractID
        
        if (str(vmeId) == "-1"):
            # get the root node
            outVmeNode = msfDOMParserInstance.GetRootNode(rootNode)
        else:
            # get the vme node
            outVmeNode = msfDOMParserInstance.GetVmeNodeById(rootNode, vmeId)
        
        
        # get the tagArray node
        outVmeTagArrayNode = msfDOMParserInstance.GetVmeTagArrayNode(outVmeNode)
        
        #AUTO TAGS SET  WITH  WEBSERVICE
        nodeURI = msfDOMParserInstance.GetTagNodeByTagName(outVmeTagArrayNode, "L0000_resource_data_Dataset_DatasetURI")
        msfDOMParserInstance.SetTagNodeText(nodeURI, self.DatasetURI)       
        text = msfDOMParserInstance.GetTagNodeText(nodeURI)
        if Debug:
            print text
        
        today = datetime.date.today()
        nodeURI = msfDOMParserInstance.GetTagNodeByTagName(outVmeTagArrayNode, "L0000_resource_data_Dataset_UploadDate")
        msfDOMParserInstance.SetTagNodeText(nodeURI, today)       
        day = msfDOMParserInstance.GetTagNodeText(nodeURI)
        if Debug:
            print day
        
       # print "has child? " + self.withChild
        if (self.withChild == "true"):   
            newDir = os.getcwd()
            os.chdir(sys.path[0])
            #cicle on file with list of child URI
            list = ''
            fileName = str(self.vmeName)+ str(self.originalId) + ".childURI"
            if Debug:
                print "file name: " + fileName 
            if(os.path.exists(fileName)):
                file = open(fileName ,"r")
                if Debug:
                    print "file opened: " + fileName
                for line in file.readlines():
                    line = line.replace("\n", ' ')
                    list += line                
                    
                nodeURI = msfDOMParserInstance.GetTagNodeByTagName(outVmeTagArrayNode, "L0000_resource_MAF_TreeInfo_VmeChildURI1")
                msfDOMParserInstance.SetTagNodeText(nodeURI, list)       
                childURI = msfDOMParserInstance.GetTagNodeText(nodeURI)
                if Debug:
                    print childURI 
                file.close()
                os.remove(fileName)
                os.chdir(newDir)
            else:
                print fileName + ' not Found'
                errorFile = open(sys.path[0] + '\\ErrorFound.lhp', 'a')
                errorFile.write(fileName + ' not Found\n')
                errorFile.close() 
                return 
                
                    
        
       # print "has Link = " + self.hasLink
        if (self.hasLink == "true" and self.IsUploadMSF == False):   
            newDir = os.getcwd()
            os.chdir(sys.path[0])
            #cicle on file with list of link URI
            list = ''
            fileName = self.vmeName + str(self.originalId) + ".linkURI"
            if(os.path.exists(fileName)):
                file = open(fileName,"r")

                for line in file.readlines():
                    line = line.replace("\n", ' ')
                    list += line                
                    
                nodeURI = msfDOMParserInstance.GetTagNodeByTagName(outVmeTagArrayNode, "L0000_resource_MAF_Procedural_VMElinkURI1")
                msfDOMParserInstance.SetTagNodeText(nodeURI, list)       
                linkURI = msfDOMParserInstance.GetTagNodeText(nodeURI)
                if Debug:
                    print linkURI 
                file.close()
                os.remove(fileName)
                os.chdir(newDir)
            else:
                print fileName + ' not Found'
                errorFile = open(sys.path[0] + '\\ErrorFound.lhp', 'a')
                errorFile.write(fileName + ' not Found\n')
                errorFile.close() 
                return 
            
        

        # for the moment cannot remove anything
        # msfDOMParserInstance.RemoveTagsByList(outVmeTagArrayNode, a)
        
        # create output directory
        fileUtilities._mkdir(self.OutputFolderName)
        
        # save created XML to this directory
        os.chdir(self.OutputFolderName)    
        oldDir = os.getcwd()
        os.chdir(self.InputMSFDirectory)
        
        #if(len(fileNameList) == 1 and len(fileNameList[0]) != 0):
        if(self.hasBinary == True):
            if Debug:
                print self.InputMSFDirectory
            os.chdir(self.InputMSFDirectory)
            shutil.copyfile(self.binaryName,self.OutputFolderName + "\\" + self.binaryName)
            
            #-----MD5 checksum calculation-----#
            os.chdir(self.OutputFolderName)
            if Debug:
                print self.OutputFolderName
            self.localChksum = self.md5(self.binaryName)
            if Debug:
                print "Local checksumII= " + self.localChksum
            
            #AUTO TAGS SET  WITH  WEBSERVICE      
            nodeURI = msfDOMParserInstance.GetTagNodeByTagName(outVmeTagArrayNode, "L0000_resource_data_Dataset_LocalFileCheckSum")
            msfDOMParserInstance.SetTagNodeText(nodeURI, self.localChksum)       
            chksum = msfDOMParserInstance.GetTagNodeText(nodeURI)
            if Debug:
                print chksum

        
        os.chdir(oldDir)
        newDoc = minidom.Document()
        newDoc.appendChild(outVmeNode)
        outFileXML = open(self.OutputVMEXMLName, 'w')
        newDoc.writexml(outFileXML)

        if Debug:
            print "\nWritten output XML file " + self.OutputVMEXMLName + " in directory " + self.OutputFolderName

    
    def md5(self,fileName):
        #Compute md5 hash of the specified file
        m = hashlib.md5()
        try:
            fd = open(fileName,"rb")
        except IOError:
            print "Unable to open the file in read mode: " + filename
            return
        content = fd.readlines()
        fd.close()
        for eachLine in content:
            m.update(eachLine)
        return m.hexdigest()

        
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
