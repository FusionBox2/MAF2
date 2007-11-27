#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import sys, string
import Debug
from xml.dom import minidom
from xml.dom import Node
            
class lhpXMLDictionaryParser:
    """Facilities to handle lhpBuilder XML dictionary"""
    
    def __init__(self):                              
        
        self.TagsList = []
        self.XMLDictionaryFileName = "UNDEFINED"
        self.DictionaryDOMDocument = None
        self.DictionaryDOMDocumentRoot = None
        
    def LoadXMLDictionary(self, xmlDictFileName):
        """Load lhpBuilder XML dictionary"""
        self.XMLDictionaryFileName = xmlDictFileName
        self.DictionaryDOMDocument = minidom.parse(xmlDictFileName)
        self.DictionaryDOMDocumentRoot = self.DictionaryDOMDocument.firstChild
        
    def PrintXMLDictionary(self):
        """ print XML dictionary to standard output """
        self.PrintXML(self.DictionaryDOMDocumentRoot, sys.stdout)
    
    def GetXMLDictionaryTagsList(self):
        """Return XML nodes names list
        For example:
        [u'L0000', u'resource', u'data', u'Type',...]
        """
        self.TagsList = []
        self.__GetTagsListInternal(self.DictionaryDOMDocumentRoot)
        return self.TagsList
        
    def GetTagArrayTagsList(self):
        """Return a list of tags as stored in vme tag array
        For example:
        [...
        L0000_resource_Pricing_Quotation1,
        L0000_resource_Pricing_Quotation1_GroupID,
        L0000_resource_Pricing_Quotation1_Price,
        L0000_resource_Pricing_Quotation1_Policy,
        ...]
        """
        self.TagArrayTaglList = []
        self.__GetTagArrayTagsInternal(self.DictionaryDOMDocumentRoot, sys.stdout, 0)
        return self.TagArrayTaglList
    
    def GetTagArrayTagName(self, node):
        """Given a XML dictionary node get its corresponding flat tag name in VME tagarray
        ie, for example, from Price node return L0000_resource_Pricing_Quotation1_Price 
        which is the node parents hierarchy with _ between tag names"""
        self.Names = []
        isinstance(node, minidom.Node)
        self.__GetParent(node)
        if Debug:
            print "node name: " + self.GetNodeName(node)
            print "parents list: "
            print self.Names
        self.Names.reverse()
        tagName = self.Names[1]
        for name in self.Names[2:]:
            tagName = tagName + "_" + str(name) 
        if Debug:
            print tagName
        return tagName
    
    def __GetParent(self, node):
        """Cycle on every node's parent"""
        name = self.GetNodeName(node)
        self.Names.append(name)
        if node.parentNode:
           self.__GetParent(node.parentNode)       
    
    def __GetTagArrayTagsInternal(self,parent, outFile, level):  
        self.TagArrayTaglList.append(self.GetTagArrayTagName(parent))
        if parent.childNodes:
            for node in parent.childNodes:
                self.__GetTagArrayTagsInternal(node, sys.stdout, level)
    
    def __GetTagsListInternal(self, parent):
        self.TagsList.append(self.GetNodeName(parent))
        if parent.childNodes:
            for node in parent.childNodes:
                self.__GetTagsListInternal(node)
    
    def GetNodeName(self,node):
        """Return the node name"""
        if node.nodeType == Node.ELEMENT_NODE:
            if Debug:
                print node.nodeName
            return node.nodeName
            
    def PrintXML(self, parent, outFile):
        """ Print XML starting from given parent node to output file outFile"""
        level = 0
        self.__PrintXMLDictionaryInternal(parent,outFile,level)
        
    def __PrintXMLDictionaryInternal(self,parent, outFile, level):  
        self.PrintNode(parent, outFile, level)
        if parent.childNodes:
            for node in parent.childNodes:
                self.__PrintXMLDictionaryInternal(node, outFile, level+1)
    
    def PrintNode(self,node,outFile,level):
        """Print node on output file outFile with level indentation"""
        if node.nodeType == Node.ELEMENT_NODE:
            # Write out the element name.
            self.__printLevel(outFile, level)
            outFile.write('ElementNode Name: %s\n' % node.nodeName)
            # Write out the attributes.
            attrs = node.attributes                            
            for attrName in attrs.keys():
                attrNode = attrs.get(attrName)
                attrValue = attrNode.nodeValue
                self.__printLevel(outFile, level + 2)
                outFile.write('Attribute Name: %s  Value: %s\n' % \
                    (attrName, attrValue))
    
    def GetNodeByNodeName(self, parentNode, nodeName):
        """Return the first ELEMENT_NODE nodeType node found with name nodeName descending from parentNode"""
        self.__GetNodeByNodeNameInternal(parentNode, nodeName)
        return self.__OutputNode
    
    def __GetNodeByNodeNameInternal(self, parentNode, nodeName):
        for node in parentNode.childNodes:
            if node.nodeType == Node.ELEMENT_NODE:
                if node.nodeName == nodeName:
                    print('NodeName: %s\n' % node.nodeName)
                    self.__OutputNode = node
                    return
            self.__GetNodeByNodeNameInternal(node, nodeName)
    
    
    def __printLevel(self,outFile, level):
        for idx in range(level):
            outFile.write('    ')

        
        
def run(inFileName):                                            
    outFile = sys.stdout
    doc = minidom.parse(inFileName)
    rootNode = doc.documentElement
    level = 0
    PrintXMLDictionary(rootNode, outFile, level)

def main():
    args = sys.argv[1:]
    if len(args) != 1:
        print 'usage: VMEUploaderDownloader test.py infile.xml'
        sys.exit(-1)
    run(args[0])


if __name__ == '__main__':
    main()

