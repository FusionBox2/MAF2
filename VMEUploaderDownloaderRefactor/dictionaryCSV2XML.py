#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

from Debug import Debug
import csv, sys
import Enum
from xml.dom.minidom import Document
  
class dictionaryCSV2XML:    
    """convert open office exported csv LHDL dictionary to XML"""
    
    def __init__(self):                                
        
        self.DictionaryColumnLabels = Enum.Enum([\
            'NumTag', 'L1', 'L2', 'L3', 'L4', 'L5', 'L6', 'L7', 'L8',\
            'ValueType', 'Multiplicity', 'ValueList', 'Editable', 'Searchable',\
            'DefaultValue', 'Expert', 'Notes'])
        
        self.InputCSVDictionaryFileName = "UNDEFINED"
        self.OutputXMLDictionaryFileName = "outputXMLDictionary.xml"
   
    def GetDictionaryIdString(self):
        reader = csv.reader(open(self.InputCSVDictionaryFileName, "r"))       

        # skip labels
        reader.next()
        
        firstDataLine = reader.next()
        
        # 0000 must came from dictionary hierarchy but since
        # dictionary is work in progress for the moment I hardcode it here...
        
        # Get Dictionary ID, for example 0000 four master dictionary; I append an L character
        # in front of the ID since a number is not a valid XML tag so the ID becomes L0000
        
        dictTag = firstDataLine[self.DictionaryColumnLabels.NumTag.asInt]
        dictId = dictTag[0:4]
        
        idString = "L" + str(dictId)
        return idString

        
    def BuildXMLDictionary(self): 
        # for every row create a dom node
        attributesRange = self.DictionaryColumnLabels.irange(self.DictionaryColumnLabels.ValueType , self.DictionaryColumnLabels.Notes)
        
        idString = self.GetDictionaryIdString()
        
        reader = csv.reader(open(self.InputCSVDictionaryFileName, "r"))       

        reader.next()
        
        # Create the minidom document
        doc = Document() 
        root = doc.createElement(idString)
        doc.appendChild(root)
        
        lineNum = 0

        # root is the first parent
        parent = doc
        node = root
        
        nodeLColumn = 0
        previousNodeLColumn =  0
        
        # create a dict for each line
        try:
            for row in reader:
        
                 # search for the L tag name
                 LNRange = self.DictionaryColumnLabels.irange(self.DictionaryColumnLabels.L1 , self.DictionaryColumnLabels.L8)
           
                 # current row node hierarchy
                 for i in LNRange:
                     if row[i.asInt]: 
                         if Debug:
                             print "tag: "  + row[i.asInt] + " found in L" + str(i.asInt) + " column" 
                         
                         elementName = row[i.asInt]
                         
                         nodeLColumn = i.asInt
                         deltaCol =  nodeLColumn-previousNodeLColumn
                         # handle column difference
                         if Debug:
                             print "deltaCol: " + str(deltaCol) 
                         
                         if deltaCol ==  1:
                             parent = node
                             if Debug:
                                 print "parent: " + parent.tagName
                             
                         elif deltaCol <= -1:
                             absDC = abs(deltaCol)
                             
                             tmpParent = parent
                             tmpNode = node
                            
                             while absDC >= 0:
                                 tmpParent = tmpNode.parentNode
                                 tmpNode = tmpParent
                                 absDC -= 1 
                                 
                             parent = tmpParent
                             if Debug:
                                 print "tmpparent: " + parent.tagName
                         
                         previousNodeLColumn = nodeLColumn
                 
                         # create element parent dict
                         
                 rowDict = {}    
        
                 if Debug:
                     print row 
                 
                 # dictionary starting from ValueType column
                 for i in self.DictionaryColumnLabels[self.DictionaryColumnLabels.ValueType.asInt:]:
                     #print i # ValueType
                     #print i.asInt # 9
                     #print row[i.asInt] # tag                    
                     
                     rowDict[i] = row[i.asInt]
                
                 #print rowDict
## {(0:#tag): '0000, 0098', (1:L1): '', (2:L2): 'Status', (3:L3): '', (4:L4): '', (5:L5): '', (6:L6): '', (7:L7): '', (8:L8): '', (9:ValueType): 'list', (10:Multiplicity): '3', (11:ValueList): 'Private, Published, ReplacedBy', (12:Editable): 'y', (13:Searchable): 'n', (14:DefaultValue): '', (15:Expert): 'Viceconti', (16:Notes): 'Resource accessibility'}
                 
                 #print rowDict.keys()
## [(0:#tag), (1:L1), (2:L2), (3:L3), (4:L4), (5:L5), (6:L6), (7:L7), (8:L8), (9:ValueType), (10:Multiplicity), (11:ValueList), (12:Editable), (13:Searchable), (14:DefaultValue), (15:Expert), (16:Notes)]

                 #print rowDict.values()
## ['0000, 0098', '', 'Status', '', '', '', '', '', '', 'list', '3', 'Private, Published, ReplacedBy', 'y', 'n', '', 'Viceconti', 'Resource accessibility']

                 # create the node
                 node = self.CreateElement(doc, parent, elementName, rowDict)
                 
                 lineNum += 1
                 
        except csv.Error, e:
            sys.exit('file %s, line %d: %s' % (filename, reader.line_num, e))
            
        if Debug:
            print doc.toprettyxml(indent="  ")  
        
        outFileXML = open(self.OutputXMLDictionaryFileName, 'w')
        doc.writexml(outFileXML)

    def CreateElement(self, domDoc, parent, elementName, attributesDict):
         """create a node, append it to given parent and return reference to it"""
        
         # create the node        
         isinstance(domDoc, Document)
         newEl = domDoc.createElement(elementName)
    
         isinstance(attributesDict , dict)
         
         # set attributes from the dict
         dictKeys = attributesDict.keys()
         dictValues = attributesDict.values()
        
         for i in attributesDict:
             newEl.setAttribute(str(i), str(attributesDict[i]))
         
         # add to parent
         parent.appendChild(newEl)
        
         return newEl
     
def run(lhdlMasterDictionary, outputXMLDictionaryFileName):                      
    
    csvToXML = dictionaryCSV2XML()
    csvToXML.InputCSVDictionaryFileName = lhdlMasterDictionary
    csvToXML.OutputXMLDictionaryFileName = outputXMLDictionaryFileName
    csvToXML.BuildXMLDictionary()
    
def main():
    args = sys.argv[1:]
    if len(args) != 2:
        print 'usage: dictionaryCSV2XML lhdlMasterDictionary.csv outputXMLDictionaryFileName.xml'
        sys.exit(-1)
    run(args[0], args[1])

if __name__ == '__main__':
    main()

