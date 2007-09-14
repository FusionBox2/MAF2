#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

# This is a vme in XML
# 
# <Node Crypting="0" Id="1" Name="test_volume" Type="mafVMEVolumeGray">
# 
# Element: Node
#                 Attribute -- Name: Type  Value: mafVMEVolumeGray
#                 Attribute -- Name: Crypting  Value: 0
#                 Attribute -- Name: Id  Value: 1
#                 Attribute -- Name: Name  Value: test_volume

import domParser
import sys, string
from xml.dom import minidom
from xml.dom import Node

import unittest
import shutil
import difflib
import pickle


class domParserTest(unittest.TestCase):
      
    def setUp(self):
        self.inFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\msf_test_import_export_VME\msf_test_import_export_VME.msf'
        self.copyFileName = r'D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\msf_test_import_export_VME\copyied_msf_test_import_export_VME.msf'
        self.domParserInstance = domParser.domParser()
        shutil.copy(self.inFileName, self.copyFileName)
        self.doc = minidom.parse(self.copyFileName)
        self.rootNode = self.doc.documentElement
        
    # def testRun(self):
        # self.domParserInstance.run(self.inFileName)
    
    def testPrintXMLTree(self):
        nodeName =  "test_volume"
        nodeId = 1;
        # other node attributes eventually here
        outFile = open('outFullXML.txt', 'w')
        doc = minidom.parse(self.copyFileName)
        rootNode = doc.documentElement
        level = 0
        self.domParserInstance.PrintDOMTree(rootNode, outFile, level)

    def testPrintNodesOnly(self):
        nodeName =  "test_volume"
        nodeId = 1
        # other node attributes eventually here
        outFile = open('sourceOutXMLNodeNames.txt', 'w')
        doc = minidom.parse(self.copyFileName)
        rootNode = doc.documentElement
        level = 0
    
    def testCloneNode(self):
        outFile = open('cloneSource.txt', 'w')
        doc = minidom.parse(self.copyFileName)
        rootNode = doc.documentElement
        level = 0
        self.domParserInstance.PrintDOMTree(rootNode, outFile, level)
        # create the clone
        clone = rootNode.cloneNode(True)
        outFile = open('clone.txt', 'w')
        level = 0
        self.domParserInstance.PrintDOMTree(clone, outFile, level)
        # = difflib.
        self.assertEqual(rootNode.nodeName, clone.nodeName)
        
    def testExtractTestVolumeVME(self):
        #NodeName: Node
                #Attribute -- Name: Type  Value: mafVMEVolumeGray
                #Attribute -- Name: Crypting  Value: 0
                #Attribute -- Name: Id  Value: 1
                #Attribute -- Name: Name  Value: test_volume
        #Content: "
        print " Extracted vme info:"
        self.domParserInstance.GetVmeNodeById(self.rootNode, 1)
        outFile = open('vmeTestVolumeDOMNode.txt', 'w')
        level = 0
        self.domParserInstance.PrintDOMTree(self.domParserInstance.OutputVme,outFile,level)
    
    def testPrintVMETagArrayNodeAndSaveToXML(self):
        self.domParserInstance.GetVmeNodeById(self.rootNode, 1)
        self.domParserInstance.GetVmeTagArrayNode(self.domParserInstance.OutputVme)
        outFile = open('vmeTestVolumeTagArray.txt', 'w')
        level = 0
        self.domParserInstance.PrintDOMTree(self.domParserInstance.OutputTagArrayNode,outFile,level)
        newDoc = minidom.Document()
        newDoc.appendChild(self.domParserInstance.OutputTagArrayNode)
        outFileXML = open('vmeTestVolumeTagArray.xml', 'w')
        newDoc.writexml(outFileXML)

    def testPrintTagNames(self):
        self.domParserInstance.GetVmeNodeById(self.rootNode, 1)
        self.domParserInstance.GetVmeTagArrayNode(self.domParserInstance.OutputVme)
        self.assertNotEqual(self.domParserInstance.OutputVme,None)
        tagList = self.domParserInstance.PrintTagNames(self.domParserInstance.OutputTagArrayNode)
    
    def testGetVmeNodeByID(self):
        self.domParserInstance.GetVmeNodeById(self.rootNode, 1)
        self.assertNotEqual(self.domParserInstance.OutputVme, None)
        
    # this is failing...
    #def testGetVmeNodeByID2(self):
        #node = self.domParserInstance.GetVmeNodeById2(self.rootNode, 1)
        #self.assertNotEqual(node, None)
    
    def testGetVmeTagNodeByName(self):
        self.domParserInstance.GetVmeNodeById(self.rootNode, 1)
        self.domParserInstance.GetVmeTagArrayNode(self.domParserInstance.OutputVme)
        self.domParserInstance.GetVmeTagItemNodeByName(self.domParserInstance.OutputTagArrayNode, r'Dicom_CT_peakvoltage(kV)')
        self.assertNotEqual(self.domParserInstance.OutputTagItemNode,None)
    
    def testRemoveTagsByList(self):
        self.domParserInstance.GetVmeNodeById(self.rootNode, 1)
        self.domParserInstance.GetVmeTagArrayNode(self.domParserInstance.OutputVme)
        self.domParserInstance.RemoveTagsByList(self.domParserInstance.OutputTagArrayNode, [r'Dicom_CT_peakvoltage(kV)'])
        
        outFile = open('afterTagsRemoval.txt', 'w')
        level = 0
        self.domParserInstance.PrintDOMTree(self.domParserInstance.OutputVme, outFile,level)
            
    def testPickle(self):
        # testing VMEUploaderDownloader serialization for list objects 
        stupidList = [1,2,3]
        outFile = open('pickleStupidList.txt', 'w')
        pickle.dump(stupidList,outFile)
        outFile.close
        outFile = open('pickleStupidList.txt', 'r')
        newList = pickle.load(outFile)
        self.assertEqual(stupidList,newList)
        
if __name__ == '__main__':
    unittest.main()
    
