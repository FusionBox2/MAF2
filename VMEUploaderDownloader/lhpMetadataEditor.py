#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni
#-----------------------------------------------------------------------------

from Debug import Debug
import os
import csv
import  wx
import  wx.gizmos   as  gizmos
import time
import lhpXMLDictionaryParser
from xml.dom import minidom
import lhpMetadataEditorTextEntryDialog#----------------------------------------------------------------------

class MetadataEditorPanel(wx.Panel):
    def __init__(self, parent, log, arg):
        
        self.TagsToBeSavedList = []
        self.TagsThatCanBeEditedDictionary = {}        
        self.InTreeButNotInUnhandledPlusManual = []
        self.TagsToBeSkippedFromTreeRendering = ["L0000"]
        self.folderToBeSkippedFromTreeRendering = ["L0000_resource_service"]

        curDir = os.getcwd()        
        if Debug:
            print " current directory is: " + curDir

                
        self.inputUnhandledPlusManualCSVFileName = str(arg[2]) + str(arg[0]) 
        
        if Debug:
            print self.inputUnhandledPlusManualCSVFileName
        assert(os.path.exists(self.inputUnhandledPlusManualCSVFileName))
        
        if len(arg) ==  3:
            self.OutputCSVFileName  = self.inputUnhandledPlusManualCSVFileName
        elif len(arg) ==  4:
            self.OutputCSVFileName = os.getcwd() + r'\\' + str(arg[2])
            
        
        # read unhandledPlusManual tags ie manual tags plus tags the factory cannot fill
        # so the user is required to do so (for factory methods not yet implemented)
        unhandledPlusManualTagsReader = csv.reader(open(self.inputUnhandledPlusManualCSVFileName, "r"))
        
        self.unhandledPlusManualDict = {}
         
        try:
            for row in unhandledPlusManualTagsReader:
                self.unhandledPlusManualDict[row[0].strip()] = str(row[1].strip())
                
        except csv.Error:
#            sys.exit('file %s, line %d: %s' % (filename, unhandledPlusManualTagsReader.line_num, e))
             pass
                   
        
        # read handledAutoTagsList.csv ie tags filled from the factory to be displayed read only        
        self.factoryFilledTagsCSVFileName = os.getcwd() + r'\\' + "handledAutoTagsList.csv"     
        if Debug:
            print self.factoryFilledTagsCSVFileName
        assert(os.path.exists(self.factoryFilledTagsCSVFileName))
       
        factoryFilledTagsReader = csv.reader(open(self.factoryFilledTagsCSVFileName, "r"))
        
        self.factoryFilledTagsDict = {}
         
        try:
            for row in factoryFilledTagsReader:
                self.factoryFilledTagsDict[row[0].strip()] = str(row[1].strip())
                
        except csv.Error:
#            sys.exit('file %s, line %d: %s' % (filename, factoryFilledTagsReader.line_num, e))
             pass
                   
        
        # load xml dictionary (already assembled if composed)
        self.inputXMLDictionaryFileName = os.getcwd() + r'\\' + str(arg[1]) # to be used to save on exit    
 
        time.sleep(5)
        assert(os.path.exists(self.inputXMLDictionaryFileName))
        
        self.lhpXMLDictionaryParserInstance = lhpXMLDictionaryParser.lhpXMLDictionaryParser()
        
        pi = self.lhpXMLDictionaryParserInstance
        pi.LoadXMLDictionary(self.inputXMLDictionaryFileName)
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
        self.folderImageID     = il.Add(wx.ArtProvider_GetBitmap(wx.ART_FOLDER,      wx.ART_OTHER, isz))
        self.fileOpenImageID = il.Add(wx.ArtProvider_GetBitmap(wx.ART_FILE_OPEN,   wx.ART_OTHER, isz))
        self.manualTagImageID     = il.Add(wx.ArtProvider_GetBitmap(wx.ART_NORMAL_FILE, wx.ART_OTHER, isz))
        # self.factoryTagImageID    = il.Add(images.getSmilesBitmap())
        self.factoryTagImageID = il.Add(wx.ArtProvider_GetBitmap(wx.ART_EXECUTABLE_FILE, wx.ART_OTHER, isz))
        

        self.tree.SetImageList(il)
        self.il = il

#        'NumTag', 'L1', 'L2', 'L3', 'L4', 'L5', 'L6', 'L7', 'L8', \
#        'ValueType', 'Multiplicity', 'ValueList', 'Editable', 'Searchable', \
#        'DefaultValue', 'Expert', 'Notes'])
#        
        
        # create a column for each field
        self.tree.AddColumn("Tags")
        self.tree.AddColumn("Value")
        self.tree.AddColumn("MSF Tag Name (Debug)")
        self.tree.ShowColumn(2, False)
        interval = pi.DictionaryColumnLabels.irange\
        (pi.DictionaryColumnLabels.ValueType \
        , pi.DictionaryColumnLabels.Notes)
        
        for columnName in interval:     
            self.tree.AddColumn(str(columnName))
                    
        self.tree.SetMainColumn(0) # the one with the tree in it...
        self.tree.SetColumnWidth(0, 250)

        for i in range(1,10):
            self.tree.SetColumnWidth(i, 60)
            
        self.tree.SetColumnWidth(1, 150)
        self.tree.SetColumnWidth(2, 150)
        self.tree.SetColumnWidth(10, 400)      
        
        # self.tree.SelectItem()
        # self.tree.EnsureVisible()
        
        self.root = self.tree.AddRoot("LHDL Master Dictionary")
        self.tree.SetItemImage(self.root, self.folderImageID)
        
        self.FillGuiTree(pi.DictionaryDOMDocument.firstChild,  self.root)
        self.tree.ExpandAll(self.root)

        self.tree.GetMainWindow().Bind(wx.EVT_RIGHT_UP, self.OnRightUp)
        self.tree.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.OnActivate)
    
        if Debug:
            print """self.InTreeButNotInUnhandledPlusManual: 
tags in tree but not in UnhandledPlusManual: should be removed from the factory"""
        
        for item in self.InTreeButNotInUnhandledPlusManual:
            if Debug:
                print item
        
        
    def SaveOnExit(self):
        
        childId = None
        listValuesFromTree = []
        for tag in self.TagsToBeSavedList:
    
            childId = tag[2]
            listValuesFromTree.append(self.tree.GetItemText(childId,1))
                
        tagsToBeSavedSet = set()
        for item in self.TagsToBeSavedList:
            tagsToBeSavedSet.add(item[0])
        
        unhPlusManualSet = set()
        for item in self.unhandledPlusManualDict.keys():
            unhPlusManualSet.add(item)
                            
        setDiff = unhPlusManualSet.difference(tagsToBeSavedSet)
        setDiff2 = tagsToBeSavedSet.difference(unhPlusManualSet)
        
        if Debug:
            print setDiff
            print setDiff2
        
        toBeSaved = len(self.TagsToBeSavedList) 
        unhandledPlusMan = len(self.unhandledPlusManualDict.keys())
        if Debug:
            print "unhandledPlusManual list lenght: " + str(unhandledPlusMan)
            print "toBeSaved dict size: "  + str(toBeSaved)
        
            print "unhandledPlusManualSet size: " + str(len(unhPlusManualSet))
            print "toBeSavedSet size: "  + str(len(tagsToBeSavedSet))
        
#        assert(toBeSaved == unhandledPlusMan)
        
        file = open(self.OutputCSVFileName, 'w')
        assert(os.path.exists(self.inputUnhandledPlusManualCSVFileName))
        
        for index in range(len(self.TagsToBeSavedList)):
        
            toWrite = '"' + self.TagsToBeSavedList[index][0] + '"' + " , " + '"' + listValuesFromTree[index] + '"' + "\n" 
            file.write(str(toWrite))
        
        file.close()
            
    def OnActivate(self, evt):
        self.log.write('OnActivate: %s' % self.tree.GetItemText(evt.GetItem()))

        tagName = self.tree.GetItemText(evt.GetItem())
        selectedTag = self.tree.GetItemText(evt.GetItem(), 2)
        selectedTagValue = self.tree.GetItemText(evt.GetItem(), 1)
        if Debug:
            print selectedTag
        if self.TagsThatCanBeEditedDictionary.has_key(selectedTag):
        
            dlg = lhpMetadataEditorTextEntryDialog.Dialog(self, -1, tagName, size=(350, 200),
                             #style=wx.CAPTION | wx.SYSTEM_MENU | wx.THICK_FRAME,
                             style=wx.DEFAULT_DIALOG_STYLE, selectedTag = selectedTag,
                             selectedText = selectedTagValue# & ~wx.CLOSE_BOX,
                             )
            
            val = dlg.ShowModal()
            
            if val == wx.ID_OK:
               self.log.WriteText("You pressed OK\n")
               if dlg.TextControl: 
                    if Debug:
                        print "assigning: " + dlg.TextControl.GetValue()
                    self.tree.SetItemText(evt.GetItem(),dlg.TextControl.GetValue(), 1)
            
            else:
               self.log.WriteText("You pressed Cancel\n")
               if dlg.TextControl: 
                    if Debug:
                        print "discarding: " + dlg.TextControl.GetValue()
    
            dlg.Destroy()
       
        elif self.tree.GetItemImage(evt.GetItem()) == self.factoryTagImageID:
            
            dlg = wx.MessageDialog(self,  'You are not allowed to edit me, I`m factory filled :D',
                               'Warning',
                               wx.OK | wx.ICON_WARNING
                               #wx.YES_NO | wx.NO_DEFAULT | wx.CANCEL | wx.ICON_INFORMATION
                               )
            dlg.ShowModal()

        else:
            
            dlg = wx.MessageDialog(self,  'You are not allowed to edit this tag',
                               'Warning',
                               wx.OK | wx.ICON_WARNING
                               #wx.YES_NO | wx.NO_DEFAULT | wx.CANCEL | wx.ICON_INFORMATION
                               )
            dlg.ShowModal()

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
        
        tagsSkipList = self.TagsToBeSkippedFromTreeRendering
        
        if  len(tagsSkipList) != 0 and tagsSkipList.count(msfTagName) == 1:
            tagsSkipList.remove(msfTagName)
            xmlDictNode = xmlDictNode.childNodes[0]
            msfTagName =  pi.GetVMETagArrayTagNameFromNode(xmlDictNode)  
            guiTreeNodeName = pi.GetNodeName(xmlDictNode)
            
        
        foldersSkipList = self.folderToBeSkippedFromTreeRendering
        
        if  len(foldersSkipList) != 0 and foldersSkipList.count(msfTagName) == 1:
            print  " removing: " + msfTagName
            foldersSkipList.remove(msfTagName)
            xmlDictNode = xmlDictNode.nextSibling
            msfTagName =  pi.GetVMETagArrayTagNameFromNode(xmlDictNode)  
            guiTreeNodeName = pi.GetNodeName(xmlDictNode)
            
            return
        
        msfTagValue = ""
        
        
        assert isinstance(xmlDictNode , minidom.Node)
       
        img = None
        # render tag node as folder
        if pi.GetValueType(xmlDictNode) == "tag": 
            msfTagValue = " "
            img = self.folderImageID
        
        # manual nodes
        elif self.unhandledPlusManualDict.has_key(msfTagName):
            msfTagValue = self.unhandledPlusManualDict[msfTagName]
            img = self.manualTagImageID
            self.TagsThatCanBeEditedDictionary[msfTagName] = ""
            
            # apply default value for boolean nodes
            if pi.GetValueType(xmlDictNode) == "bool":

               if msfTagValue == "enter a value":
                  msfTagValue = str(pi.GetDefaultValue(xmlDictNode))
#       
        # factory filed node
        elif self.factoryFilledTagsDict.has_key(msfTagName):
            msfTagValue = self.factoryFilledTagsDict[msfTagName]
            img = self.factoryTagImageID
        else:
            msfTagValue = str(msfTagName) + " NOT FOUND IN UNHPLUSMAN AND FACTORY FILLED!!!!!!! \
            CHECK FOR THIS STRANGE BEHAVIOR!!!!!"
            return
        
                  
        attrDict =  pi.GetAttributesDictionary(xmlDictNode)
        
        columnNames = pi.DictionaryColumnLabels.irange(pi.DictionaryColumnLabels.ValueType \
        , pi.DictionaryColumnLabels.Notes)
         
        childId = self.tree.AppendItem(guiTreeParent, str(guiTreeNodeName),img)  
        
        if self.unhandledPlusManualDict.has_key(msfTagName):               
            self.TagsToBeSavedList.append([msfTagName, msfTagValue, childId])
        
        elif self.factoryFilledTagsDict.has_key(msfTagName):
            if Debug:
                print "I`m " + str(msfTagName) + " The factory was so kind to fill me :D!"
        else:
           
            # not in unhandledPlusManual
            self.InTreeButNotInUnhandledPlusManual.append(msfTagName)
            msfTagValue = "EXCEPTION: Remove this tag filling from the factory!"
           
        assert isinstance(msfTagValue ,str )
        msfTagValue = msfTagValue.replace('"', '')
        # set the value column
        self.tree.SetItemText(childId, msfTagValue, 1)
        self.tree.SetItemText(childId, msfTagName, 2)
        
        # set item guiTreeNodeName        
        if attrDict != {}:
            for guiColumnId in range(2,10):      
                  
                columnName = pi.DictionaryColumnLabels[guiColumnId + 7]      
                value = attrDict[str(columnName)]
                self.tree.SetItemText(childId, value, guiColumnId  + 1)
           
        if xmlDictNode.childNodes:
            for node in xmlDictNode.childNodes:
                self.__FillGuiTreeInternal(node, childId)

    

def execute(frame, nb, log,arg):
    win = MetadataEditorPanel(nb, log,arg)
    return win



if __name__ == '__main__':
    # USAGE lhpMetadataEditor argv0 argv1 argv2 arg3
    # arg[0]: input csv file name
    # arg[1]: input xml dictionary file name
    # arg[2]: csv file path (Cache Directory)
    # arg[3]: if present output generated csv  will be written to that file otherwise
    # input csv will be used 
    
    # a file named handledAutoTagsList.csv must be present in the same dir to gather auto filled tags
    # (refactor note: this file name could be passed as parameters)
   
    import sys,os
    import runLHPMetadataEditor
          
    runLHPMetadataEditor.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

