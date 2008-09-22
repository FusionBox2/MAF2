#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

from xml.dom.minidom import Childless
import os
import  wx
import  wx.gizmos   as  gizmos
import  images
import lhpXMLDictionaryParser
import msfParser
from xml.dom import minidom
#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        
        # load msf infos
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
        
        self.inFileName = curDir + r'\metadataEditorTestData\surfaceWithMasterDictionaryTags\surfaceWithMasterDictionaryTags.msf'
        assert(os.path.exists(self.inFileName))
        
        self.msfParserInstance = msfParser.msfParser()
        self.doc = minidom.parse(self.inFileName)
        self.rootNode = self.doc.documentElement
        
        p = self.msfParserInstance
        
        # get a test vme 
        vme = p.GetVmeNodeById(self.rootNode, 18)
        tagArrayNode = p.GetVmeTagArrayNode(vme)
        assert(tagArrayNode != None)
        
        self.msfTagsDictionary = p.GetTagDictionary(tagArrayNode)
        print self.msfTagsDictionary
        
        for key in self.msfTagsDictionary.keys():
            print key, '\t', self.msfTagsDictionary[key]
    
        # load xml dictionary
        xmlDict = r'.\metadataEditorTestData\xmlDictionaries\LHDL_dictionary.xml'

        self.lhpXMLDictionaryParserInstance = lhpXMLDictionaryParser.lhpXMLDictionaryParser()
        
        pi = self.lhpXMLDictionaryParserInstance
        pi.LoadXMLDictionary(xmlDict)
        pi.PrintXMLDictionary()
        
        self.log = log
        wx.Panel.__init__(self, parent, -1)
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
        self.tree.SetItemText(self.root, "col 1 root", 1)
        self.tree.SetItemText(self.root, "col 2 root", 2)
        self.tree.SetItemImage(self.root, fldridx, which = wx.TreeItemIcon_Normal)
        self.tree.SetItemImage(self.root, fldropenidx, which = wx.TreeItemIcon_Expanded)
        
    
        self.FillGuiTree(pi.DictionaryDOMDocument.firstChild,  self.root)
        
#        for x in range(15):
#            txt = "Item %d" % x
#            child = self.tree.AppendItem(self.root, txt)
#            self.tree.SetItemText(child, txt + "(c1)", 1)
#            self.tree.SetItemText(child, txt + "(c2)", 2)
#            self.tree.SetItemImage(child, fldridx, which = wx.TreeItemIcon_Normal)
#            self.tree.SetItemImage(child, fldropenidx, which = wx.TreeItemIcon_Expanded)
#
#            for y in range(5):
#                txt = "item %d-%s" % (x, chr(ord("a")+y))
#                last = self.tree.AppendItem(child, txt)
#                self.tree.SetItemText(last, txt + "(c1)", 1)
#                self.tree.SetItemText(last, txt + "(c2)", 2)
#                self.tree.SetItemImage(last, fldridx, which = wx.TreeItemIcon_Normal)
#                self.tree.SetItemImage(last, fldropenidx, which = wx.TreeItemIcon_Expanded)
#
#                for z in range(5):
#                    txt = "item %d-%s-%d" % (x, chr(ord("a")+y), z)
#                    item = self.tree.AppendItem(last,  txt)
#                    self.tree.SetItemText(item, txt + "(c1)", 1)
#                    self.tree.SetItemText(item, txt + "(c2)", 2)
#                    self.tree.SetItemImage(item, fileidx, which = wx.TreeItemIcon_Normal)
#                    self.tree.SetItemImage(item, smileidx, which = wx.TreeItemIcon_Selected)


        self.tree.ExpandAll(self.root)

        self.tree.GetMainWindow().Bind(wx.EVT_RIGHT_UP, self.OnRightUp)
        self.tree.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.OnActivate)


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
        msfTagValue = self.msfTagsDictionary[msfTagName] 
        print msfTagValue
        
        attrDict =  pi.GetAttributesDictionary(xmlDictNode)
        
        # names from ValueType to Notes
        columnNames = pi.DictionaryColumnLabels.irange(pi.DictionaryColumnLabels.ValueType \
        , pi.DictionaryColumnLabels.Notes)
   
        
        # append item to tree 
        child = self.tree.AppendItem(guiTreeParent, str(nodeName))  
        
        # set item nodeName        
        if attrDict != {}:
            for guiColumnId in range(2,10):        
                print "guiColumnId: " + str(guiColumnId)
                print self.tree.GetColumnText(guiColumnId)
                # get the column string# 
                columnName = pi.DictionaryColumnLabels[guiColumnId + 7]
                print "col name: " + str(columnName)
                
                value = attrDict[str(columnName)]
                print "value: " + str(value)#               
        
                pass
        # set item text
                self.tree.SetItemText(child, value, guiColumnId)
           
        if xmlDictNode.childNodes:
            for node in xmlDictNode.childNodes:
                self.__FillGuiTreeInternal(node, child)

    
#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>TreeListCtrl</center></h2>

The TreeListCtrl is essentially a wx.TreeCtrl with extra columns,
such that the look is similar to a wx.ListCtrl.

</body></html>
"""


if __name__ == '__main__':
    #raw_input("Press enter...")
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

