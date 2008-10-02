#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

from Debug import Debug
from xml.dom.minidom import Childless, Node
from test.test_new import argcount
import os
import csv
import  wx
import  wx.gizmos   as  gizmos
import  images
import lhpXMLDictionaryParser
import msfParser
from xml.dom import minidom
#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log, arg):
        
               # make an image list

        # tegs in iput vme csv        
        self.TagsToBeSaved = []
        
        # tags in tree but not in input vme csv 
        # for example this could be tags erroneusly filled by the factory
        self.InTreeButNotInUnhandledPlusManual = []
        
        # load msf infos
        curDir = os.getcwd()        
        print " current directory is: " + curDir
        
                
        self.inputCSVFileName = os.getcwd() + r'\\' + str(arg[0]) # to be used to save on exit    
        print self.inputCSVFileName
        assert(os.path.exists(self.inputCSVFileName))
        
        if len(arg) ==  2:
            self.OutputCSVFileName  = self.inputCSVFileName
        elif len(arg) ==  3:
            self.OutputCSVFileName = os.getcwd() + r'\\' + str(arg[2])
            
        
        # read this from the csv
        autoTagsReader = csv.reader(open(self.inputCSVFileName, "r"))
        
        self.unhandledPlusManualDict = {}
         
        try:
            for row in autoTagsReader:
                if Debug:          
                             
                    print "row: " +  str(row)
                self.unhandledPlusManualDict[row[0].strip()] = str(row[1].strip())
                
        except csv.Error, e:
#            sys.exit('file %s, line %d: %s' % (filename, autoTagsReader.line_num, e))
             pass
        if Debug:          
            print self.unhandledPlusManualDict

        for item in self.unhandledPlusManualDict:
            print item[0]+ " " + item[1]
            
        tagsList = sorted(self.unhandledPlusManualDict.keys())
        print tagsList

        file = open("InputTagsSorted", 'w')
        sorts = sorted(self.unhandledPlusManualDict.keys())
        assert(os.path.exists(self.inputCSVFileName))
        for key in sorts:
            toWrite = '"' + str(key) + '"' + " , " + '"' + str(self.unhandledPlusManualDict[key]) + '"' + "\n" 
            file.write(str(toWrite))
        
        file.close()
        
        # load xml dictionary (already assembled if composed)
        self.inputXMLDictionaryFileName = os.getcwd() + r'\\' + str(arg[1]) # to be used to save on exit    
        print self.inputXMLDictionaryFileName
        assert(os.path.exists(self.inputXMLDictionaryFileName))
        
        self.lhpXMLDictionaryParserInstance = lhpXMLDictionaryParser.lhpXMLDictionaryParser()
        
        pi = self.lhpXMLDictionaryParserInstance
        pi.LoadXMLDictionary(self.inputXMLDictionaryFileName)
        pi.PrintXMLDictionary()
        
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        
        # wx.Panel.SetSize(1024,768)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        
        
        self.tree = gizmos.TreeListCtrl(self, -1, style =
                                        wx.TR_DEFAULT_STYLE
                                        | wx.TR_HAS_BUTTONS
#                                       | wx.TR_TWIST_BUTTONS
#                                         | wx.TR_ROW_LINES
                                        | wx.TR_COLUMN_LINES
                                        #| wx.TR_NO_LINES 
                                        | wx.TR_FULL_ROW_HIGHLIGHT
                                   )

        isz = (16,16)
        il = wx.ImageList(isz[0], isz[1])
        self.folderImageID     = il.Add(wx.ArtProvider_GetBitmap(wx.ART_FOLDER,      wx.ART_OTHER, isz))
        self.fileOpenImageID = il.Add(wx.ArtProvider_GetBitmap(wx.ART_FILE_OPEN,   wx.ART_OTHER, isz))
        self.normalFileID     = il.Add(wx.ArtProvider_GetBitmap(wx.ART_NORMAL_FILE, wx.ART_OTHER, isz))
        self.smileID    = il.Add(images.getSmilesBitmap())

        self.tree.SetImageList(il)
        self.il = il

#        'NumTag', 'L1', 'L2', 'L3', 'L4', 'L5', 'L6', 'L7', 'L8', \
#        'ValueType', 'Multiplicity', 'ValueList', 'Editable', 'Searchable', \
#        'DefaultValue', 'Expert', 'Notes'])
#        
        
        # create a column for each field
        self.tree.AddColumn("Tags")
        self.tree.AddColumn("Value", edit = True)
        
        interval = pi.DictionaryColumnLabels.irange\
        (pi.DictionaryColumnLabels.ValueType \
        , pi.DictionaryColumnLabels.Notes)
        
        for columnName in interval:     
            print "column name: " + str(columnName)
            self.tree.AddColumn(str(columnName))
            
        
        
        # create some columns         
        self.tree.SetMainColumn(0) # the one with the tree in it...
        self.tree.SetColumnWidth(0, 200)

        for i in range(1,10):
            self.tree.SetColumnWidth(i, 80)
            
        self.tree.SetColumnWidth(1, 200)
        self.tree.SetColumnWidth(9, 400)      
  
        self.root = self.tree.AddRoot("LHDL Master Dictionary")
        self.tree.SetItemImage(self.root, self.folderImageID, which = wx.TreeItemIcon_Normal)
        self.tree.SetItemImage(self.root, self.fileOpenImageID, which = wx.TreeItemIcon_Expanded)
    
        self.FillGuiTree(pi.DictionaryDOMDocument.firstChild,  self.root)
        self.tree.ExpandAll(self.root)

        self.tree.GetMainWindow().Bind(wx.EVT_RIGHT_UP, self.OnRightUp)
        self.tree.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.OnActivate)
    
        print "self.InTreeButNotInUnhandledPlusManual: tags in tree but not in UnhandledPlusManual"
        for item in self.InTreeButNotInUnhandledPlusManual:
            print item
        
        
    def SaveOnExit(self):
        
        rootId = self.tree.GetRootItem()
        childId = None
        listValuesFromTree = []
        for tag in self.TagsToBeSaved:
    
            childId = tag[2]
            listValuesFromTree.append(self.tree.GetItemText(childId,1))
        
        print listValuesFromTree
        
        toBeSaved = len(self.TagsToBeSaved) 
        unhandledPlusMan = len(self.unhandledPlusManualDict)
        assert(toBeSaved == unhandledPlusMan)
        
        file = open(self.OutputCSVFileName, 'w')
        assert(os.path.exists(self.inputCSVFileName))
        
        for index in range(len(self.TagsToBeSaved)):
        
            toWrite = '"' + self.TagsToBeSaved[index][0] + '"' + " , " + '"' + listValuesFromTree[index] + '"' + "\n" 
            file.write(str(toWrite))
        
        file.close()
            
    def OnActivate(self, evt):
        self.log.write('OnActivate: %s' % self.tree.GetItemText(evt.GetItem()))
        

    def OnRightUp(self, evt):
        pos = evt.GetPosition()
        item, flags, col = self.tree.HitTest(pos)
        if item:
            self.log.write('Flags: %s, Col:%s, Text: %s' %
                           (flags, col, self.tree.GetItemText(item, col)))

    def OnSize(self, evt):
    
        self.tree.SetSize(self.GetSize())
   
    # navigate the dictionary and fill the self.tree       
       
    def FillGuiTree(self, xmlDictParent, guiParent):
        self.__FillGuiTreeInternal(xmlDictParent,guiParent)

    def __FillGuiTreeInternal(self,xmlDictNode, guiTreeParent):
        pi = self.lhpXMLDictionaryParserInstance  
        guiTreeNodeName = pi.GetNodeName(xmlDictNode)
             
        # get tag array corresponding entry 
        msfTagName =  pi.GetVMETagArrayTagNameFromNode(xmlDictNode)  
        
        msfTagName = msfTagName.strip()
        
        msfTagValue = ""
        
        assert isinstance(xmlDictNode , minidom.Node)

       
        img = None
        if xmlDictNode.hasChildNodes():
            # GROUPING NODE
            msfTagValue = " "
            img = self.folderImageID
        elif self.unhandledPlusManualDict.has_key(msfTagName):
            msfTagValue = self.unhandledPlusManualDict[msfTagName]
            img = self.normalFileID
        else:
            msfTagValue = "NOT FOUND IN UNHPLUSMAN"
            return
        print msfTagValue
        
        # build {long_tag , value dictionary}
        
         
        attrDict =  pi.GetAttributesDictionary(xmlDictNode)
        
        columnNames = pi.DictionaryColumnLabels.irange(pi.DictionaryColumnLabels.ValueType \
        , pi.DictionaryColumnLabels.Notes)
         
        childId = self.tree.AppendItem(guiTreeParent, str(guiTreeNodeName),img)  
        
        if self.unhandledPlusManualDict.has_key(msfTagName):               
            self.TagsToBeSaved.append([msfTagName, msfTagValue, childId])
              
        else:
            # not in unhandledPlusManual
            self.InTreeButNotInUnhandledPlusManual.append(msfTagName)
            msfTagValue = "EXCEPTION: Remove this tag filling from the factory!"
           
        assert isinstance(msfTagValue ,str )
        msfTagValue = msfTagValue.replace('"', '')
        # set the value column
        self.tree.SetItemText(childId, msfTagValue, 1)
        
        # set item guiTreeNodeName        
        if attrDict != {}:
            for guiColumnId in range(2,10):      
                  
                columnName = pi.DictionaryColumnLabels[guiColumnId + 7]      
                value = attrDict[str(columnName)]
                self.tree.SetItemText(childId, value, guiColumnId)
           
        if xmlDictNode.childNodes:
            for node in xmlDictNode.childNodes:
                self.__FillGuiTreeInternal(node, childId)

    
#----------------------------------------------------------------------

def runTest(frame, nb, log,arg):
    win = TestPanel(nb, log,arg)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>TreeListCtrl</center></h2>

The TreeListCtrl is essentially a wx.TreeCtrl with extra columns,
such that the look is similar to a wx.ListCtrl.

</body></html>
"""


if __name__ == '__main__':
    
    import sys,os
    import run
    # list: ['D:\\vapps_merge_target\\LHPBuilder_Parabuild_Binary\\VMEUploaderDownloader\\lhpMetadataEditor.py'
    print sys.argv[1]     # , 'Surface_Parametric_id18_tag.csv']
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
