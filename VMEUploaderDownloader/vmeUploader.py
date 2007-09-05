import sys, string
import domParser
import shutil
import sets
import os
from xml.dom import minidom
from xml.dom import Node

class vmeUploader:
    """upload a vme xml file from an MSF given its ID and a dictionary"""
    
    def __init__(self):                       
        self.inputMSFFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\msf_test_import_export_VME\msf_test_import_export_VME.msf'
        self.copyInputMSFFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\msf_test_import_export_VME\copyied_msf_test_import_export_VME.msf'
        shutil.copy(self.inputMSFFileName, self.copyInputMSFFileName)

        self.DomParser = domParser.domParser()
        dict = self.LhpbDictionary = domParser.LHPBDictionary()
        self.DictionaryFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\testDictionaries\testDictionary.txt'
        
        self.LhpbDictionary.DictionaryFileName = self.DictionaryFileName
        # load the dictionary
        dict.Load()
        
        self.MSFDOMDocument = minidom.parse(self.copyInputMSFFileName)
        self.MSFRootNode = self.MSFDOMDocument.documentElement
        
        self.VmeToExtractID = 1
        
    def Parse(self):
        
        print "\ninput MSF: " + self.copyInputMSFFileName
        print "\nlhdl dictionary: " + self.DictionaryFileName + '\n'
        print "\nExtracting vme with ID1: " + str(self.VmeToExtractID) + '\n' 
        """parse the msf extracting tags from the dictionary"""
        dict = self.LhpbDictionary
        rootNode = self.MSFRootNode
        vmeId = self.VmeToExtractID
        domP = self.DomParser
        
        # get the vme node
        domP.GetVmeNodeById(rootNode, vmeId)
        outVmeNode = domP.OutputVme
        
        # get the tagArray node
        domP.GetVmeTagArrayNode(outVmeNode)
        outVmeTagArrayNode = domP.OutputTagArrayNode
        
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
        
        newDoc = minidom.Document()
        newDoc.appendChild(outVmeNode)
        outFileXML = open('exportedVME.xml', 'w')
        newDoc.writexml(outFileXML)
        
        print "\nWritten output XML file exportedVME.xml in directory " + os.getcwd()
        
        

def run(inMSFFileName, vmeToExtractId, inDictionaryFileName):                                            
    upl = vmeUploader()
    upl.inputMSFFileName = inMSFFileName
    upl.VmeToExtractID = int(vmeToExtractId)
    upl.DictionaryFileName = inDictionaryFileName
    upl.__init__()
    upl.Parse()
        
    

def main():
    args = sys.argv[1:]
    if len(args) != 3:
        print 'usage: python.exe vmeUploader.py infile.msf vmeToExtractId dictionaryFile.txt'
        sys.exit(-1)
    run(args[0],args[1],args[2])


if __name__ == '__main__':
    main()
