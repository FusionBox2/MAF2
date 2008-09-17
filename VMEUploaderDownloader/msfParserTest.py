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

import Debug
import msfParser
import sys, string
from xml.dom import minidom
from xml.dom import Node

import unittest
import shutil
import difflib
import pickle
import os


class msfParserTest(unittest.TestCase):
 
        
    def setUp(self):
        
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
        
        self.inFileName = curDir + r'\msf_test_import_export_VME\msf_test_import_export_VME.msf'
        self.copyFileName = curDir + r'\msf_test_import_export_VME\copyied_msf_test_import_export_VME.msf'
        self.msfParserInstance = msfParser.msfParser()
        shutil.copy(self.inFileName, self.copyFileName)
        self.doc = minidom.parse(self.copyFileName)
        self.rootNode = self.doc.documentElement
            
    def testPrintXMLTree(self):
        nodeName =  "test_volume"
        nodeId = 1;
        # other node attributes eventually here
        outFile = open('outFullXML.txt', 'w')
        doc = minidom.parse(self.copyFileName)
        rootNode = doc.documentElement
        level = 0
        self.msfParserInstance.PrintDOMTree(rootNode, outFile)

    def testPrintNodesOnly(self):
        nodeName =  "test_volume"
        nodeId = 1
        # other node attributes eventually here
        outFile = open('sourceOutXMLNodeNames.txt', 'w')
        doc = minidom.parse(self.copyFileName)
        rootNode = doc.documentElement
        p = self.msfParserInstance
        p.PrintVmeNodes(rootNode,outFile)
        
    def testCloneNode(self):
        outFile = open('cloneSource.txt', 'w')
        doc = minidom.parse(self.copyFileName)
        rootNode = doc.documentElement
        level = 0
        self.msfParserInstance.PrintDOMTree(rootNode, outFile)
        # create the clone
        clone = rootNode.cloneNode(True)
        outFile = open('clone.txt', 'w')
        level = 0
        self.msfParserInstance.PrintDOMTree(clone, outFile)
        # = difflib.
        self.assertEqual(rootNode.nodeName, clone.nodeName)
        
    def testExtractTestVolumeVME(self):
        #NodeName: Node
                #Attribute -- Name: T5ype  Value: mafVMEVolumeGray
                #Attribute -- Name: Crypting  Value: 0
                #Attribute -- Name: Id  Value: 1
                #Attribute -- Name: Name  Value: test_volume
        #Content: "
        print " Extracted vme info:"
        p = self.msfParserInstance
        vme = p.GetVmeNodeById(self.rootNode, 1)
        outFile = open('vmeTestVolumeDOMNode.txt', 'w')
        p.PrintDOMTree(vme,outFile)
    
    def testPrintVMETagArrayNodeAndSaveToXML(self):
        p = self.msfParserInstance
        vme = p.GetVmeNodeById(self.rootNode, 1)
        tagArray = p.GetVmeTagArrayNode(vme)
        outFile = open('vmeTestVolumeTagArray.txt', 'w')
        p.PrintDOMTree(tagArray,outFile)
        newDoc = minidom.Document()
        newDoc.appendChild(tagArray)
        outFileXML = open('vmeTestVolumeTagArray.xml', 'w')
        newDoc.writexml(outFileXML)

    def testGetTagNames(self):
        p = self.msfParserInstance
        vme = p.GetVmeNodeById(self.rootNode, 1)
        tagArray = p.GetVmeTagArrayNode(vme)
        tagList = p.GetTagNames(tagArray)
    
    def testGetVmeNodeByID(self):
        vme = self.msfParserInstance.GetVmeNodeById(self.rootNode, 1)
        self.assertNotEqual(vme, None)
    
    def testGetVmeTagNodeByName(self):
        p = self.msfParserInstance
        vme = p.GetVmeNodeById(self.rootNode, 1)
        tagArray = p.GetVmeTagArrayNode(vme)
        tagItem = p.GetVmeTagItemNodeByName(tagArray, r'Dicom_CT_peakvoltage(kV)')
        self.assertNotEqual(tagItem,None)
    
    def testGetVMEDataURLList(self):
        p = self.msfParserInstance
        vme = p.GetVmeNodeById(self.rootNode, 1)
        data = p.GetVMEDataURLList(vme)
        print data
        self.assertEqual(data[0],"TESTmsf.1.vtk")
        self.assertEqual(len(data),1)
    
    def testRemoveTagsByList(self):
        p = self.msfParserInstance
        vme = p.GetVmeNodeById(self.rootNode, 1)
        tagArray = p.GetVmeTagArrayNode(vme)
        p.RemoveTagsByList(tagArray, [r'Dicom_CT_peakvoltage(kV)'])
        
        outFile = open('afterTagsRemoval.txt', 'w')
        p.PrintDOMTree(vme, outFile)
            
    def testPickle(self):
        # testing VMEUploaderDownloader serialization for list objects 
        stupidList = [1,2,3]
        outFile = open('pickleStupidList.txt', 'w')
        pickle.dump(stupidList,outFile)
        outFile.close
        outFile = open('pickleStupidList.txt', 'r')
        newList = pickle.load(outFile)
        self.assertEqual(stupidList,newList)
        
    
    def testAddTagsFromList(self):
        p = self.msfParserInstance
        vme = p.GetVmeNodeById(self.rootNode, 1)
        tagArray = p.GetVmeTagArrayNode(vme)
        self.assertNotEqual(tagArray, None)
        p.PrintNodeToScreen(tagArray)
        p.AddTagsFromList(self.doc,tagArray, [r'Dicom_CT_peakvoltage(kV)',r'pippo',r'pluto'])
        
        outFile = open('afterTagsAdding.txt', 'w')
        p.PrintDOMTree(vme, outFile)
    
    def testGetTagNodeByTagName(self):
        
        p = self.msfParserInstance
        vme = p.GetVmeNodeById(self.rootNode, 1)
        tagArray = p.GetVmeTagArrayNode(vme)
        self.assertNotEqual(tagArray, None)
        p.PrintNodeToScreen(tagArray)
        p.AddTagsFromList(self.doc,tagArray, [r'pippo',r'pluto'])
        # pippo exist
        pippoNode = p.GetTagNodeByTagName(tagArray, "pippo")
        self.assertTrue(pippoNode)
        # mazinga doesn't...
        mazingaNode = p.GetTagNodeByTagName(tagArray, "mazinga")        
        self.assertFalse(mazingaNode)
    
    def testSetGetTagNodeText(self) :
        
        p = self.msfParserInstance
        vme = p.GetVmeNodeById(self.rootNode, 1)
        tagArray = p.GetVmeTagArrayNode(vme)
        self.assertNotEqual(tagArray, None)
        p.PrintNodeToScreen(tagArray)
        p.AddTagsFromList(self.doc,tagArray, [r'pippo',r'pluto'])
        pippoNode = p.GetTagNodeByTagName(tagArray, "pippo")
        
        text = p.GetTagNodeText(pippoNode)
        # text before set         
        print text
        self.assertFalse(text == "ciao ciao")
        
        p.SetTagNodeText(pippoNode, "ciao ciao")
        text = p.GetTagNodeText(pippoNode)
        
        # text after set
        print text
        self.assertTrue(text == "ciao ciao")

    def testGetTagDictionary(self):
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
        
        self.inFileName = curDir + r'\metadataEditorTestData\surfaceWithMasterDictionaryTags\surfaceWithMasterDictionaryTags.msf'
        self.assertTrue(os.path.exists(self.inFileName))
        self.msfParserInstance = msfParser.msfParser()
        
        self.doc = minidom.parse(self.inFileName)
        self.rootNode = self.doc.documentElement
        
        p = self.msfParserInstance
        # <Node Crypting="0" Id="18" Name="Surface Parametric" Type="mafVMESurface">
      
        vme = p.GetVmeNodeById(self.rootNode, 18)
        tagArrayNode = p.GetVmeTagArrayNode(vme)
        self.assertNotEqual(tagArrayNode, None)
        # p.PrintNodeToScreen(tagArrayNode)
        
        p.AddTagsFromList(self.doc,tagArrayNode, [r'pippo',r'pluto'])
        
        # pippo exist
        pippoNode = p.GetTagNodeByTagName(tagArrayNode, "pippo")
        self.assertTrue(pippoNode)
        
        # mazinga doesn't...
        mazingaNode = p.GetTagNodeByTagName(tagArrayNode, "mazinga")        
        self.assertFalse(mazingaNode)
    
        tagDictionary = p.GetTagDictionary(tagArrayNode)
        print tagDictionary
        
        for key in tagDictionary.keys():
            print key, '\t', tagDictionary[key]
            
        self.assertTrue(tagDictionary["L0000_resource_MAF_TreeInfo_VmeRootURI"] \
                         == "Automated Tag Not Handled (instance exists)")
        
        self.assertTrue(tagDictionary["L0000_resource_data_DataType_Dimension"] \
                         == "SURFACE")
        
        
if __name__ == '__main__':
    
    unittest.main()
    

