#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import sys, string
import Debug
from xml.dom import minidom
from xml.dom import Node
import Enum
import os
import msfParser
            
class lhpXMLDictionariesBuilder:
    """Facilities to handle lhpBuilder XML dictionary"""
    
    def __init__(self):                              
        
        self.masterDictionaryFileName = "UNDEFINED"
        self.subDictionaryFileName = "UNDEFINED"
        self.outputDictionaryFileName = "dictionaryPlusSubdictionary.xml"
        
    def BuildMotionAnalysisDictionary(self):
        
        # load master dictionary
        msfDOMParserInstance = msfParser.msfParser()        
        masterDomDocument = minidom.parse(self.masterDictionaryFileName)
        masterRootNode = masterDomDocument.documentElement
        
        # get the master subdir node
        sourceDirNode = msfDOMParserInstance.GetNodeByNodeName(masterRootNode,"MASource")
        
        assert(masterRootNode != None)
        
        # load motionAnalysys dictionary
        motionAnalysisSubictionaryDomDocument = minidom.parse(self.subDictionaryFileName)
        motionAnalysisRootNode = motionAnalysisSubictionaryDomDocument.documentElement
        
        # get motionAnalysys parent node
        motionAnalysisTypeNode = msfDOMParserInstance.GetNodeByNodeName(motionAnalysisRootNode,"Motion")
        
        assert(motionAnalysisTypeNode != None)
       
        sourceDirNode.appendChild(motionAnalysisTypeNode)
        
        newDoc = minidom.Document()
        newDoc.appendChild(masterRootNode)
               
        outFileXML = open(self.outputDictionaryFileName, 'w')
        newDoc.writexml(outFileXML)
     
    def BuildDicomDictionary(self):
        # load master dictionary
        msfDOMParserInstance = msfParser.msfParser()        
        masterDomDocument = minidom.parse(self.masterDictionaryFileName)
        masterRootNode = masterDomDocument.documentElement
        
        # get the master subdir node
        sourceDirNode = msfDOMParserInstance.GetNodeByNodeName(masterRootNode,"DicomSource")
        
        assert(masterRootNode != None)
        
        # load motionAnalysys dictionary
        motionAnalysisSubictionaryDomDocument = minidom.parse(self.subDictionaryFileName)
        motionAnalysisRootNode = motionAnalysisSubictionaryDomDocument.documentElement
        
        # get motionAnalysys parent node
        motionAnalysisTypeNode = msfDOMParserInstance.GetNodeByNodeName(motionAnalysisRootNode,"Type")
        
        assert(motionAnalysisTypeNode != None)
       
        sourceDirNode.appendChild(motionAnalysisTypeNode)
        
        newDoc = minidom.Document()
        newDoc.appendChild(masterRootNode)
               
        outFileXML = open(self.outputDictionaryFileName, 'w')
        newDoc.writexml(outFileXML)
     
    def BuildFunctionalAnatomyDictionary(self):
        # load master dictionary
        msfDOMParserInstance = msfParser.msfParser()        
        masterDomDocument = minidom.parse(self.masterDictionaryFileName)
        masterRootNode = masterDomDocument.documentElement
        
        # get the master subdir node
        sourceDirNode = msfDOMParserInstance.GetNodeByNodeName(masterRootNode,"FunctionalAnatomy")
        
        assert(masterRootNode != None)
        
        # load motionAnalysys dictionary
        FASubictionaryDomDocument = minidom.parse(self.subDictionaryFileName)
        motionAnalysisRootNode = FASubictionaryDomDocument.documentElement
        
        # get motionAnalysys parent node
        FATypeNode = msfDOMParserInstance.GetNodeByNodeName(motionAnalysisRootNode,"Type")
        
        assert(FATypeNode != None)
       
        sourceDirNode.appendChild(FATypeNode)
        
        newDoc = minidom.Document()
        newDoc.appendChild(masterRootNode)
               
        outFileXML = open(self.outputDictionaryFileName, 'w')
        newDoc.writexml(outFileXML)
    
def run(masterXMLDictionaryFilename, subXMLDictionaryFilename, command, outputXMLDictionaryFilename):
    """"""
    
    lhpXMLDictionariesBuilderInstance = lhpXMLDictionariesBuilder()
    lhpXMLDictionariesBuilderInstance.masterDictionaryFileName = masterXMLDictionaryFilename
    lhpXMLDictionariesBuilderInstance.subDictionaryFileName = subXMLDictionaryFilename
    lhpXMLDictionariesBuilderInstance.outputDictionaryFileName = outputXMLDictionaryFilename
    
    if command == 'dicom':
        lhpXMLDictionariesBuilderInstance.BuildDicomDictionary()
         
    elif command == 'motion_analysis':
        lhpXMLDictionariesBuilderInstance.BuildMotionAnalysisDictionary()
    
    elif command == 'functional_anatomy':
        lhpXMLDictionariesBuilderInstance.BuildFunctionalAnatomyDictionary()

    else:
        print 'command not available!'
        sys.exit(1)

    if Debug: 
        print "Beware:  In order to work run this est must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
      
    return 

if __name__ == '__main__':
    import sys
    usage_msg = '''Usage: %s 
    masterXMLDictionaryFilename, subXMLDictionaryFilename 
    <option> outputXMLDictionaryFilename 
    where option can be:
    dicom - assemble dicom dictionary
    motion_analysis  - assemble motion analysis dictionary
    ''' % sys.argv[0]

    if len(sys.argv) != 5:
        print 'Error :\n' + usage_msg
        sys.exit(1)

    masterXMLDictionaryFilename = sys.argv[1]
    subXMLDictionaryFilename = sys.argv[2]
    command = sys.argv[3]
    outputXMLDictionaryFilename = sys.argv[4]
    
    run(masterXMLDictionaryFilename, subXMLDictionaryFilename, command, outputXMLDictionaryFilename)
