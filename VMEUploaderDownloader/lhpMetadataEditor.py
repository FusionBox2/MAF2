#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

from Debug import Debug
from xml.dom.minidom import Childless
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
        
        self.CacheList = []
        self.ExceptionList = []
        
        # load msf infos
        curDir = os.getcwd()        
        print " current directory is: " + curDir
        
        self.inputCSVFileName = os.getcwd() + r'\\' + str(arg[0]) # to be used to save on exit    
        print self.inputCSVFileName
        assert(os.path.exists(self.inputCSVFileName))
        
        # read this from the csv
        autoTagsReader = csv.reader(open(self.inputCSVFileName, "r"))
        
        self.vmeCSVUnhandledPlusManualTagsFile = {}
         
        try:
            for row in autoTagsReader:
                if Debug:          
                             
                    print "row: " +  str(row)
                self.vmeCSVUnhandledPlusManualTagsFile[row[0].strip()] = str(row[1].strip())
                
        except csv.Error, e:
#            sys.exit('file %s, line %d: %s' % (filename, autoTagsReader.line_num, e))
             pass
        if Debug:          
            print self.vmeCSVUnhandledPlusManualTagsFile

        for item in self.vmeCSVUnhandledPlusManualTagsFile:
            print item[0]+ " " + item[1]
            
        tagsList = sorted(self.vmeCSVUnhandledPlusManualTagsFile.keys())
        print tagsList

       
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
        fldridx     = il.Add(wx.ArtProvider_GetBitmap(wx.ART_FOLDER,      wx.ART_OTHER, isz))
        fldropenidx = il.Add(wx.ArtProvider_GetBitmap(wx.ART_FILE_OPEN,   wx.ART_OTHER, isz))
        fileidx     = il.Add(wx.ArtProvider_GetBitmap(wx.ART_NORMAL_FILE, wx.ART_OTHER, isz))
        smileidx    = il.Add(images.getSmilesBitmap())

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
            
        self.tree.SetColumnWidth(9, 400)      
  
        self.root = self.tree.AddRoot("LHDL Master Dictionary")
        self.tree.SetItemImage(self.root, fldridx, which = wx.TreeItemIcon_Normal)
        self.tree.SetItemImage(self.root, fldropenidx, which = wx.TreeItemIcon_Expanded)
    
        self.FillGuiTree(pi.DictionaryDOMDocument.firstChild,  self.root)
        self.tree.ExpandAll(self.root)

        self.tree.GetMainWindow().Bind(wx.EVT_RIGHT_UP, self.OnRightUp)
        self.tree.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.OnActivate)
    
        print "self.ExceptionList:"
        for item in self.ExceptionList:
            print item
        
        
    def SaveOnExit(self):
        for i in self.CacheList:
            print i[0] + " " + i[1]
        
        file = open(self.inputCSVFileName, 'w')
        assert(os.path.exists(self.inputCSVFileName))
        for row in self.CacheList:
            toWrite = '"' + row[0] + '"' + " , " + row[1] + "\n"
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
        nodeName = pi.GetNodeName(xmlDictNode)
             
        # get tag array corresponding entry 
        msfTagName =  pi.GetVMETagArrayTagNameFromNode(xmlDictNode)  
        print msfTagName
        # print self.vmeCSVUnhandledPlusManualTagsFile
        msfTagName = msfTagName.strip()
        print msfTagName
        
        msfTagValue = ""
        
        if self.vmeCSVUnhandledPlusManualTagsFile.has_key(msfTagName):
            msfTagValue = self.vmeCSVUnhandledPlusManualTagsFile[msfTagName]
        else:
            msfTagValue = "ANNOTATE ME!"
  
        print msfTagValue
        
        # build {long_tag , value dictionary}
        
         
        attrDict =  pi.GetAttributesDictionary(xmlDictNode)
        
        # names from ValueType to Notes
        columnNames = pi.DictionaryColumnLabels.irange(pi.DictionaryColumnLabels.ValueType \
        , pi.DictionaryColumnLabels.Notes)
        
        # append item to tree 
        child = self.tree.AppendItem(guiTreeParent, str(nodeName))  
        
        self.CacheList.append([msfTagName, msfTagValue])
     
        assert isinstance(msfTagValue ,str )
        msfTagValue = msfTagValue.replace('"', '')
        # set the value column
        self.tree.SetItemText(child, msfTagValue, 1)
        
        # set item nodeName        
        if attrDict != {}:
            for guiColumnId in range(2,10):        
                # print "guiColumnId: " + str(guiColumnId)
                # print self.tree.GetColumnText(guiColumnId)
                # get the column string# 
                columnName = pi.DictionaryColumnLabels[guiColumnId + 7]
                # print "col name: " + str(columnName)
                
                value = attrDict[str(columnName)]
                # print "value: " + str(value)#               
        
                pass
        # set item text
                self.tree.SetItemText(child, value, guiColumnId)
           
        if xmlDictNode.childNodes:
            for node in xmlDictNode.childNodes:
                self.__FillGuiTreeInternal(node, child)

    
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
