#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni  <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

from Debug import Debug
import csv, sys
import unittest
import Enum
import dictionaryCSV2XML
from xml.dom.minidom import Document

class dictionaryCSV2XMLTest(unittest.TestCase):
    """Tests and experiments on csv lhdl dictionary XML transformation"""
    
    def testValidationMasterDictionary(self):
         """main test drive for dictionaryCSV2XML: parse csv dictionary and create xml output"""
         
         # Application Experts note
         # your CSV to validate here! ( replace sample data file name )
         csvDict = r'.\csv2XMLTestData\LHDL_dictionary.csv'
         
         # Application Experts note
         # this is the output to open in Firefox to check it`s valid XML ( replace output sample data )   
         xmlOutput = r'.\csv2XMLTestData\LHDL_dictionary.xml'

         dictionaryCSV2XML.run(csvDict,xmlOutput)
         
    
    def testValidationMotionAnalysisDictionary(self):
         
         # Application Experts note
         # your CSV to validate here! ( replace sample data file name )
         csvDict = r'.\csv2XMLTestData\MotionAnalysis.csv'
         
         # Application Experts note
         # this is the output to open in Firefox to check it`s valid XML ( replace output sample data )   
         xmlOutput = r'.\csv2XMLTestData\MotionAnalysis.xml'

         dictionaryCSV2XML.run(csvDict,xmlOutput)
    
    
    def testValidationDicomDictionary(self):
         
         # Application Experts note
         # your CSV to validate here! ( replace sample data file name )
         csvDict = r'.\csv2XMLTestData\DicomSource.csv'
         
         # Application Experts note
         # this is the output to open in Firefox to check it`s valid XML ( replace output sample data )   
         xmlOutput = r'.\csv2XMLTestData\DicomSource.xml'

         dictionaryCSV2XML.run(csvDict,xmlOutput)
         
######################################################################################################## 
        
        
    # def testTagNameFromNode(self):
        
    # open XML dictionary
    # TagNameFromNode 
    # NodeFromTagName
    # Print Tag Names
         
    # more experiments here: add "t" before est to enable code execution in test suite...
    def estReadingHierarchy(self):
        
        columnLabels = Enum.Enum(['#tag', 'L1', 'L2', 'L3', 'L4', 'L5', 'L6', 'L7', 'L8',\
                             'ValueType', 'Multiplicity', 'ValueList', 'Editable', 'Searchable',\
                             'DefaultValue', 'Expert', 'Notes'])

        attributesRange = columnLabels.irange(columnLabels.ValueType , columnLabels.Notes)
        LNRange = columnLabels.irange(columnLabels.L1 , columnLabels.L8)
        
        filename = r'.\csv2XMLTestData\LHDL_Resources_Taxonomy_v7c.csv'

        reader = csv.reader(open(filename, "rb"))       
        
        # skip the first line
        reader.next()
              
        lineNum = 0
        
        try:
            for row in reader:
                                   
                 print row[0:columnLabels.ValueType.asInt] 
                 
                 #for i in LNRange:
                     #if row[i.asInt]:
                         #print i.asInt
                         #print row[i.asInt]

        except csv.Error, e:
            sys.exit('file %s, line %d: %s' % (filename, reader.line_num, e))

     
    def estReadingExperts(self):
        
        columnLabels = Enum.Enum(['#tag', 'L1', 'L2', 'L3', 'L4', 'L5', 'L6', 'L7', 'L8',\
                             'ValueType', 'Multiplicity', 'ValueList', 'Editable', 'Searchable',\
                             'DefaultValue', 'Expert', 'Notes'])

        filename = r'.\csv2XMLTestData\LHDL_Resources_Taxonomy_v7c.csv'

        reader = csv.reader(open(filename, "rb"))
        
        try:
            for row in reader:
                print row[columnLabels.Expert.asInt]
                 # print row
        except csv.Error, e:
            sys.exit('file %s, line %d: %s' % (filename, reader.line_num, e))

    def estWriteXML(self):
        
        # Create the minidom document
        doc = Document()
        
        # Create the <wml> base element
        wml = doc.createElement("wml")
        doc.appendChild(wml)
        
        # Create the main <card> element
        maincard = doc.createElement("card")
        maincard.setAttribute("id", "main")
        maincard.setAttr1ibute("mouse", "main")
        wml.appendChild(maincard)
        
        # Create a <p> element
        paragraph1 = doc.createElement("p")
        maincard.appendChild(paragraph1)
        
        # Give the <p> elemenet some text
        ptext = doc.createTextNode("This is a test!")
        paragraph1.appendChild(ptext)
        
        # Print our newly created XML
        print doc.toprettyxml(indent="  ")  
    
    def estCreteElement(self): 
        # for every row create a dom node
        
        columnLabels = Enum.Enum(['#tag', 'L1', 'L2', 'L3', 'L4', 'L5', 'L6', 'L7', 'L8',\
                             'ValueType', 'Multiplicity', 'ValueList', 'Editable', 'Searchable',\
                             'DefaultValue', 'Expert', 'Notes'])

        attributesRange = columnLabels.irange(columnLabels.ValueType , columnLabels.Notes)
        
        filename = r'.\csv2XMLTestData\LHDL_Resources_Taxonomy_v7c.csv'

        reader = csv.reader(open(filename, "rb"))       

        reader.next()
        
        # Create the minidom document
        doc = Document() 
        
        root = doc.createElement("L0000")
        # root.createElement(0000 ie dictionary identifier here XXX?)
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
                 LNRange = columnLabels.irange(columnLabels.L1 , columnLabels.L8)

                 
                 # current row node hierarchy
                 for i in LNRange:
                     if row[i.asInt]: 
                         print "tag: "  + row[i.asInt] + " found in L" + str(i.asInt) + " column" 
                         
                         elementName = row[i.asInt]
                         
                         nodeLColumn = i.asInt
                         deltaCol =  nodeLColumn-previousNodeLColumn
                         # handle column difference
                         print "deltaCol: " + str(deltaCol) 
                         
                         if deltaCol ==  1:
                             parent = node
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
                             print "tmpparent: " + parent.tagName
                         
                         previousNodeLColumn = nodeLColumn
                 
                         # create element parent dict
                         
                 rowDict = {}    
        
                 print row 
                 
                 # dictionary starting from ValueType column
                 for i in columnLabels[columnLabels.ValueType.asInt:]:
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
            
        print doc.toprettyxml(indent="  ")  

         
    def CreateElement(self, domDoc, parent, elementName, attributesDict):
         """create a node, append it to a parent and return a reference to it"""
        
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
     
    def GetDictionaryId(self, dictionaryFileName):
        pass # return 0000
        
    def estGetDictionaryId(self):
        pass
    
    def CreateXMLDictionary(self, dictionaryFileName):
                
        columnLabels = Enum.Enum(['#tag', 'L1', 'L2', 'L3', 'L4', 'L5', 'L6', 'L7', 'L8',\
                             'ValueType', 'Multiplicity', 'ValueList', 'Editable', 'Searchable',\
                             'DefaultValue', 'Expert', 'Notes'])

        attributesRange = columnLabels.irange(columnLabels.ValueType , columnLabels.Notes)
        
        filename = r'.\csv2XMLTestData\LHDL_Resources_Taxonomy_v7c.csv'

        reader = csv.reader(open(filename, "rb"))       

        reader.next()
        
        # create dom document
                
        # append to its parent
        doc = Document() 
        
        root = doc.createElement("root")
        # root.createElement(0000 ie dictionary identifier here XXX?)
        doc.appendChild(root)
        
        lineNum = 0

        # create a dict for each line
        try:
            # read csv line
            # create a node     
            for row in reader:
                
                 # create the node
                 rowDict = {}    
        
                 print row 
                 
                 for i in columnLabels:
                     print i # ValueType
                     print i.asInt # 9
                     print row[i.asInt] # tag                    
                     
                     rowDict[i] = row[i.asInt]
                
                 print rowDict
# {(0:#tag): '0000, 0098', (1:L1): '', (2:L2): 'Status', (3:L3): '', (4:L4): '', (5:L5): '', (6:L6): '', (7:L7): '', (8:L8): '', (9:ValueType): 'list', (10:Multiplicity): '3', (11:ValueList): 'Private, Published, ReplacedBy', (12:Editable): 'y', (13:Searchable): 'n', (14:DefaultValue): '', (15:Expert): 'Viceconti', (16:Notes): 'Resource accessibility'}
                 
                 print rowDict.k0
                 eys()
# [(0:#tag), (1:L1), (2:L2), (3:L3), (4:L4), (5:L5), (6:L6), (7:L7), (8:L8), (9:ValueType), (10:Multiplicity), (11:ValueList), (12:Editable), (13:Searchable), (14:DefaultValue), (15:Expert), (16:Notes)]

                 print rowDict.values()
# ['0000, 0098', '', 'Status', '', '', '', '', '', '', 'list', '3', 'Private, Published, ReplacedBy', 'y', 'n', '', 'Viceconti', 'Resource accessibility']

                 self.CreateElement(doc, root, str(lineNum), rowDict)
                 
                 lineNum += 1
                 
        except csv.Error, e:
            sys.exit('file %s, line %d: %s' % (filename, reader.line_num, e))
            
        print doc.toprettyxml(indent="  ")  

        
        pass 
        
    def estCreateXMLDictionary(self):
        pass
    
    
if __name__ == '__main__':
    unittest.main()
