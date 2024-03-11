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
import csv
import datetime
from Debug import Debug
from xml.dom import minidom
from xml.dom import Node

class vmeUploader:
    """upload a vme xml file from an MSF given its ID and a dictionary"""
    
    def __init__(self):                       
        
        self.InputMSFDirectory = "No input msf"
        self.UnhandledPlusManualTagsListFileName = "No UnhandledPlusManualTagsListFile!"
        self.HandledAutoTagsListFileName = "No HandledAutoTagsListFile!" 
        self.VmeToExtractID = 1    
        self.OutputVMEXMLName = "exportedVME.xml"
        self.OutputFolderName = "FolderToUpload"

        self.DatasetURI = ""
        
    def Upload(self):
        self.__Parse()
        
    def __Parse(self):
       
        msfDOMParserInstance = msfParser.msfParser()
       
        # Search the MSF file inside given the given directory
        os.chdir(self.InputMSFDirectory)
        files = os.listdir(self.InputMSFDirectory)
        
        if Debug:
            print "Directory: " + self.InputMSFDirectory + " contains: "
            print files
        msfFileNameList = []
        for file in files:
            if re.search('\\.msf$',file):
               msfFileNameList.append(file)
            
        if Debug:
            print msfFileNameList
        assert(len(msfFileNameList)  == 1)
        
        msfFileName = msfFileNameList[0]
        domDocument = minidom.parse(msfFileName)
        msfRootNode = domDocument.documentElement
       
        print "\ninput MSF Directory: " + self.InputMSFDirectory
        print "\ninput MSF filename: " + msfFileName
        print "\nlhdl dictionary: " + self.UnhandledPlusManualTagsListFileName + '\n'
        print "\nExtracting vme with ID: " + str(self.VmeToExtractID) + '\n' 
        """parse the msf extracting tags from the UnhandledPlusManualTagsList file """
        
        rootNode = msfRootNode
        vmeId = self.VmeToExtractID
        
        # get the vme node
        outVmeNode = msfDOMParserInstance.GetVmeNodeById(rootNode, vmeId)
        
        # get the tagArray node
        outVmeTagArrayNode = msfDOMParserInstance.GetVmeTagArrayNode(outVmeNode)
        
        # get the tags list from vme tag array node
        vmeTagList = msfDOMParserInstance.GetTagNames(outVmeTagArrayNode)
        
        if Debug:            
            print vmeTagList
        
        # AUTO TAGS
        
        if Debug:
            print self.HandledAutoTagsListFileName
        autoTagsReader = csv.reader(open(self.HandledAutoTagsListFileName, "r"))
        
        autoTagsDictionary = {}
        
        try:
            for row in autoTagsReader:
                if Debug:                   
                    print "row: " +  str(row)
                autoTagsDictionary[row[0]] = str(row[1])
                
        except csv.Error, e:
            sys.exit('file %s, line %d: %s' % (filename, autoTagsReader.line_num, e))
       
        if Debug:          
            print autoTagsDictionary
        
        autoTagsList = sorted(autoTagsDictionary.keys())
        print autoTagsList
        
        # remove old auto tags from tagArray
        msfDOMParserInstance.RemoveTagsByList(outVmeTagArrayNode, autoTagsList)
    
        # add new and updated autotags
        # "TAGNAME" , "TAGTEXT"
        msfDOMParserInstance.AddTagsFromDictionary(domDocument,outVmeTagArrayNode, autoTagsDictionary)
    
        # UNHANDLED AUTO + MANUAL TAGS
        
        # import unhandled auto + manual csv
        
        # create a list of keys
        
        unhandledPlusManualTagsReader = csv.reader(open(self.UnhandledPlusManualTagsListFileName, "r"))       
        
        unhandledPlusManualTagsDictionary  = {}
        
        try:
            for row in unhandledPlusManualTagsReader:
                if Debug:                   
                    print row
                unhandledPlusManualTagsDictionary[row[0]] = str(row[1])
                
        except csv.Error, e:
            sys.exit('file %s, line %d: %s' % (filename, unhandledPlusManualTagsReader.line_num, e))
       
        if Debug:          
            print unhandledPlusManualTagsDictionary  
        
        unhandledPlusManualTagsList = sorted(unhandledPlusManualTagsDictionary.keys())
        print unhandledPlusManualTagsList 
     
        # find matching manual and unhandled manual tags between dictionary and vme tagArray
        vmeTagListSet = sets.Set(vmeTagList)
        
        if Debug:
            print vmeTagListSet
        
        unhandledPlusManualTagsListSet = sets.Set(unhandledPlusManualTagsList) 
        # print unhandledPlusManualTagsListSet
        
        tagsToBeExported = vmeTagListSet.intersection(unhandledPlusManualTagsListSet)
        # print "\nThese tags will be exported: \n" + str(tagsToBeExported)
        
        tagsToBeAnnotatedManuallySet = unhandledPlusManualTagsListSet.difference(vmeTagList)
       
        # Add tags to be annotated manually from list
        isinstance(tagsToBeAnnotatedManuallySet,set)
        tagsToBeAnnotatedManuallySorted = sorted(tagsToBeAnnotatedManuallySet)
        
        tagsToBeAnnotatedManuallyMap = {}
        # for each item in set
        for key in tagsToBeAnnotatedManuallySorted:
            
            # create a map item
            tagsToBeAnnotatedManuallyMap[key] = unhandledPlusManualTagsDictionary[key] 
        
        if Debug:
            print "tagsToBeAnnotatedManuallyMap: "
            print tagsToBeAnnotatedManuallyMap
        
        print "\nThese tags need to be annotated by the user: \n" + str(tagsToBeAnnotatedManuallySorted)
        msfDOMParserInstance.AddTagsFromDictionary(domDocument,outVmeTagArrayNode, tagsToBeAnnotatedManuallyMap)

        tagsToBeRemoved  = vmeTagListSet.difference(tagsToBeExported)
        # print "\nThese tags  be removed from output vme XML: \n" + str(tagsToBeRemoved)
    
        a = list(tagsToBeRemoved)
        print a

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
        assert(len(fileNameList)  == 1)
        
        if(len(fileNameList)  == 1):
           #assert(len(fileNameList)  == 1)
        
           os.chdir(self.InputMSFDirectory)
           shutil.copy2(fileNameList[0],self.OutputFolderName)
        
        print "\nWritten output XML file " + self.OutputVMEXMLName + " in directory " + self.OutputFolderName
        
        
def run(inputMSFDirectory, vmeToExtractId,  handledAutoTagsListFileName, unhandledPlusManualTagsListFileName, outputFolderName, outputVMEXMLName):                                            
    upl = vmeUploader()
    upl.InputMSFDirectory = inputMSFDirectory
    upl.VmeToExtractID = int(vmeToExtractId)
    upl.HandledAutoTagsListFileName = handledAutoTagsListFileName
    upl.UnhandledPlusManualTagsListFileName = unhandledPlusManualTagsListFileName
    upl.OutputFolderName = outputFolderName
    upl.OutputVMEXMLName = outputVMEXMLName
    upl.Parse()    

def main():
    args = sys.argv[1:]
    if len(args) != 6:
        print """
        usage: python.exe vmeUploader.py 
        inputMSFDirectory vmeToExtractId 
        autoTagsFile.csv manualTagsFile.csv
        outputFolderName outputVMEXMLName.xml
        """
        sys.exit(-1)
    run(args[0],args[1],args[2],args[3],args[4],args[5])


if __name__ == '__main__':
    main()
