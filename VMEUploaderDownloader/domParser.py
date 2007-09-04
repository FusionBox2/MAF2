# VMEUploaderDownloader domParser.py "D:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\msf_test_import_export_VME\msf_test_import_export_VME.msf"

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

class LHPBDictionary:
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
            
class domParser:
    """Facilities to handle MSF DOM Tree"""
    
    def __init__(self):                              
        self.OutputVme = None 
        self.OutputTagArrayNode = None
        self.OutputTagItemNode = None
    
    def PrintDOMTree(self,parent, outFile, level):  
        self.PrintNode(parent, outFile, level)
        if parent.childNodes:
            for node in parent.childNodes:
                self.PrintDOMTree(node, outFile, level+1)
    
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
                            self.OutputTagArrayNode = node
                self.GetVmeTagArrayNode(node)
                
    def GetVmeTagItemNodeByName(self, inputVmeTagArrayNode, tagName):
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
                            self.OutputTagItemNode = node
                        
        
    def RemoveTagsByList(self, inputVmeTagArrayNode, tagsToBeRemoved):
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
    
    def PrintTagNames(self, tagArrayNode):
        """Print the given tagArrayNode tags, also return the tagList"""
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
        print "tagArray node tag names:"
        for el in range(len(tagList)):
            print tagList[el]
        print "Number of tags: " + str(len(tagList))
        return tagList
    
    def GetVmeNodeById(self, vmeTreeRootNode, vmeId):
        """Get a vme given the vme tree root and its Id"""
        #NodeName: Node
                #Attribute -- Name: Type  Value: mafVMEVolumeGrayAnd
                
                #Attribute -- Name: Crypting  Value: 0
                #Attribute -- Name: Id  Value: 1
                #Attribute -- Name: Name  Value: test_volume
        #Content: "
        for node in vmeTreeRootNode.childNodes:
            #  search for a Node with name "Node"...0
            
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
                                self.OutputVme = node
            self.GetVmeNodeById(node, vmeId)
    
    def PrintNodeNames(self,parent, outFile, level):                               
        for node in parent.childNodes:
            if node.nodeType == Node.ELEMENT_NODE:
                # Write out the element name.            
                if node.nodeName == "Node":
                    __printLevel(outFile, level)
                    outFile.write('NodeName: %s\n' % node.nodeName)
                # Write out the attributes.
                self.PrintNodeNames(node, outFile, level+1)
    
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

