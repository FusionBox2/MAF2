#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import fileUtilities
import sys, string
import msfParser
import shutil
import sets
import os
import re
from xml.dom import minidom
from xml.dom import Node

class vmeUploader:
    """upload a vme xml file from an MSF given its ID and a dictionary"""
    
    def __init__(self):                       
        
        self.InputMSFDirectory = "No input msf"
        self.DictionaryFileName = "No dictionary"
        self.VmeToExtractID = 1    
        self.OutputVMEXMLName = "exportedVME.xml"
        self.OutputFolderName = "FolderToUpload"
        
    def Upload(self):
        self.__Parse()
        
    def __Parse(self):
       
        domP = msfParser.msfParser()
        dict = msfParser.lhpbDictionary()
        dict.DictionaryFileName = self.DictionaryFileName
        dict.Load()
       
        # Search the MSF file inside given the given directory
        os.chdir(self.InputMSFDirectory)
        files = os.listdir(self.InputMSFDirectory)

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
        print "\nlhdl dictionary: " + self.DictionaryFileName + '\n'
        print "\nExtracting vme with ID: " + str(self.VmeToExtractID) + '\n' 
        """parse the msf extracting tags from the dictionary"""
        
        rootNode = msfRootNode
        vmeId = self.VmeToExtractID
        
        # get the vme node
        outVmeNode = domP.GetVmeNodeById(rootNode, vmeId)
        
        # get the tagArray node
        outVmeTagArrayNode = domP.GetVmeTagArrayNode(outVmeNode)
        
        # get the tags list
        vmeTagList = domP.PrintTagNames(outVmeTagArrayNode)
        
        print vmeTagList
        dict.Print()
        
        # find matching tags between dictionary and vme tagArray
        vmeTagListSet = sets.Set(vmeTagList)
        # print vmeTagListSet
        
        dictionaryTagListSet = sets.Set(dict.DictionaryTagsList) 
        # print dictionaryTagListSet
        
        tagsToBeExported = vmeTagListSet.intersection(dictionaryTagListSet)
        print "\nThese tags will be exported: \n" + str(tagsToBeExported)
        
        tagsToBeAddedByTheUser = dictionaryTagListSet.difference(vmeTagList)
        print "\nThese tags need to be added by the user: \n" + str(tagsToBeAddedByTheUser)
        
        tagsToBeRemoved  = vmeTagListSet.difference(tagsToBeExported)
        print "\nThese tags will be removed from output vme XML: \n" + str(tagsToBeRemoved)
    
        a = list(tagsToBeRemoved)
        print a
        
        domP.RemoveTagsByList(outVmeTagArrayNode, a)
        
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
        fileNameList = domP.GetVMEDataURLList(outVmeNode)
        assert(len(fileNameList)  == 1)
        
        if(len(fileNameList)  == 1):
           #assert(len(fileNameList)  == 1)
        
           os.chdir(self.InputMSFDirectory)
           shutil.copy2(fileNameList[0],self.OutputFolderName)
        
        print "\nWritten output XML file " + self.OutputVMEXMLName + " in directory " + self.OutputFolderName
        
        
def run(inputMSFDirectory, vmeToExtractId, inDictionaryFileName, outputFolderName, outputVMEXMLName):                                            
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
    run(args[0],args[1],args[2],args[3],[4])


if __name__ == '__main__':
    main()
