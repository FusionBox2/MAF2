#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

# VMEUploaderDownloader msfParser.py "D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\msf_test_import_export_VME\msf_test_import_export_VME.msf"

 #NodeName: Name
            #Content: "TagArray"
            #NodeName: TItem
                    #Attribute --0 Name: Type  Value: STR
                    #Attribute --0 Name: Name  Value: Dicom_CT_model
                    #Attribute --0 Name: Mult  Value: 1
            #Content: "
               
            #"
                #NodeName: TItem
                #Content: "
                 
              #"
                    #NodeName: TC
                    #Content: "GE SuperGulp"
            #NodeName: TItem
                    #Attribute --0 Name: Type  Value: NUM
                    #Attribute --0 Name: Name  Value: Dicom_CT_peakvoltage(kV)
                    #Attribute --0 Name: Mult  Value: 1
            #Content: "
               
            #"
                #NodeName: TItem
                #Content: "
                 
              #"
                    #NodeName: TC
                    #Content: "120"
            #NodeName: TItem
                    #Attribute --0 Name: Type  Value: NUM
                    #Attribute --0 Name: Name  Value: Dicom_CT_tubecurrent(mA)
                    #Attribute --0 Name: Mult  Value: 1
            #Content: "
               
            #"
                #NodeName: TItem
                #Content: "
                 
              #"
                    #NodeName: TC
                    #Content: "160"
            #NodeName: TItem
                    #Attribute --0 Name: Type  Value: STR
                    #Attribute --0 Name: Name  Value: VME_NATURE
                    #Attribute --0 Name: Mult  Value: 1
            #Content: "
               
            #"
                #NodeName: TItem
                #Content: "
                 
              #"
                    #NodeName: TC
                    #Content: "NATURAL"
       
       
import sys, string
from xml.dom import minidom
from xml.dom import Node

class lhpbDictionary:
    """store and retrieve list of tags in 1 column"""
    
    def __init__(self):                                
        self.Clear()
        
    def Clear(self):
        self.DictionaryFileName = ""
        self.DictionaryTagsList = []
    
    def Load(self):
        self.DictionaryTagsList = []
        f = open(self.DictionaryFileName, 'r')
        for line in f:
            lineStripped = str(line).strip()
            self.DictionaryTagsList.append(lineStripped)        
        f.close()    
        
    def Save(self):
        f = open(self.DictionaryFileName, 'w')
        for tag in self.DictionaryTagsList:
            print >> f, tag
        f.close()
    
    def Print(self):
        print "\nDictionary file name: " + self.DictionaryFileName
        print "\nDictionary content:" 
        i = 0
        for tag in self.DictionaryTagsList:
            print "item: " + str(i) + "     " + "value: " + str(tag) 
            i+=1
            
class msfParser:
    """Facilities to handle MSF DOM Tree"""
    
    def __init__(self):                              
        self.__OutputVme = None 
        self.__OutputTagArrayNode = None
        self.__OutputTagItemNode = None
        self.__OutputNode = None
    
    def PrintDOMTree(self,parent, outFile):
        level = 0
        self.__PrintDOMTreeInternal(parent,outFile,level)
        
    def __PrintDOMTreeInternal(self,parent, outFile, level):  
        self.PrintNode(parent, outFile, level)
        if parent.childNodes:
            for node in parent.childNodes:
                self.__PrintDOMTreeInternal(node, outFile, level+1)
    
    def PrintNode(self,node,outFile,level):
        if node.nodeType == Node.ELEMENT_NODE:
            # Write out the element name.
            self.__printLevel(outFile, level)
            outFile.write('NodeName: %s\n' % node.nodeName)
            # Write out the attributes.
            attrs = node.attributes                            
            for attrName in attrs.keys():
                attrNode = attrs.get(attrName)
                attrValue = attrNode.nodeValue
                self.__printLevel(outFile, level + 2)
                outFile.write('Attribute --0 Name: %s  Value: %s\n' % \
                    (attrName, attrValue))
            # Walk over any text nodes in the current node.
            content = []                                      
            for child in node.childNodes:
                if child.nodeType == Node.TEXT_NODE:
                    content.append(child.nodeValue)
            if content:
                strContent = string.join(content)
                self.__printLevel(outFile, level)
                outFile.write('Content: "')
                outFile.write(strContent)
                outFile.write('"\n')
                
    def GetVmeTagArrayNode(self,inputVmeNode):
        self.__GetVmeTagArrayNodeInternal(inputVmeNode)
        return self.__OutputTagArrayNode
    
    def __GetVmeTagArrayNodeInternal(self,inputVmeNode):
        #NodeName: Name
            #Content: "TagArray"
        for node in inputVmeNode.childNodes:
            #  search for a Node with name "Node"...0
            
            if node.nodeType == Node.ELEMENT_NODE:
                if node.nodeName == "Item":
                    # get node attributes
                    attrs = node.attributes                             
                    for attrName in attrs.keys():
                        attrNode = attrs.get(attrName)
                        attrValue = attrNode.nodeValue
                        if attrValue  == "mafTagArray":
                            self.__OutputTagArrayNode = node
                self.__GetVmeTagArrayNodeInternal(node)
                
    def GetVmeTagItemNodeByName(self, inputVmeTagArrayNode, tagName):
        self.GetVmeTagItemNodeByNameInternal(inputVmeTagArrayNode, tagName)
        return self.__OutputTagItemNode
        
    def GetVmeTagItemNodeByNameInternal(self, inputVmeTagArrayNode, tagName):
        for node in inputVmeTagArrayNode.childNodes:
        #  search for a Node with name "Node"...0
            if node.nodeType == Node.ELEMENT_NODE:
                if node.nodeName == "TItem":
                    # get node attributes
                    attrs = node.attributes                             
                    for attrName in attrs.keys():
                        attrNode = attrs.get(attrName)
                        attrValue = attrNode.nodeValue
                        if (attrValue == tagName):
                            assert(isinstance(attrNode,Node))
                            self.__OutputTagItemNode = node
                        
        
    def RemoveTagsByList(self, inputVmeTagArrayNode, tagsToBeRemoved):
        # change number of tags
        # <Item NumberOfTags="2" Type="mafTagArray">
        removedTagsNumber = 0
        for node in inputVmeTagArrayNode.childNodes:
        #  search for a Node with name "Node"...0
            if node.nodeType == Node.ELEMENT_NODE:
                if node.nodeName == "TItem":
                    # get node attributes
                    attrs = node.attributes                             
                    for attrName in attrs.keys():
                        attrNode = attrs.get(attrName)
                        attrValue = attrNode.nodeValue
                        for tagName in tagsToBeRemoved:
                            if (attrValue == tagName):
                                inputVmeTagArrayNode.removeChild(node)
                                removedTagsNumber += 1
        attrs = inputVmeTagArrayNode.attributes                             
        for attrName in attrs.keys():
            print attrName
            if attrName  == "NumberOfTags":
               attrNode = attrs.get(attrName)
               oldTagsNumber = attrNode.nodeValue
               print oldTagsNumber
               attrNode.nodeValue = str(int(oldTagsNumber) - removedTagsNumber)
               print attrNode.nodeValue
               
    def PrintTagNames(self, tagArrayNode):
        """Print the given tagArrayNode tags list, also return the tagList"""
        tagList = []
        for node in tagArrayNode.childNodes:
        #  search for a Node with name "Node"...0
            if node.nodeType == Node.ELEMENT_NODE:
                if node.nodeName == "TItem":
                    # get node attributes
                    attrs = node.attributes                             
                    for attrName in attrs.keys():
                        attrNode = attrs.get(attrName)
                        attrValue = attrNode.nodeValue
                        if attrName  == "Name":
                            tagList.append(attrValue)
        
        # print tagList         
        print "\nVME tagArray node tag names:\n"
        for el in range(len(tagList)):
            print tagList[el]
        print "Number of tags: " + str(len(tagList))
        return tagList
    
    def __GetVmeNodeByIdInternal(self, vmeTreeRootNode, vmeId):
        """Get a vme given the vme tree root and its Id, since the vme ID is unique
        in a well formed MSF this is always returning one and only one node"""
        #NodeName: Node
                #Attribute -- Name: Type  Value: mafVMEVolumeGrayAnd
                
                #Attribute -- Name: Crypting  Value: 0
                #Attribute -- Name: Id  Value: 1
                #Attribute -- Name: Name  Value: test_volume
        #Content: "
        # for each child...
        for node in vmeTreeRootNode.childNodes:
            #  search for a Node with name "Node" ie a vme
            if node.nodeType == Node.ELEMENT_NODE:
                if node.nodeName == "Node":
                    print('NodeName: %s\n' % node.nodeName)
                    # and an attribute "Id"...
                    attrs = node.attributes                            
                    for attrName in attrs.keys():
                        attrNode = attrs.get(attrName)
                        attrValue = attrNode.nodeValue
                        if attrName  == "Id": 
                            if eval(attrValue) == vmeId: 
                                print('Attribute -- Name: %s  Value: %s\n' % \
                                    (attrName, attrValue))
                                self.__OutputVme = node
                                break
                            # exit here and return the found node...
            # search children
            self.__GetVmeNodeByIdInternal(node, vmeId)
            
    def GetVmeNodeById(self, vmeTreeRootNode, vmeId):
        self.__GetVmeNodeByIdInternal(vmeTreeRootNode, vmeId)
        # return vme node given its Id and the root node 
        return self.__OutputVme
        
    def __GetVmeNodeByIdInternal2(self, vmeTreeRootNode, vmeId):
        """Prototype and not working... should get a vme given the vme tree root and its Id"""
        #NodeName: Node
                #Attribute -- Name: Type  Value: mafVMEVolumeGrayAnd
                
                #Attribute -- Name: Crypting  Value: 0
                #Attribute -- Name: Id  Value: 1
                #Attribute -- Name: Name  Value: test_volume
        #Content: "
        for node in vmeTreeRootNode.childNodes:
            if node.nodeType == Node.ELEMENT_NODE:
                if node.nodeName == "Node":
                    print('NodeName: %s\n' % node.nodeName)
                    # and an attribute "Id"...
                    attrs = node.attributes                            
                    for attrName in attrs.keys():
                        attrNode = attrs.get(attrName)
                        attrValue = attrNode.nodeValue
                        if attrName  == "Id": 
                            if eval(attrValue) == vmeId: 
                                print('Attribute -- Name: %s  Value: %s\n' % \
                                    (attrName, attrValue))
                                self.__OutputVme = node 
                                break
            else:
                self.__GetVmeNodeByIdInternal2(node, vmeId)
                
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
    
    def PrintVmeNodes(self, parent, outFile):
        level = 0
        self.PrintVmeNodesInternal(parent, outFile, level)
        
    def PrintVmeNodesInternal(self,parent, outFile, level):                               
        for node in parent.childNodes:
            if node.nodeType == Node.ELEMENT_NODE:
                # Write out the element name.            
                if node.nodeName == "Node":
                    self.__printLevel(outFile, level)
                    self.PrintNode(node, outFile, level)
                    # Write out the attributes.
            self.PrintVmeNodesInternal(node, outFile, level+1)
    
    def __printLevel(self,outFile, level):
        for idx in range(level):
            outFile.write('    ')

        
        
def run(inFileName):                                            
    outFile = sys.stdout
    doc = minidom.parse(inFileName)
    rootNode = doc.documentElement
    level = 0
    PrintDOMTree(rootNode, outFile, level)

def main():
    args = sys.argv[1:]
    if len(args) != 1:
        print 'usage: VMEUploaderDownloader test.py infile.xml'
        sys.exit(-1)
    run(args[0])


if __name__ == '__main__':
    main()

