#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Roberto Mucci   <r.mucci@cineca.it>
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
import time
from Debug import Debug
from xml.dom import minidom
from xml.dom import Node

import urllib, urllib2, base64, re, os, cookielib, sys
from HttpsProxy import *

class lhpEditVMETag:
    """Edit VME tags and save them into original msf"""
    
    
    def __init__(self):
        curDir = sys.path[0]
        self.InputMSFDirectory = "No input msf dir"
        self.msfFileName = "MSF Name"
        self.UnhandledPlusManualTagsListFileName = "No UnhandledPlusManualTagsListFile!"
        self.HandledAutoTagsListFileName = "HandledAutoTagsListFile!" 
        self.VmeToExtractID = 1
        self.OutputFolderName = "No output msf dir"
        self.OutputVMEXMLName = "OutputXML.xml"  #xml with tag edited
        self.OutputMSFFileName = "OutputMSF.lhp"
        self.FakeRootMSFFileName = "FakeRoot"
        self.FakeMSFFileName = "FakeMSF"
        
        
        self.DatasetURI = ""

        
    def ParseInput(self):
        #Create an xml from msf to edit

        
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
        
        
        #assert(len(msfFileNameList)  == 1)
        
        self.msfFileName = msfFileNameList[0]
        print self.msfFileName
        

        domDocument = minidom.parse(self.msfFileName)
        msfRootNode = domDocument.documentElement


 
        print "\ninput MSF Directory: " + self.InputMSFDirectory
        print "\ninput MSF filename: " + msfFileNameList[0]
        print "\nlhdl dictionary: " + self.UnhandledPlusManualTagsListFileName + '\n'
        print "\nExtracting vme with ID: " + str(self.VmeToExtractID) + '\n' 
        """parse the msf extracting tags from the UnhandledPlusManualTagsList file """
        
        rootNode = msfRootNode
        vmeId = self.VmeToExtractID
        
        if (vmeId == -1):
            # get the root node
            outVmeNode = msfDOMParserInstance.GetRootNode(rootNode)
        else:
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
        unhandledPlusManualTagsList = unhandledPlusManualTagsDictionary.keys()
        unhandledPlusManualTagsList.sort()

        #unhandledPlusManualTagsList = sorted(unhandledPlusManualTagsDictionary.keys())
        #print unhandledPlusManualTagsList 
    
        # find matching manual and unhandled manual tags between dictionary and vme tagArray
        vmeTagListSet = set(vmeTagList)

        if Debug:
            print vmeTagListSet
        
        unhandledPlusManualTagsListSet = set(unhandledPlusManualTagsList) 
        # print unhandledPlusManualTagsListSet
        
        tagsToBeExported = vmeTagListSet.intersection(unhandledPlusManualTagsListSet)
        # print "\nThese tags will be exported: \n" + str(tagsToBeExported)
        
        tagsToBeAnnotatedManuallySet = unhandledPlusManualTagsListSet.difference(vmeTagList)
       
        # Add tags to be annotated manually from list
        isinstance(tagsToBeAnnotatedManuallySet,set)

        #tagsToBeAnnotatedManuallySorted = sorted(tagsToBeAnnotatedManuallySet)
        tagsToBeAnnotatedManuallySorted = list(tagsToBeAnnotatedManuallySet)
        tagsToBeAnnotatedManuallySorted.sort()
        
        tagsToBeAnnotatedManuallyMap = {}
       
        # for each item in set
        for key in tagsToBeAnnotatedManuallySorted:
            
            # create a map item
            tagsToBeAnnotatedManuallyMap[key] = unhandledPlusManualTagsDictionary[key] 
        
        if Debug:
            print "tagsToBeAnnotatedManuallyMap: "
            print tagsToBeAnnotatedManuallyMap
        

        msfDOMParserInstance.AddTagsFromDictionary(domDocument,outVmeTagArrayNode, tagsToBeAnnotatedManuallyMap)
        

        tagsToBeRemoved  = vmeTagListSet.difference(tagsToBeExported)
        # print "\nThese tags  be removed from output vme XML: \n" + str(tagsToBeRemoved)
    
        a = list(tagsToBeRemoved)
        print a
        
        # create output directory
        fileUtilities._mkdir(self.OutputFolderName) 
        
        # save created XML to this directory
        os.chdir(self.OutputFolderName)

        newDoc = minidom.Document()
        newDoc.appendChild(outVmeNode)
        outFileXML = open(self.OutputVMEXMLName, 'w')
        newDoc.writexml(outFileXML)


        print "\nWritten XML file " + self.OutputVMEXMLName + " with tag edited in directory " + self.OutputFolderName
        
    def ParseOutput(self):
        #Create an msf from xml created in ParseInput
        self.InputVMEXMLFileName = self.OutputVMEXMLName
        
        domP = msfParser.msfParser()
    
        #  Import the fake root tree
        importedMSFDocument = minidom.parse(self.FakeRootMSFFileName)
        fakeRootNode = importedMSFDocument.documentElement
        
        #  Import the fake MSF tree
        importedMSFDocument = minidom.parse(self.FakeMSFFileName)
        fakeMSFNode = importedMSFDocument.documentElement
    
     #outFile = open('fakeRootNode.txt', 'w')
     #domP.PrintDOMTree(fakeRootNode,outFile)
       
        # Open the imported VME XML
        importedVMEDocument = minidom.parse(self.InputVMEXMLFileName)
        importedVmeNode = importedVMEDocument.documentElement
    
    # outFile = open('importedVMENode.txt', 'w')
    # domP.PrintDOMTree(importedVmeNode,outFile)
    
        # create the output msf containing the fake root with imported vme appended
        msfOutputDoc = minidom.Document()
        if (self.VmeToExtractID != -1):
            msfOutputDoc.appendChild(fakeRootNode)
            outputDoc = msfOutputDoc.documentElement
            childrenNode = domP.GetNodeByNodeName(outputDoc,"Children") 
            # append the imported XML to the Children node
            childrenNode.appendChild(importedVmeNode)
        else:
            msfOutputDoc.appendChild(fakeMSFNode)
            outputDoc = msfOutputDoc.documentElement
            # create the msfParser
            childrenNode = domP.GetMSFNode(outputDoc) 
            # append the imported XML to the Children node
            childrenNode.appendChild(importedVmeNode)

            
        # create output directory
        fileUtilities._mkdir(self.OutputFolderName)
        
        # save created XML to this directory
        os.chdir(self.OutputFolderName)
        
        # write to XML
        outMSFFile = open(self.OutputMSFFileName, 'w')
        msfOutputDoc.writexml(outMSFFile)
            
        #remove XML file
        xmlFilePath = self.OutputFolderName + "\\" + self.InputVMEXMLFileName
        os.remove(xmlFilePath)
        
        print "\nWritten output MSF file " + self.OutputMSFFileName + " in directory " + self.OutputFolderName

            
def run(inputMSFDirectory, vmeToExtractId, unhandledPlusManualTagsListFileName):                                        
     curDir = sys.path[0]
     lhpEditVMETagInstance = lhpEditVMETag()
     lhpEditVMETagInstance.InputMSFDirectory = inputMSFDirectory
     lhpEditVMETagInstance.HandledAutoTagsListFileName = curDir + r'\handledAutoTagsList.csv'
     lhpEditVMETagInstance.UnhandledPlusManualTagsListFileName = curDir + '\\' + unhandledPlusManualTagsListFileName
     lhpEditVMETagInstance.VmeToExtractID = int(vmeToExtractId)
     lhpEditVMETagInstance.OutputVMEXMLName = "OutputXML.xml"
     #lhpEditVMETagInstance.OutputVMEXMLName = curDir + r'\Outgoing'
     lhpEditVMETagInstance.OutputFolderName = inputMSFDirectory #put out xml in the msf directoty
     lhpEditVMETagInstance.FakeRootMSFFileName = curDir + r'\applicationData\fakeRoot.xml'
     lhpEditVMETagInstance.FakeMSFFileName = curDir + r'\applicationData\fakeMSF.xml'

     lhpEditVMETagInstance.ParseInput()
     lhpEditVMETagInstance.ParseOutput() 
    
if __name__ == '__main__':    
    import sys
    usage_msg = '''Usage: %s
    inputMSFDirectory, vmeToExtractId,
     unhandledPlusManualTagsListFileName
    ''' % sys.argv[0]
    
    if len(sys.argv) != 4:
        print 'Error :\n' + usage_msg
        sys.exit(1)
        
    inputMSFDirectory = sys.argv[1].replace("?", " ")
    vmeToExtractId = sys.argv[2]
    unhandledPlusManualTagsListFileName = sys.argv[3]
    
    print "\ninput MSF Directory: " + inputMSFDirectory
    print "\ninput vme Id: " + vmeToExtractId
    print "\nlhdl dictionary: " + unhandledPlusManualTagsListFileName
    
    run(inputMSFDirectory, vmeToExtractId, unhandledPlusManualTagsListFileName)

 
