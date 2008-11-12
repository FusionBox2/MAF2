#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni, Roberto Mucci
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
       

from Debug import Debug                    
import sys, string
from xml.dom import minidom
from xml.dom import Node
from xml.dom.minidom import Document

            
class msfParser:
    """Facilities to handle MSF DOM Tree"""
    
    def __init__(self):                              
        self.__OutputVme = None 
        self.__OutputTagArrayNode = None
        self.__OutputTagItemNode = None
        self.__OutputNode = None
        self.extension = ""
        self.extFound = 0
        self.fileNameFound = 0
    
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
                
    
    def PrintNodeToScreen(self,node):
        if node.nodeType == Node.ELEMENT_NODE:
            # Write out the element name.
            if Debug:
                print('NodeName: %s\n' % node.nodeName)
            # Write out the attributes.
            attrs = node.attributes                            
            for attrName in attrs.keys():
                attrNode = attrs.get(attrName)
                attrValue = attrNode.nodeValue
                if Debug:
                    print('Attribute -- Name: %s  Value: %s\n' % \
                    (attrName, attrValue))
            # Walk over any text nodes in the current node.
            content = []                                      
            for child in node.childNodes:
                if child.nodeType == Node.TEXT_NODE:
                    content.append(child.nodeValue)
            if content:
                strContent = string.join(content)
                if Debug:
                    print('Content: "')
                    print(strContent)
                    print('"\n')
                
    def GetVmeTagArrayNode(self,inputVmeNode):
        self.Id = -2
        self.correctId = 1
        self.__GetVmeTagArrayNodeInternal(inputVmeNode)
        return self.__OutputTagArrayNode
    
    def __GetVmeTagArrayNodeInternal(self,inputVmeNode):
        #NodeName: Name
            #Content: "TagArray"
        attrs = inputVmeNode.attributes
        for attrName in attrs.keys():
            attrNode = attrs.get(attrName)
            attrValue = attrNode.nodeValue
            if (attrName  == "Id"):
                if (self.Id == -2):
                    self.Id = attrValue 
                    break
                if (attrValue != self.Id):
                    self.correctId = 0
                    break
                  
        for node in inputVmeNode.childNodes:
            #  search for a Node with name "Node"...0
            
            if node.nodeType == Node.ELEMENT_NODE:
                if node.nodeName == "Item":
                    # get node attributes
                    attrs = node.attributes                             
                    for attrName in attrs.keys():
                        attrNode = attrs.get(attrName)
                        attrValue = attrNode.nodeValue
                        if (attrValue  == "mafTagArray" and self.correctId == 1):
                            self.__OutputTagArrayNode = node
                            break
                self.__GetVmeTagArrayNodeInternal(node)
                
    def GetVmeTagItemNodeByName(self, inputVmeTagArrayNode, tagName):
        self.__GetVmeTagItemNodeByNameInternal(inputVmeTagArrayNode, tagName)
        return self.__OutputTagItemNode
        
    def __GetVmeTagItemNodeByNameInternal(self, inputVmeTagArrayNode, tagName):
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
                            #assert(isinstance(attrNode,Node))
                            self.__OutputTagItemNode = node
                        
    def GetVMEDataURLList(self, inputVme):
        contents = []
        self.__GetVMEDataURLListInternal(inputVme, contents)
        return contents
        
    def __GetVMEDataURLListInternal(self, inputVme, contents = []): 
        # <URL>msf_test_import_export_VME.1.vtk</URL>
        # get node attributes
        # Walk over any text nodes in the current node.
        
        #URL of the binary file are serialized in different part of the msf
        #so I have to manage 3 cases. In the future, we'll try to uniform
        #the URL position.
        isExternal = 0
        
        attrs = inputVme.attributes
        for attrName in attrs.keys():
            attrNode = attrs.get(attrName)
            attrValue = attrNode.nodeValue
            if (attrName  == "Type"):
              if (attrValue == "mafVMEExternalData"):
                  isExternal = 1
                  contents.append("") #so binary must be found
              else:                  
                 break

                
        for child in inputVme.childNodes:
            if child.nodeType == Node.ELEMENT_NODE:
              if Debug:
                  print child.nodeName
              
              if (child.nodeName == "TC" and self.extFound == 1 and len(self.extension) == 0):
                  self.extension = (child.childNodes[0].nodeValue)
                  self.extFound = 0
                  
                                    
              if (child.nodeName == "TC" and self.fileNameFound == 1 and len(self.extension) != 0):
                  fileName = (child.childNodes[0].nodeValue)
                  self.fileNameFound = 0
                  contents[0] = (fileName + '.' + self.extension)
                  return contents
              
              
              if (child.nodeName == "TItem"):
                   attrs = child.attributes                            
                   for attrName in attrs.keys():
                        attrNode = attrs.get(attrName)
                        attrValue = attrNode.nodeValue
                        if (attrName  == "Name"): #in .zvtk files
                               if (attrValue == "EXTDATA_EXTENSION"):
                                   self.extFound = 1
                                   break
                        if (attrName  == "Name"): #in .zvtk files
                              if (attrValue == "EXTDATA_FILENAME"):
                                   self.fileNameFound = 1
                                   break
                                             
                  
              if (child.nodeName == "Children"):
                  continue 
              if child.nodeName == "DataVector":
                   contents.append("") #so binary must be found
                   attrs = child.attributes                            
                   for attrName in attrs.keys():
                        attrNode = attrs.get(attrName)
                        attrValue = attrNode.nodeValue
                        if (attrName  == "ArchiveFileName"): #in .zvtk files
                            contents[0] = (attrValue)
                            return contents
                         
              if (child.nodeName == "URL"): #in .vtk files
                   contents[0] = (child.childNodes[0].nodeValue)
                   return contents            

              self.__GetVMEDataURLListInternal(child, contents)
        
    def GetTagNodeByTagName(self, inputVmeTagArrayNode, tagName):
        """Get tagNode given tag array node and tag name """
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
                             return node
        return None
    
    def SetTagNodeText(self, tagNode, stringValue):
        """Set text content for tag node of type:
        
         <TItem Mult="1" Name="tagName" Type="STR">
               <TItem>
                 <TC>This is my text content...</TC>
               </TItem>
         </TItem>
        
        which is a standard vme tag item
        node must exist already"""
        isinstance(tagNode, minidom.Node)
        
        try:
            textNode = tagNode.childNodes[0].childNodes[0].childNodes[0]
        except:
            try:
                textNode = tagNode.childNodes[1].childNodes[1].childNodes[0]
            except:
                 return
            
        isinstance(textNode,minidom.Node)
        textNode.data = stringValue

    def GetTagNodeText(self, tagNode):
        """Return text content for tag node of type
        
         <TItem Mult="1" Name="tagName" Type="STR">
               <TItem>
                 <TC>This is my text content...</TC>
               </TItem>
         </TItem>
        
        which is a standard vme tag item
        """
        isinstance(tagNode, minidom.Node)
        
        try:
            textNode = tagNode.childNodes[0].childNodes[0].childNodes[0]
        except:
            try:
                textNode = tagNode.childNodes[1].childNodes[1].childNodes[0]
            except:
                 return

        isinstance(textNode,minidom.Node)
        return textNode.data
        
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
            if Debug:
                print attrName
            if attrName  == "NumberOfTags":
               attrNode = attrs.get(attrName)
               oldTagsNumber = attrNode.nodeValue
               if Debug:
                   print oldTagsNumber
               attrNode.nodeValue = str(int(oldTagsNumber) - removedTagsNumber)
               if Debug:
                   print attrNode.nodeValue
               
    
    def AddTagsFromList(self, domDoc, inputVmeTagArrayNode, tagsToBeAdded):
        # change number of tags
        # <Item NumberOfTags="2" Type="mafTagArray">
        addedTagsNumber = 0
        for tagName in tagsToBeAdded:
            node = self.CreateSTRTagItem(tagName,"ANNOTATE ME!!! I`M A MANUAL TAG!!!", domDoc)
            # self.PrintNode(node, sys.stdout,0)
            inputVmeTagArrayNode.appendChild(node)
            addedTagsNumber += 1
        attrs = inputVmeTagArrayNode.attributes                             
        for attrName in attrs.keys():
            if Debug:               
                print attrName
            if attrName  == "NumberOfTags":
               attrNode = attrs.get(attrName)
               oldTagsNumber = attrNode.nodeValue
               if Debug:
                   print "old tags number was: " + oldTagsNumber
                   print str(addedTagsNumber)+ " tags have been added"
               attrNode.nodeValue = str(int(oldTagsNumber)  +  addedTagsNumber)
               if Debug:
                   print "new tags number is: " + attrNode.nodeValue  
                   print attrNode.nodeValue
    
    
    def AddTagsFromDictionary(self, domDoc, inputVmeTagArrayNode, dictionaryOfTagsToBeAdded):
        # change number of tags
        # <Item NumberOfTags="2" Type="mafTagArray">
        addedTagsNumber = 0

        for key in dictionaryOfTagsToBeAdded:
            value = (str(dictionaryOfTagsToBeAdded[key])).strip().replace("\"","")
            node = self.CreateSTRTagItem(key, value, domDoc)
            # self.PrintNode(node, sys.stdout,0)
            inputVmeTagArrayNode.appendChild(node)
            addedTagsNumber += 1
        attrs = inputVmeTagArrayNode.attributes    
                        
        for attrName in attrs.keys():
            if Debug:               
                print attrName
            if attrName  == "NumberOfTags":
               attrNode = attrs.get(attrName)
               oldTagsNumber = attrNode.nodeValue
               if Debug:
                   print oldTagsNumber
               attrNode.nodeValue = str(int(oldTagsNumber)  +  addedTagsNumber)
               if Debug:    
                   print attrNode.nodeValue

    
    def CreateSTRTagItem(self, tagName, tagText, domDoc):
         """create a STR TagItam node ie
         
         <TItem Mult="1" Name="Dicom_CT_model" Type="STR">
               <TItem>
                 <TC>This is my text content...</TC>
               </TItem>
         </TItem>
        
        and return reference to it"""
        
         # create the node        
         isinstance(domDoc, Document)
         
         newEl = domDoc.createElement("TItem")
             
         newEl.setAttribute("Mult", "1" )
         newEl.setAttribute("Name", tagName.strip() )
         newEl.setAttribute("Type", "STR" )
         
         newElChild = domDoc.createElement("TItem")
         newEl.appendChild(newElChild)
        
         newElTC = domDoc.createElement("TC")
         newElChild.appendChild(newElTC)
        
         TCChild = domDoc.createTextNode(tagText)
         # TCChild.data = "ANNOTATE ME!!!!!!"
         newElTC.appendChild(TCChild)
         
         return newEl
    
    def GetTagNames(self, tagArrayNode):
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
        #print "\nVME tagArray node tag names:\n"
        #for el in range(len(tagList)):
        #    print tagList[el]

        #print "Number of tags: " + str(len(tagList))

        return tagList
    
    def GetTagDictionary(self, tagArrayNode):
        """return tag names and their text content in a dictionary"""
        tagDictionary = {} 

        for node in tagArrayNode.childNodes:
        #  search for a Node with name "Node"...0
            if node.nodeType == Node.ELEMENT_NODE:
                if node.nodeName == "TItem":
                    
                    tagText = self.GetTagNodeText(node)
                    
                    # get node attributes
                    attrs = node.attributes                             
                    for attrName in attrs.keys():
                        attrNode = attrs.get(attrName)
                        attrValue = attrNode.nodeValue
                        if attrName  == "Name":
                            tagDictionary[attrValue] = tagText
        
        # print tagDictionary         
        #print "\nVME tagArray node tag names:\n"
        #for el in range(len(tagDictionary)):
        #    print tagDictionary[el]

        #print "Number of tags: " + str(len(tagDictionary))

        return tagDictionary

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
                    if Debug:
                        print('NodeName: %s\n' % node.nodeName)
                    # and an attribute "Id"...
                    attrs = node.attributes                            
                    for attrName in attrs.keys():
                        attrNode = attrs.get(attrName)
                        attrValue = attrNode.nodeValue
                        if attrName  == "Id": 
                            #if (eval(attrValue) == vmeId):
                            if (str(attrValue)== str(vmeId)): 
                                if Debug:
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
    
    def GetRootNode(self, vmeTreeRootNode):
        for node in vmeTreeRootNode.childNodes:
            #  search for a Node with name "Root"
            if node.nodeType == Node.ELEMENT_NODE:
                if node.nodeName == "Root":
                    self.__OutputVme = node
        return self.__OutputVme
    
    def GetMSFNode(self, parentNode):
        if (parentNode.tagName == "MSF"):
            self.__OutputVme = parentNode
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
                    if Debug:
                        print('NodeName: %s\n' % node.nodeName)
                    # and an attribute "Id"...
                    attrs = node.attributes                            
                    for attrName in attrs.keys():
                        attrNode = attrs.get(attrName)
                        attrValue = attrNode.nodeValue
                        if attrName  == "Id": 
                            if eval(attrValue) == vmeId: 
                                if Debug:
                                    print('Attribute -- Name: %s  Value: %s\n' % \
                                    (attrName, attrValue))
                                self.__OutputVme = node 
                                break
            else:
                self.__GetVmeNodeByIdInternal2(node, vmeId)
                
    def IsARoot(self, parentNode):
        """Return true if node is a root"""
        result = False
        if parentNode.nodeName == "Root":
            result = True
            return result
                
        return result
                
    def GetNodeByNodeName(self, parentNode, nodeName):
        """Return the first ELEMENT_NODE nodeType node found with name nodeName descending from parentNode"""
        self.__GetNodeByNodeNameInternal(parentNode, nodeName)
        return self.__OutputNode
    
    def __GetNodeByNodeNameInternal(self, parentNode, nodeName):
        for node in parentNode.childNodes:
            if node.nodeType == Node.ELEMENT_NODE:
                if node.nodeName == nodeName:
                    if Debug:
                        print('NodeName: %s\n' % node.nodeName)
                    self.__OutputNode = node
                    return
            self.__GetNodeByNodeNameInternal(node, nodeName)
    
    def PrintVmeNodes(self, parent, outFile):
        level = 0
        self.__PrintVmeNodesInternal(parent, outFile, level)
        
    def __PrintVmeNodesInternal(self,parent, outFile, level):                               
        for node in parent.childNodes:
            if node.nodeType == Node.ELEMENT_NODE:
                # Write out the element name.            
                if node.nodeName == "Node":
                    self.__printLevel(outFile, level)
                    self.PrintNode(node, outFile, level)
                    # Write out the attributes.
            self.__PrintVmeNodesInternal(node, outFile, level+1)
    
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

