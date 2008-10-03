#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------
from test.test_codecs import Str2StrTest
from pickle import NONE

import sys, string
from Debug import Debug
from xml.dom import minidom 
from xml.dom import Node
import Enum
import os
from string import split
            
class lhpXMLDictionaryParser:
    """Facilities to handle lhpBuilder XML dictionary"""
    
    def __init__(self):                              
        
        # dictionary columns from XML dictionary csv source
        self.DictionaryColumnLabels = Enum.Enum([\
            'NumTag', 'L1', 'L2', 'L3', 'L4', 'L5', 'L6', 'L7', 'L8', \
            'ValueType', 'Multiplicity', 'ValueList', 'Editable', 'Searchable', \
            'DefaultValue', 'Expert', 'Notes'])
      
        self.TagsList = []
        self.XMLDictionaryFileName = "UNDEFINED"
        self.DictionaryDOMDocument = None
        self.DictionaryDOMDocumentRoot = None
        
        
    def LoadXMLDictionary(self, xmlDictFileName):
        """Load lhpBuilder XML dictionary"""
        self.XMLDictionaryFileName = xmlDictFileName
        self.DictionaryDOMDocument = minidom.parse(xmlDictFileName)
        self.DictionaryDOMDocumentRoot = self.DictionaryDOMDocument.firstChild
        
    
    def GetXMLDictionaryNodeNamesList(self):
        """Return all XML dictionary nodes names list
        For example:
        [u'L0000', u'resource', u'data', u'Type',...]
        """
        self.TagsList = []
        self.__GetTagsListInternal(self.DictionaryDOMDocumentRoot)
        return self.TagsList
    
    # Attribute Name: Editable  Value: n
    def GetXMLDictionaryAutoNodeNamesList(self):
        """Return XML nodes names list
        For example:
        [u'L0000', u'resource', u'data', u'Type',...]
        """
        self.AutoTagsList = []
        self.__GetAutoTagsListInternal(self.DictionaryDOMDocumentRoot)
        return self.AutoTagsList
    
    def GetVMETagArrayTagNamesList(self):
        """Return list of every dictionary tags ie auto and manual as stored in vme tag array
        For example:
        [...
        L0000_resource_Pricing_Quotation1,
        L0000_resource_Pricing_Quotation1_GroupID,
        L0000_resource_Pricing_Quotation1_Price,
        L0000_resource_Pricing_Quotation1_Policy,
        ...]
        """
        self.TagArrayTagList = []
        self.__GetTagArrayTagsInternal(self.DictionaryDOMDocumentRoot, sys.stdout, 0)
        return self.TagArrayTagList
    
    def GetVMETagArrayAutoTagNamesList(self):
        """Return the list of auto tags only ie tags that should be handled by 
        lhpbuilder tag factory as stored in vme tag array
        For example:
        [...
        L0000_resource_Pricing_AutoTagPippo,
        L0000_resource_Pricing_Quotation1_AutoTagPluto,
        ...]
        """
        self.TagArrayAutoTagList = []
        self.__GetTagArrayAutoTagsInternal(self.DictionaryDOMDocumentRoot, sys.stdout, 0)
        return self.TagArrayAutoTagList
    
    
    def GetVMETagArrayManualTagNamesList(self):
        """Return the list of manual tags ie tags that should be filled by the user as stored
        in vme tag array
        For example:
        [...
        L0000_resource_Pricing_AutoTagPippo,
        L0000_resource_Pricing_Quotation1_AutoTagPluto,
        ...]
        """
        self.TagArrayManualTagList = []
        self.__GetTagArrayManualTagsInternal(self.DictionaryDOMDocumentRoot, sys.stdout, 0)
        return self.TagArrayManualTagList
    
    def GetVMETagArrayTagNameFromNode(self, node):
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
    
    
    def GetNodeName(self, node):
        """Return the node name"""
        if node.nodeType == Node.ELEMENT_NODE:
            return node.nodeName
    
    def IsAuto(self, node):
        """Return if the node represents an auto tag ie a
        tag that should be filled automatically"""
        if node.nodeType == Node.ELEMENT_NODE:
            if Debug:
                print "Node name: " + node.nodeName
            attrs = node.attributes                             
            for attrName in attrs.keys():
                attrNode = attrs.get(attrName)
                attrValue = attrNode.nodeValue
                if Debug:
                    print "Attribute name: " + attrName + "  Attribute value: " + attrValue
                if attrName == self.DictionaryColumnLabels.Editable.asString :
                    # Attribute Name: Editable    Value: n => AUTO
                    if attrValue == "n":
                        return True
                    # Attribute Name: Editable    Value: y => MANUAL
                    elif attrValue == "y":
                        return False
                    else:
                        # tag with no Editable attribute is manual
                        print "Editable tag not found for: " + attrName + "tag"
                        return False
            # tags with no attributes at all is manual
            print "Tag: " + node.nodeName + " has no attributes... "
            return False
    
    def GetAttributesDictionary(self, node):
        """ Return node attributes  and values in a dictionary, for example
        
         KEY            VALUE
        Expert:         Pippo
        Searchable:     n
        Multiplicity:   1
        DefaultValue:   0
        Editable:       n
        ValueList       0, 1, 2, 3
        Notes:          This is a note
        ValueType:      tag

        """
        dictionary = {}
        if node.nodeType == Node.ELEMENT_NODE:
            if Debug:
                print "Node name: " + node.nodeName
            attrs = node.attributes     
            print attrs.keys                        
            for attribute in attrs.keys():
                attrNode = attrs.get(attribute)
                attrValue = attrNode.nodeValue
                if Debug:
                    print "Attribute name: " + attribute + "  Attribute value: " + attrValue
                dictionary[attribute] = attrValue
            return dictionary
        return None
        
    def GetExpert(self, node):
        dictionary = self.GetAttributesDictionary(node)
        return str(dictionary["Expert"])
    
    def IsSearchable(self, node):
        dictionary = self.GetAttributesDictionary(node)
        if (dictionary["Searchable"] == "y"):
            return True
        elif (dictionary["Searchable"] == "n"):
            return False
        
    def GetMultiplicity(self, node):
        dictionary = self.GetAttributesDictionary(node)
        multiplicity = dictionary["Multiplicity"]
        if multiplicity:
            return int(multiplicity)
        else:
            return None
    
    def GetDefaultValue(self, node):
        dictionary = self.GetAttributesDictionary(node)
        defaultValue = dictionary["DefaultValue"]
        if defaultValue:
            return defaultValue
        else:
            return None
        
    def IsEditable(self, node):
        dictionary = self.GetAttributesDictionary(node)
        if (dictionary["Editable"] == "y"):
            return True
        elif (dictionary["Editable"] == "n"):
            return False
        
    def GetValueList(self,node):
        dictionary = self.GetAttributesDictionary(node)
        valueList = dictionary["ValueList"]
        if valueList:
            return valueList.split(",")
        else:
            return None
        
    def GetNotes(self, node):
        dictionary = self.GetAttributesDictionary(node)
        notes = dictionary["Notes"]
        if notes:
            return str(notes)
        else:
            return None

    def GetValueType(self, node):
        dictionary = self.GetAttributesDictionary(node)
        valueType = dictionary["ValueType"]
        if valueType:
            return str(valueType)
        else:
            return None

    def PrintXMLDictionary(self):
        """ print XML dictionary to standard output """
        self.PrintXML(self.DictionaryDOMDocumentRoot, sys.stdout)
       
    def PrintXML(self, parent, outFile):
        """ Print XML starting from given parent node to output file outFile"""
        level = 0
        self.__PrintXMLDictionaryInternal(parent, outFile, level)

    def __PrintXMLDictionaryInternal(self, parent, outFile, level):  
        self.PrintNode(parent, outFile, level)
        if parent.childNodes:
            for node in parent.childNodes:
                self.__PrintXMLDictionaryInternal(node, outFile, level+1)
    
    def PrintNode(self, node, outFile, level):
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
    
    def __GetParent(self, node):
        """Cycle on every node's parent"""
        name = self.GetNodeName(node)
        self.Names.append(name)
        if node.parentNode:
           self.__GetParent(node.parentNode)       
    
    def __GetTagArrayTagsInternal(self, parent, outFile, level):  
        self.TagArrayTagList.append(self.GetVMETagArrayTagNameFromNode(parent))
        if parent.childNodes:
            for node in parent.childNodes:
                self.__GetTagArrayTagsInternal(node, sys.stdout, level)
    
    def __GetTagArrayAutoTagsInternal(self, parent, outFile, level):  
        if self.IsAuto(parent) == True:    
            self.TagArrayAutoTagList.append(self.GetVMETagArrayTagNameFromNode(parent))
        if parent.childNodes:
            for node in parent.childNodes:
                self.__GetTagArrayAutoTagsInternal(node, sys.stdout, level)
    
    def __GetTagArrayManualTagsInternal(self, parent, outFile, level):  
        if self.IsAuto(parent) == False:    
            self.TagArrayManualTagList.append(self.GetVMETagArrayTagNameFromNode(parent))
        if parent.childNodes:
            for node in parent.childNodes:
                self.__GetTagArrayManualTagsInternal(node, sys.stdout, level)
    
    def __GetTagsListInternal(self, parent):
        self.TagsList.append(self.GetNodeName(parent))
        if parent.childNodes:
            for node in parent.childNodes:
                self.__GetTagsListInternal(node)
    
    def __GetAutoTagsListInternal(self, parent):
        if self.IsAuto(parent):           
            self.AutoTagsList.append(self.GetNodeName(parent))
        if parent.childNodes:
            for node in parent.childNodes:
                self.__GetAutoTagsListInternal(node)
    
        
    def __printLevel(self, outFile, level):
        for idx in range(level):
            outFile.write('    ')
    
    def GetChildNodeByName(self, parent, searchedName):
        """Get the child node by name. Return none if non existant 
        domDocumentNode
            ElementNode Name: L0000
                ElementNode Name: resource
                        ElementNode Name: data
                        ElementNode Name: DataType
            
        GetChildNodeByName(domDocumentNode, L0000): returns L0000 node
        GetChildNodeByName(domDocumentNode, resource): returns None
        """
        if parent.hasChildNodes():
            for node in parent.childNodes:
                name = self.GetNodeName(node)
                if name == searchedName:
                    return node
        return None
    
    def GetNodeFromVMETagArrayTagName(self, vmeTagArrayTagName):
        """Get corresponding xml dictionary node from vme tag item name:
        For example: given L0000_resource_MAF_TreeInfo_VmeRootURI as argument
        the VmeRootURI named dom node is returned if it exists otherwise None"""
        assert(self.DictionaryDOMDocument)
        splitted = split(vmeTagArrayTagName, '_')
        print splitted
        
        currentParent = self.DictionaryDOMDocument
        
        for name in splitted:
            child = self.GetChildNodeByName(currentParent, name)
            if child:
                currentParent = child
            else:
                return None
        
        return currentParent
        

def run(xmlDictionaryFilename, command, outputTagsFileName):
    """"""
    # load XML dictionary
    lhpXMLDictionaryParserInstance = lhpXMLDictionaryParser()
    lhpXMLDictionaryParserInstance.LoadXMLDictionary(xmlDictionaryFilename)
    
    tags = []
    
    if command == 'auto_tags':
    # generates auto tags    
         tags = lhpXMLDictionaryParserInstance.GetVMETagArrayAutoTagNamesList()
         
    elif command == 'manual_tags':
        tags = lhpXMLDictionaryParserInstance.GetVMETagArrayManualTagNamesList()
    
    else:
        print 'command not available!'
        sys.exit(1)

    if Debug: 
        print "Beware:  In order to work run this est must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
      
        for tag in tags:
             print tag   
      
    
    # Save
    tagsFile = open(outputTagsFileName, 'w')
    for tag in tags:
        print "storing: " + str(tag)
        print >> tagsFile, tag
    tagsFile.close()

    return 

if __name__ == '__main__':
    import sys
    usage_msg = '''Usage: %s <option>
where option can be:
auto_tags - get auto tags from dictionary
manual_tags - get manual tags from dictionary
''' % sys.argv[0]

    if len(sys.argv) != 4:
        print 'Error :\n' + usage_msg
        sys.exit(1)

    xmlDictionaryFilename = sys.argv[1]
    command = sys.argv[2]
    outputTagsFileName = sys.argv[3]
    
    run(xmlDictionaryFilename, command, outputTagsFileName)
