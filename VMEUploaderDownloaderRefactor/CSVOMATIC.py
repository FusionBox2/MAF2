#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni  <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

# BEWARE: this is a temporary editor for metadata but it will be trashed 
# when we will have the real XML editor working

# this code is based on CVSOMATIC.py 
# http://developer.berlios.de/projects/csvomatic/

import wx
import wx.grid as grid
import string
import re
import os
import sys
import DSV
from os.path import getsize
import encodings.cp1252
#from CLR.System.Data import DataTable
#import CLR.System as System
#from CLR.System.Reflection import Assembly
#a = Assembly.LoadWithPartialName("DBDataClass")
# now we can import namespaces defined in that assembly
#from CLR.DBDataClass import MSDBase
#get rid of all warnings for Deprecation
# ICONBITMAP=os.getcwd()+"/DSV.ico"
DISPLAYLIMIT = 100 #limit the output of the MAIN WINDOW to 100 rows...in certain cases.
#event sluts

ID_OPEN = 101
ID_EXIT = 102
ID_ROW1 = 103
ID_ROW2 = 104
ID_SAVE = 105
ID_CANCEL = 106
ID_OK = 107
ID_OP1 = 108
ID_COLUMN1 = 109
ID_COLUMN2 = 110
ID_OP2 = 111
ID_COLUMN3 = 112
ID_OP3 = 113
ID_GRIDFIND1 = 114
ID_GRIDFIND2 = 115
ID_GRIDFIND3 = 116
ID_MATCH1 = 117
ID_ROW3 = 118
ID_GRIDFIND4 = 119
ID_GRIDFIND5 = 120
ID_OP4 = 121
ID_OP5 = 122
ID_OP6 = 123

#----EVIL GLOBALS! data is NEVER passed just a link to it's namespace
global foundrows
global foundcols
#If these are not boosted to global status not every window can access them
#which is a Bad Thing(tm) because I need to use them to delete/view matches.
#-----------------------------------------------------------------------
class DBQuery(wx.Dialog):
    def __init__ (self, parent, dialogmessage):
        wxDialog.__init__(self,parent,-1, dialogmessage,style = wx.DEFAULT_DIALOG_STYLE)
        self.RowCount = 0 #no rows!
        self.data = [] #empty dataset of course!
        self.headers = [] #no headers...yet
        self.parent = parent
        self.dbsetup = parent.dbsetup
        self.input1 = wx.TextCtrl(self, 1, style=wx.TE_WORDWRAP )
        self.input2 = wx.TextCtrl(self, 1, style=wx.TE_WORDWRAP )
        self.input3 = wx.TextCtrl(self, 1, style=wx.TE_WORDWRAP )
        self.input4 = wx.TextCtrl(self, 1, style=wx.TE_WORDWRAP )
        self.input5 = wx.TextCtrl(self, 1, style=wx.TE_WORDWRAP )
        self.PosLabel1 = wx.StaticText( self, -1, "Server")
        self.PosLabel2 = wx.StaticText( self, -1, "DBase")
        self.PosLabel3 = wx.StaticText( self, -1, "User")
        self.PosLabel4 = wx.StaticText( self, -1, "Password")
        self.PosLabel5 = wx.StaticText( self, -1, "Query")
        self.input1.SetValue(self.dbsetup[0])
        self.input2.SetValue(self.dbsetup[1])
        self.input3.SetValue(self.dbsetup[2])
        self.input4.SetValue(self.dbsetup[3])
        self.input5.SetValue(self.dbsetup[4])
        self.sizer2 = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer3 = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer4 = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer5 = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer6 = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer7 = wx.BoxSizer(wx.HORIZONTAL)
        self.buttons=[]
        self.buttons.append(wx.Button(self, ID_OK, "Ok"))
        self.buttons.append(wx.Button(self, ID_CANCEL, "Cancel"))
        self.sizer2.Add(self.buttons[0],1,wx.EXPAND)
        self.sizer2.Add(self.buttons[1],1,wx.EXPAND)
        # Use some sizers to see layout options
        self.sizer=wx.BoxSizer(wx.VERTICAL)
        self.sizer3.Add(self.input1,1,wx.EXPAND)
        self.sizer3.Add(self.PosLabel1,1,wx.EXPAND)
        self.sizer4.Add(self.input2,2,wx.EXPAND)
        self.sizer4.Add(self.PosLabel2,1,wx.EXPAND)
        self.sizer5.Add(self.input3,2,wx.EXPAND)
        self.sizer5.Add(self.PosLabel3,1,wx.EXPAND)
        self.sizer6.Add(self.input4,2,wx.EXPAND)
        self.sizer6.Add(self.PosLabel4,1,wx.EXPAND)
        self.sizer7.Add(self.input5,2,wx.EXPAND)
        self.sizer7.Add(self.PosLabel5,1,wx.EXPAND)
        self.sizer.Add(self.sizer3,0,wx.EXPAND)
        self.sizer.Add(self.sizer4,0,wx.EXPAND)
        self.sizer.Add(self.sizer5,0,wx.EXPAND)
        self.sizer.Add(self.sizer6,0,wx.EXPAND)
        self.sizer.Add(self.sizer7,0,wx.EXPAND)
        self.sizer.Add(self.sizer2,0,wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetAutoLayout(1)
        self.sizer.Fit(self)
        self.Show(True)
        wx.EVT_BUTTON(self, ID_CANCEL, self.OnCANCEL)
        wx.EVT_BUTTON(self, ID_OK, self.OnOK)
                       
    def GetValue(self):
        return self.headers,self.data
            
    def RowCount(self):
        return self.RowCount
    
    def OnCANCEL(self,e):
        self.Close()
        self.SetReturnCode(wx.ID_CANCEL)
          
    def OnOK(self,e): #,e means EVENT
        self.parent.dbsetup[0] = self.input1.GetValue()#server
        self.parent.dbsetup[1] = self.input2.GetValue()#database
        self.parent.dbsetup[2] = self.input3.GetValue()#user
        self.parent.dbsetup[3] = self.input4.GetValue()#password
        self.parent.dbsetup[4] = self.input5.GetValue()#query
        ConnectionString = "server=" + self.parent.dbsetup[0] + ";uid="+self.parent.dbsetup[2]+";pwd="+self.parent.dbsetup[3]+";database="+self.parent.dbsetup[1]
        records = DataTable("junk");
        Query = self.parent.dbsetup[4]
        Query = Query.lower()
        position = Query.find("from")
        if position > -1:
            tablename = Query[position+5:]
            table = ""
            for char in tablename:
                if char == " ":
                    break
                else:
                    table = table + char
            MSDatabase = MSDBase(table,ConnectionString)
            records = MSDatabase.GetDataTable(self.parent.dbsetup[4])
            CurrentRow = ""
            DataList = []
            self.headers = []
            for colindex in range(0,records.Columns.Count):
                item = str(records.Columns[colindex].ColumnName)
                if CurrentRow == "":
                    CurrentRow = str(item)
                else:
                    CurrentRow = CurrentRow +'\t'+ str(item)
                self.headers.append(item)
            DataList.append(CurrentRow)
            CurrentRow = ""
            for index in range(0,records.Rows.Count):
                for colindex in range(0,records.Columns.Count):
                    try:
                        item = str(records.Rows[index][colindex])
                    except:
                        item = " "
                    if item == "":
                        item = " "
                    if CurrentRow == "":
                        CurrentRow = item
                    else:
                        CurrentRow = CurrentRow +'\t'+ item
                DataList.append(CurrentRow)
                CurrentRow = ""
            self.data = DataList
            self.RowCount = len(DataList)
        else:
            print "Error, no table in the query"
        self.Close()
        self.SetReturnCode(wx.ID_OK)
        
#-----------------------------------------------------------------------
class MatchDialog(wx.Dialog):
    #time to make the donuts.
    #returns X and delta-X to be iterated and displayed/viewed
    def __init__ (self, parent, dialogmessage, rangemin, rangemax):
        wx.Dialog.__init__(self,parent,-1, dialogmessage,style = wx.DEFAULT_DIALOG_STYLE)
        self.match = wx.SpinCtrl(self, ID_MATCH1, "0", min = rangemin, max = (rangemax-1), size = (50, -1))
        self.sizer2 = wx.BoxSizer(wx.HORIZONTAL)
        self.buttons=[]
        self.prevwindow = parent
        self.buttons.append(wx.Button(self, ID_OK, "Ok"))
        self.buttons.append(wx.Button(self, ID_CANCEL, "Cancel"))
        self.sizer2.Add(self.buttons[0],1,wx.EXPAND)
        self.sizer2.Add(self.buttons[1],1,wx.EXPAND)
        # Use some sizers to see layout options
        self.sizer=wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.match,2,wx.EXPAND)
        self.sizer.Add(self.sizer2,0,wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetAutoLayout(1)
        self.sizer.Fit(self)
        self.Show(True)
        wx.EVT_BUTTON(self, ID_CANCEL, self.OnCANCEL)
        wx.EVT_BUTTON(self, ID_OK, self.OnOK)
        wx.EVT_SPINCTRL(self, ID_MATCH1, self.HighliteMatch)
        wx.EVT_TEXT(self, ID_MATCH1, self.HighliteMatch)
        
    def HighliteMatch(self, e):
        if len(self.prevwindow.foundrow) > self.match.GetValue():
            self.prevwindow.grid.MakeCellVisible(self.prevwindow.foundrow[self.match.GetValue()], self.prevwindow.foundcol[self.match.GetValue()])
            self.prevwindow.grid.SelectBlock(self.prevwindow.foundrow[self.match.GetValue()], self.prevwindow.foundcol[self.match.GetValue()], self.prevwindow.foundrow[self.match.GetValue()], self.prevwindow.foundcol[self.match.GetValue()])
        
    def GetValue(self):
        return int(self.input1.GetValue()),int(self.input2.GetValue())
    
    def OnCANCEL(self,e):
        self.Close()
        self.SetReturnCode(wx.ID_CANCEL)
          
    def OnOK(self,e): #,e means EVENT
        self.Close()
        self.SetReturnCode(wx.ID_OK)

#-----------------------------------------------------------------------
class FindDialog(wx.Dialog):
    #time to make the donuts.
    #returns X and EXPRESSION to be filtered/matched
    def __init__ (self,parent,dialogmessage):
        wx.Dialog.__init__(self,parent,-1, dialogmessage,style = wx.DEFAULT_DIALOG_STYLE)
        self.input1 = wx.TextCtrl(self, 1, style=wx.TE_LINEWRAP )
        self.casecheck = wx.CheckBox(self,-1, "Match Case")
        self.input1.SetValue(parent.resp)
        self.sizer2 = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer3 = wx.BoxSizer(wx.HORIZONTAL)
        self.buttons=[]
        self.buttons.append(wx.Button(self, ID_OK, "Ok"))
        self.buttons.append(wx.Button(self, ID_CANCEL, "Cancel"))
        self.sizer2.Add(self.buttons[0],1,wx.EXPAND)
        self.sizer2.Add(self.buttons[1],1,wx.EXPAND)
        # Use some sizers to see layout options
        self.sizer=wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.input1,1,wx.EXPAND)
        self.sizer3.Add(self.casecheck,3,wx.EXPAND)
        self.sizer.Add(self.sizer3,2,wx.EXPAND)
        self.sizer.Add(self.sizer2,0)
        self.SetSizer(self.sizer)
        self.SetAutoLayout(1)
        self.sizer.Fit(self)
        self.Show(True)
        wx.EVT_BUTTON(self, ID_CANCEL, self.OnCANCEL)
        wx.EVT_BUTTON(self, ID_OK, self.OnOK)
        
    def GetValue(self):
        return self.input1.GetValue(),self.casecheck.GetValue()
    
    def OnCANCEL(self,e):
        self.Close()
        self.SetReturnCode(wx.ID_CANCEL)
          
    def OnOK(self,e): #,e means EVENT
        self.Close()
        self.SetReturnCode(wx.ID_OK)
        
#-----------------------------------------------------------------------
class RangeDialog(wx.Dialog):
    #time to make the donuts.
    #returns X and delta-X to be iterated and displayed/viewed
    def __init__ (self, parent, dialogmessage, rangemin, rangemax):
        wx.Dialog.__init__(self,parent,-1, dialogmessage,style = wx.DEFAULT_DIALOG_STYLE)
        self.input1 = wx.TextCtrl(self, 1, style=wx.TE_WORDWRAP )
        self.input2 = wx.TextCtrl(self, 1, style=wx.TE_WORDWRAP )
        self.PosLabel1 = wx.StaticText( self, -1, "Top")
        self.PosLabel2 = wx.StaticText( self, -1, "Bottom")
        self.input1.SetValue(rangemin)
        self.input2.SetValue(rangemax)
        self.sizer2 = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer3 = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer4 = wx.BoxSizer(wx.HORIZONTAL)
        self.buttons=[]
        self.buttons.append(wx.Button(self, ID_OK, "Ok"))
        self.buttons.append(wx.Button(self, ID_CANCEL, "Cancel"))
        self.sizer2.Add(self.buttons[0],1,wx.EXPAND)
        self.sizer2.Add(self.buttons[1],1,wx.EXPAND)
        # Use some sizers to see layout options
        self.sizer=wx.BoxSizer(wx.VERTICAL)
        self.sizer3.Add(self.input1,1,wx.EXPAND)
        self.sizer3.Add(self.PosLabel1,1,wx.EXPAND)
        self.sizer4.Add(self.input2,2,wx.EXPAND)
        self.sizer4.Add(self.PosLabel2,1,wx.EXPAND)
        self.sizer.Add(self.sizer3,0,wx.EXPAND)
        self.sizer.Add(self.sizer4,0,wx.EXPAND)
        self.sizer.Add(self.sizer2,0,wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetAutoLayout(1)
        self.sizer.Fit(self)
        self.Show(True)
        wx.EVT_BUTTON(self, ID_CANCEL, self.OnCANCEL)
        wx.EVT_BUTTON(self, ID_OK, self.OnOK)
                       
    def GetValue(self):
        valid = 1 #destroy all invalid input, and handle it below.
        try:
            value1 = int(self.input1.GetValue())
        except:
            valid = 0
        try:
            value2 = int(self.input2.GetValue())
        except:
            valid = 0
        #this code ENSURES that delta x is always in the second slot
        #30,3 becomes 3,30 and 2,40 stays that way.
        if valid == 1:
            if value1>=value2: #x, delta x
                return value2,value1
            else: #x, delta x
                return value1,value2
        else:
            dlg = wx.MessageBox("Invalid input", "Error")
            
    
    def OnCANCEL(self,e):
        self.Close()
        self.SetReturnCode(wx.ID_CANCEL)
          
    def OnOK(self,e): #,e means EVENT
        self.Close()
        self.SetReturnCode(wx.ID_OK)
        self.sizer2 = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer3 = wx.BoxSizer(wx.HORIZONTAL)
        self.sizer4 = wx.BoxSizer(wx.HORIZONTAL)
        self.buttons=[]
        self.buttons.append(wx.Button(self, ID_OK, "Ok"))
        self.buttons.append(wx.Button(self, ID_CANCEL, "Cancel"))
        self.sizer2.Add(self.buttons[0],1,wx.EXPAND)
        self.sizer2.Add(self.buttons[1],1,wx.EXPAND)
        # Use some sizers to see layout options
        self.sizer=wx.BoxSizer(wx.VERTICAL)
        self.sizer3.Add(self.input1,1,wx.EXPAND)
        self.sizer3.Add(self.PosLabel1,1,wx.EXPAND)
        self.sizer4.Add(self.input2,2,wx.EXPAND)
        self.sizer4.Add(self.PosLabel2,1,wx.EXPAND)
        self.sizer.Add(self.sizer3,0,wx.EXPAND)
        self.sizer.Add(self.sizer4,0,wx.EXPAND)
        self.sizer.Add(self.sizer2,0,wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetAutoLayout(1)
        self.sizer.Fit(self)
        self.Show(True)
        wx.EVT_BUTTON(self, ID_CANCEL, self.OnCANCEL)
        wx.EVT_BUTTON(self, ID_OK, self.OnOK)
                       
    def GetValue(self):
        valid = 1 #destroy all invalid input, and handle it below.
        try:
            value1 = int(self.input1.GetValue())
        except:
            valid = 0
        try:
            value2 = int(self.input2.GetValue())
        except:
            valid = 0
        #this code ENSURES that delta x is always in the second slot
        #30,3 becomes 3,30 and 2,40 stays that way.
        if valid == 1:
            if value1>=value2: #x, delta x
                return value2,value1
            else: #x, delta x
                return value1,value2
        else:
            dlg = wx.MessageBox("Invalid input", "Error")
            
    
    def OnCANCEL(self,e):
        self.Close()
        self.SetReturnCode(wx.ID_CANCEL)
          
    def OnOK(self,e): #,e means EVENT
        self.Close()
        self.SetReturnCode(wx.ID_OK)

#-----------------------------------------------------------------------
        
class GridFrame(wx.Frame):
    #time to make the donuts.
    #modal frame to view/search/delete/etc basically this performs lots and lots of crazy functions.
    #inserts the index into the labels if you are returning matches.
    #thats what self.foundrow and self.foundcol is for...passing into here.
    def __init__ (self, parent, headers, data, foundrow, foundcol):
	
  	wx.Frame.__init__(self, parent, -1, "FIND GRID", size = (1024,768), style = wx.CAPTION | wx.DEFAULT_DIALOG_STYLE | wx.SYSTEM_MENU | wx.RESIZE_BORDER | wx.THICK_FRAME | wx.CLOSE_BOX)
	# wx.Frame.__init__(self, parent, -1, "FIND GRID", size = (500,400), style = wx.DEFAULT_DIALOG_STYLE)
        # self.SetIcon(wx.Icon(ICONBITMAP, wx.BITMAP_TYPE_ICO))
        self.sizer = wx.BoxSizer(wx.VERTICAL)
	
        self.sizer2 = wx.BoxSizer(wx.HORIZONTAL)
        #foundcol/row for grid painting
        self.foundrow = []
        self.foundcol = []
        self.resp = parent.resp
        self.parent = parent
        global foundrows #pump the variable to global status...OR FACE DEATH!
        global foundcols #BOOST TO GLOBAL!
        #add data to grid
        self.headers = headers
        self.data = data
        cols = 0
        for line in self.data: #no room for errors
            if cols<len(line):
                cols = len(line)
             #extract longest line
        if cols<len(headers):
            cols = len(headers)#more headers than columns in the row? a-ok!
        rows = len(self.data)
        self.grid = grid.Grid(self, -1,size=(1024,768))
        self.grid.CreateGrid(rows,cols)
        self.grid.SetColLabelAlignment(wx.ALIGN_CENTRE, wx.ALIGN_CENTRE)
        self.grid.BeginBatch()
        for col in range(cols): #I think I need to go over this block and reduce redundant code.
            if foundcol==[]: #this looks confusing and probably is!
                try:    #basically decide to number the cols with foundcol or not
                    if len(foundcol) == 0:
                        #falling into this part of the loop makes it not work
                        self.grid.SetColLabelValue(col, "["+str(parent.columnindex)+"]:"+headers[(col-parent.columnindex)])
                    else:
                        self.grid.SetColLabelValue(col, "["+str(col+(parent.columnindexdelta-(parent.columnindex+1)))+"]:"+headers[(col-parent.columnindex)])
                except:
                    if len(foundcol) == 0:
                        self.grid.SetColLabelValue(col, "["+str(parent.columnindex)+"]:")
                    else:
                        self.grid.SetColLabelValue(col, "["+str(col+(parent.columnindexdelta-(parent.columnindex+1)))+"]:")
            else:
                try:
                    self.grid.SetColLabelValue(col, "["+str(foundcol[col])+"]:"+headers[col])
                except:
                    self.grid.SetColLabelValue(col, "["+str(col)+"]:")
        for row in range(rows):
            if foundrow==[]:
                self.grid.SetRowLabelValue(row,str(row+parent.rowindex))
            else:
                self.grid.SetRowLabelValue(row,str(foundrow[row]))
            for col in range(len(self.data[row])):
                try:    self.grid.SetCellValue(row, col, self.data[row][col])
                except: pass
        self.grid.AutoSizeColumns(False) # size columns to data
        self.grid.EndBatch()
        #menu stuff
        self.CreateStatusBar()
        findmenu = wx.Menu()
        findmenu.Append(ID_GRIDFIND1, "&Find Header", "RegEx Header")
        findmenu.Append(ID_GRIDFIND2, "F&ind Data", "RegEx Data" )
        findmenu.AppendSeparator()
        findmenu.Append(ID_GRIDFIND3, "&Display Matches" , "Match dialog" )
        findmenu.Append(ID_GRIDFIND4, "&Select Row Matches" , "Highlight match" )
        findmenu.Append(ID_GRIDFIND5, "&Select Column Matches" , "Highlight match" )
        menuBar = wx.MenuBar()
        menuBar.Append(findmenu,"F&ind") # Add find menu of course
        self.SetMenuBar(menuBar)
        #end grid manipulation
        self.buttons=[]
        self.buttons.append(wx.Button(self, ID_CANCEL, "Close"))
        self.sizer2.Add(self.buttons[0],1,wx.CENTER)
        # Use some sizers to see layout options
        self.sizer.Add(self.grid,1,wx.EXPAND)
        self.sizer.Add(self.sizer2,0,wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetAutoLayout(1)
        self.sizer.Fit(self)
        self.MakeModal(True)
        #popup menu section:
        self.menu_titles = [ "Properties",
                             "Delete"]
                            #"Refresh"]
                
        self.menu_title_by_id = {}
        for title in self.menu_titles:
            self.menu_title_by_id[ wx.NewId() ] = title
        self.col_menu_titles = [ "Sort","Reverse","Properties","Delete"]
        self.col_menu_title_by_id = {}
        for title in self.col_menu_titles:
            self.col_menu_title_by_id[ wx.NewId() ] = title
        #popup menu defined!
        wx.EVT_BUTTON(self, ID_CANCEL, self.OnCANCEL)
        wx.EVT_MENU(self, ID_GRIDFIND1, self.FindHeader)
        wx.EVT_MENU(self, ID_GRIDFIND2, self.FindData)
        wx.EVT_MENU(self, ID_GRIDFIND3, self.DataMatches)
        wx.EVT_MENU(self, ID_GRIDFIND4, self.RowMatches)
        wx.EVT_MENU(self, ID_GRIDFIND5, self.ColumnMatches)
        grid.EVT_GRID_LABEL_LEFT_CLICK(self, self.AddMatch)
        grid.EVT_GRID_LABEL_RIGHT_CLICK(self, self.PopMenu)
        grid.EVT_GRID_EDITOR_HIDDEN(self, self.CellEdit)
        wx.EVT_CLOSE(self, self.CloseWindow) #MUST USE THIS TO ELIMINATE MODAL!

# <TODO!!!!!> write double quotes around text
    def CellEdit(self, e):
        row = e.GetRow()
        col = e.GetCol()
        self.grid.SaveEditControlValue()
        if int(self.grid.GetRowLabelValue(0)) == self.parent.HeaderRow:#untested code here.
            self.data[row][col] = self.grid.GetCellValue(row,col)
            NewRow = ""
            for item in self.data[row]:
		# put double quotes around edited item
                NewRow = NewRow + "\"" + item + "\" " + self.parent.Delimiter
            NewRow = NewRow[:-1] #truncate the extra tab.
            self.parent.data[int(self.grid.GetRowLabelValue(row))] = NewRow #change parent data set
            if row == self.parent.HeaderRow:#if editing row 0, then refresh the headers
                self.parent.Headers = self.data[row]
                self.RefreshHeaders(self)
        else:
            self.data[row][col] = self.grid.GetCellValue(row,col)#this works good.
            NewRow = ""
            for item in self.data[row]:
                NewRow = NewRow + item + self.parent.Delimiter
            NewRow = NewRow[:-1] #truncate the extra tab.
            self.parent.data[int(self.grid.GetRowLabelValue(row))] = NewRow
        self.grid.AutoSizeColumns(True)#resize grid or the lines will overlap the grid.

    def RefreshHeaders(self,e): #this function will rebuild the current headers
        cols = 0
        parent = self.parent
        headers = parent.Headers
        self.headers = headers
        for line in self.data: #no room for errors
            if cols<len(line):
                cols = len(line)
             #extract longest line
        foundcol = range(0,cols-1)
        rows = len(self.data)
        self.grid.BeginBatch()
        for col in range(cols): #I think I need to go over this block and reduce redundant code.
            if foundcol==[]: #this looks confusing and probably is!
                try:    #basically decide to number the cols with foundcol or not
                    if len(foundcol) == 0:
                        #falling into this part of the loop makes it not work
                        #also, it NEVER falls into this part of the loop
                        self.grid.SetColLabelValue(col, "["+str(parent.columnindex)+"]:"+headers[(col-parent.columnindex)])
                    else:
                        self.grid.SetColLabelValue(col, "["+str(col+(parent.columnindexdelta-(parent.columnindex+1)))+"]:"+headers[(col-parent.columnindex)])
                except: #never goes here either
                    if len(foundcol) == 0:
                        self.grid.SetColLabelValue(col, "["+str(parent.columnindex)+"]:")
                    else:
                        self.grid.SetColLabelValue(col, "["+str(col+(parent.columnindexdelta-(parent.columnindex+1)))+"]:")
            else: #always winds up in this section!
                try:
                    self.grid.SetColLabelValue(col, "["+str(foundcol[col])+"]:"+headers[col])
                except:
                    self.grid.SetColLabelValue(col, "["+str(col)+"]:")
        self.grid.AutoSizeColumns(True) # size columns to data
        self.grid.EndBatch()

    def PopMenu(self,e):
        # record what was clicked
        self.ColClick = right_click_context = e.GetCol()
        self.RowClick = right_click_context = e.GetRow()
        if self.RowClick > -1:
            ### 2. Launcher creates wxMenu. ###
            menu = wx.Menu()
            for (id,title) in self.menu_title_by_id.items():
                ### 3. Launcher packs menu with Append. ###
                menu.Append( id, title )
                ### 4. Launcher registers menu handlers with EVT_MENU, on the menu.
                if self.ColClick > -1:
                    pass #column action
                else:
                    wx.EVT_MENU( menu, id, self.GridRowMenuSelection ) #row action

            ### 5. Launcher displays menu with call to PopupMenu, invoked on the source component, passing event's GetPoint. ###
            self.PopupMenu( menu, e.GetPosition() )
        else:
            ### 2. Launcher creates wxMenu. ###
            menu = wx.Menu()
            for (id,title) in self.col_menu_title_by_id.items():
                ### 3. Launcher packs menu with Append. ###
                menu.Append( id, title )
                ### 4. Launcher registers menu handlers with EVT_MENU, on the menu.
                if self.RowClick > -1:
                    pass #row action
                else:
                    wx.EVT_MENU( menu, id, self.GridColMenuSelection ) #row action

            ### 5. Launcher displays menu with call to PopupMenu, invoked on the source component, passing event's GetPoint. ###
            self.PopupMenu( menu, e.GetPosition() )

    def GridRowMenuSelection(self, e):
        global foundrows
        operation = self.menu_title_by_id[ e.GetId() ]
        if operation == 'Properties':
            row = self.data[self.RowClick]
            cols = len(row)
            maxsize = 0
            maxsizecol = 0
            for col in range(cols):
                try:
                    size = len(row[col])
                except:
                    size = 0
                if maxsize < size:
                        maxsize = size
                        maxsizecol = col
            dlg = wx.MessageBox(str(maxsize)+" Column Width\n"+str(maxsizecol)+" Max Column Size\n"+str(col+1)+" Columns Wide", "Row Properties")
        if operation == 'Delete': #change this to delete selected rows, or a single row if there is no selection.
            SelectedRows = self.grid.GetSelectedRows()
            if SelectedRows == []:
                self.grid.DeleteRows(self.RowClick,1)
                foundrows = [] #erase list of rows to delete...or you'll regret it!
                if int(self.grid.GetRowLabelValue(0)) > 0: #if it doesn't start at zero, then perform the delete on the row
                    del self.data[self.RowClick] #delete local copy of data
                else:
                    del self.data[int(self.grid.GetRowLabelValue(self.RowClick))] #if it starts with 0, delete the row corresponding to the label.
                foundrows.append(int(self.grid.GetRowLabelValue(self.RowClick))) #setup delete for global data copy
                self.parent.OnFilterRow(self) #filter the row!
                if int(self.grid.GetRowLabelValue(self.RowClick)) == self.parent.HeaderRow: #possibly replace with parent.HeaderRow
                    self.RefreshHeaders(self)
            else:
                SelectedRows.sort()
                SelectedRows.reverse()#another classic reverse sort...remember that this is important!! logic dictates large to small
                foundrows = [] #erase list of rows to delete...or you'll regret it!
                self.grid.BeginBatch()
                RefreshData = False
                for row in SelectedRows:
                    self.grid.DeleteRows(row,1)#if there's no reverse, this will bust bigtime.
                    if int(self.grid.GetRowLabelValue(0)) > 0: #if it doesn't start at zero, then perform the delete on the row
                        del self.data[row] #delete local copy of data
                    else:
                        del self.data[int(self.grid.GetRowLabelValue(row))] #if it starts with 0, delete the row corresponding to the label.
                    if int(self.grid.GetRowLabelValue(row)) == self.parent.HeaderRow: #possibly replace with parent.HeaderRow
                        RefreshData = True
                    foundrows.append(int(self.grid.GetRowLabelValue(row))) #setup delete for global data copy
                self.grid.EndBatch()
                self.parent.OnFilterRow(self) #must filter the row first!
                if RefreshData == True: #must filter the row first then refresh headers
                    self.RefreshHeaders(self)
        if operation == 'Refresh':
            self.RefreshHeaders(self)

    def GridColMenuSelection(self, e):
        global foundcols #global status or else!
        operation = self.col_menu_title_by_id[ e.GetId() ]
        if operation == 'Properties':
            rows = len(self.data)
            maxsize = 0
            maxsizerow = 0
            for row in range(rows):
                try:
                    size = len(self.data[row][self.ColClick])
                except:
                    size = 0
                if maxsize < size:
                        maxsize = size
                        maxsizerow = int(self.grid.GetRowLabelValue(row)) #row offset
            dlg = wx.MessageBox(str(maxsize)+" Column Width\n"+str(maxsizerow)+" Max Row", "Column Properties")
        if operation == 'Delete': #also refresh labels!
            SelectedCols = self.grid.GetSelectedCols()
            if SelectedCols == []:
                if len(self.headers)>self.ColClick:
                    del self.headers[self.ColClick]
                foundcols = []
                value = self.grid.GetColLabelValue(self.ColClick)
                self.grid.DeleteCols(self.ColClick,1)
                self.RefreshHeaders(self)
                foundcols.append(int(value[(value.find("[")+1):value.find("]")]))
                self.foundcols = []
                self.foundcols.append(self.ColClick);
                #filter local column as well
                AlteredData = []
                for line in self.data:
                    for col in self.foundcols: #delete one or more selected columns
                        if len(line)>col:
                            del line[col]
                    AlteredData.append(line)
                self.data = AlteredData#line below MUST be performed last or else,
                self.parent.OnFilterColumn(self)#it will delete foundcols
            else:
                SelectedCols.sort()
                SelectedCols.reverse()#reverse sort, it's the only way to fly.
                self.grid.BeginBatch()
                foundcols = [] #erase all matches, and prep for the final delete.
                self.foundcols = []
                for col in SelectedCols:#delete existing columns in the grid.
                    del self.headers[self.ColClick]
                    value = self.grid.GetColLabelValue(col)
                    self.grid.DeleteCols(col, 1)
                    foundcols.append(int(value[(value.find("[")+1):value.find("]")]))#tag parent columns to be deleted.
                    self.foundcols.append(col)#tag columns to be deleted in the local
                self.RefreshHeaders(self)
                #filter local column as well
                AlteredData = []
                for line in self.data:
                    for col in self.foundcols: #delete one or more selected columns
                        if len(line)>col:
                            del line[col]
                    AlteredData.append(line)
                self.data = AlteredData#line below MUST be performed last or else,
                self.parent.OnFilterColumn(self)#it will delete foundcols
                self.grid.EndBatch()
        if operation == 'Sort' or operation == "Reverse":
            operation = self.col_menu_title_by_id[ e.GetId() ]
            ColumnList = []
            for row in range(0,len(self.data)):
                ColumnItem = []
                ColumnItem.append(self.data[row][self.ColClick])
                ColumnItem.append(row)
                ColumnList.append(ColumnItem)
            ColumnList.sort()
            if operation == "Reverse":
                ColumnList.reverse() #reverse sort...classic!
            self.RowSort = {}
            index = 0
            for line in ColumnList:
                self.RowSort[index] = line[1]
                index = index + 1
            self.grid.BeginBatch()
            newData = []
            sortdlg = wx.ProgressDialog("Sorting..",
                            "Progress",
                            maximum = len(self.data)*2,
                            style = wx.PD_APP_MODAL | wx.PD_AUTO_HIDE)
            for row in range(0,len(self.data)):
                sortRow = self.RowSort[row] #get which row belongs here.
                sortdlg.Update(row)
                newData.append(self.data[sortRow]) #build up new data.
                #self.grid.SetRowLabelValue(row,str(sortRow)) #don't sort the labels!!!
                for col in range(0,len(self.data[sortRow])):
                    try:    self.grid.SetCellValue(row, col, self.data[sortRow][col])
                    except: pass
            self.data = newData
            #reconstruct line array out of existing data.
            RawData = ""
            NewData = []
            count = len(self.data)
            for element in self.headers: #whip off the headers!
                RawData = RawData + element + self.parent.Delimiter
            RawData = RawData[:-1]
            RawData = RawData + '\n'
            NewData.append(RawData)
            RawData = ""
            for line in newData:#construct the rest of the lines.
                for element in line:
                    RawData = RawData + element + self.parent.Delimiter
                RawData = RawData[:-1]
                RawData = RawData + '\n'
                NewData.append(RawData)
                RawData = ""
                count = count + 1
                sortdlg.Update(count)
            self.parent.data = NewData
            self.grid.EndBatch()
            sortdlg.Destroy()


    def ColumnMatches(self,e):
        if self.foundcol!=[]:
            for cols in self.foundcol:
                match = 0
                count = 0
                for col in self.foundcol:
                    if cols == col and match == 0:
                        match = 1
                    elif cols == col and match == 1:
                        del self.foundcol[count]
                    count = count + 1
            for cols in self.foundcol:
                self.grid.SelectCol(cols,TRUE)
        
    def RowMatches(self,e):
        if self.foundrow!=[]:
            for rows in self.foundrow:
                self.grid.SelectRow(rows,TRUE)
            
    def AddMatch(self,e):
        if e.GetRow()!=-1:
            if e.ShiftDown() and self.foundrow!=[]:
                for rows in range((self.foundrow[-1]),(e.GetRow()+1)):
                    self.grid.SelectRow(rows, True)
                    self.foundrow.append(int(self.grid.GetRowLabelValue(rows)))
                self.foundcol = [] #delete column matches to make sure!
            else:
                try: #try to return an index to the column
                    datarow = self.foundcol.index(e.GetCol())
                    if datarow>-1:
                        self.grid.ClearSelection()
                        del self.foundrow[datarow] #delete matches!
                        for row in self.foundrow:
                            self.grid.SelectRow(row, True)
                    else:
                        self.grid.SelectRow(e.GetRow(), True)
                        self.foundrow.append(int(self.grid.GetRowLabelValue(e.GetRow())))
                        self.foundcol = [] #delete rows, to make sure no matches happen
                except:
                    row = e.GetRow()#code doesn't use data labels...might be bad for the self.foundrow
                    selection = 0
                    selectedRows = self.grid.GetSelectedRows()
                    for selections in selectedRows:
                        if row == selections:
                            self.grid.DeselectRow(row)
                            selection = 1
                            selectedRows.remove(row)
                            self.foundrow = selectedRows
                    if selection == 0:
                        self.grid.SelectRow(e.GetRow(), True)
                        self.foundrow.append(int(self.grid.GetRowLabelValue(e.GetRow())))
                        self.foundcol = [] #delete column matches to make sure!

            
        if e.GetCol()!=-1: #WARNING do something different because the cols
            if e.ShiftDown() and self.foundcol!=[]:
                for cols in range((self.foundcol[-1]+1),(e.GetCol()+1)):
                    self.grid.SelectCol(cols, True)
                    self.foundcol.append(cols)
                self.foundrow = [] #delete rows, to make sure no matches happen
            else:
                try: #try to return an index to the column
                    column = self.foundcol.index(e.GetCol())
                    if column>-1:
                        self.grid.ClearSelection()
                        del self.foundcol[column] #delete matches!
                        for col in self.foundcol:
                            self.grid.SelectCol(col, True)
                    else:
                        self.grid.SelectCol(e.GetCol(), True)
                        self.foundcol.append(e.GetCol())
                        self.foundrow = [] #delete rows, to make sure no matches happen
                except: #index FAILS so do this instead.
                    self.grid.SelectCol(e.GetCol(), True)
                    self.foundcol.append(e.GetCol())
                    self.foundrow = [] #delete rows, to make sure no matches happen
                        
    def DataMatches(self,e):
        if self.foundrow!=[] or self.foundcol!=[]:
            if len(self.foundrow)==len(self.foundcol):
                dlg = MatchDialog( self, "Matches 0-" + str(len(self.foundrow)-1), 0, len(self.foundrow))
                dlg.ShowModal()
            elif self.foundrow!=[]:
                for count in self.foundrow:
                    self.grid.SelectRow(count, TRUE)
                    self.grid.MakeCellVisible(count,0)
            elif self.foundcol!=[]:
                for count in self.foundcol:
                    self.grid.SelectCol(count, TRUE) #pokey mon gotta select em all!
                    self.grid.MakeCellVisible(0,count)
        
    def FindData(self,e):
        searchexp = FindDialog(self, "Search for RegEx:")
        if searchexp.ShowModal() == wx.ID_OK:
            searchexp.Destroy()
            resp, usecase = searchexp.GetValue()
            if usecase == False:
                resp = resp.lower()
            self.resp = resp
            try:
                rp = re.compile(resp)
                count = 0
                match = 0
                x = 0
                y = 0
                rows = len(self.data)
                cols = len(self.headers)
                self.grid.BeginBatch()
                #this next line may seem confusing but it's required to make sure the index
                #bounds are not violated, and some other functions too.
                if self.foundrow!=[] and self.foundcol!=[] and len(self.foundcol)==len(self.foundrow):
                    for line in range(len(self.foundrow)):
                        self.grid.SetCellBackgroundColour(self.foundrow[line],self.foundcol[line],wx.Color(255, 255, 255))
                self.foundrow = [] #erase all matches like a Good Little Boy(tm)
                self.foundcol = [] #erase all matches like a Good Little Boy(tm)
                for row in range(rows):
                    for col in range(len(self.data[row])):
                        if usecase == False:
                            if re.search(rp, self.data[row][col].lower()) != None:
                                self.foundrow.append(row)
                                self.foundcol.append(col)
                                self.grid.SetCellBackgroundColour(row,col,wx.Color(255,194,197))
                                if match == 0:
                                    x = row
                                    y = col
                                match = 1
                        else:
                            if re.search(rp, self.data[row][col]) != None:
                                self.foundrow.append(row)
                                self.foundcol.append(col)
                                self.grid.SetCellBackgroundColour(row,col,wx.Color(255,194,197))
                                if match == 0:
                                    x = row
                                    y = col
                                match = 1
                self.grid.EndBatch()
                if match == 1:
                    self.grid.MakeCellVisible(x, y)
                else:
                    dlg = wx.MessageBox("No Match", "Not Found")
            except:
                dlg = wx.MessageBox("Invalid RegEx", "Not Found")
            
    def FindHeader(self,e):
        searchexp = wx.TextEntryDialog(self, "Search", "Search for RegEx:","", wx.OK)
        if searchexp.ShowModal() == wx.ID_OK:
            searchexp.Destroy()
            resp = searchexp.GetValue()
            resp = resp.lower()
            try:
                rp=re.compile(resp)
                count = 0
                match = 0
                self.foundrow = []
                self.foundcol = []
                for line in self.headers:
                    if re.search(rp, line.lower())!= None:
                        self.foundcol.append(count)
                        self.grid.SelectCol(count, TRUE) #pokey mon gotta select em all!
                        self.grid.MakeCellVisible(0,count)
                        match = 1
                    count = count + 1
                if match == 0:
                    dlg = wx.MessageBox("No Match", "Not Found")
            except:
                dlg = wx.MessageBox("Invalid RegEx", "Not Found")
                
    def OnCANCEL(self,e):
        self.Close(True)
        self.SetReturnCode(wx.ID_CANCEL)
          
    def OnOK(self,e): #,e means EVENT
        self.Close(True)
        self.SetReturnCode(wx.ID_OK)

    def SetReturnCode(self, code):
        self.Button = code
        
    def ReturnButton(self):
        return self.Button
    
    def CloseWindow(self,e):
        #return the matching rows/cols
        global foundrows #BOOST
        global foundcols #BOOST
        if self.foundrow != [] or self.foundcol != []:
            foundrows = []
            foundcols = []
            rows = self.grid.GetSelectedRows()
            cols = self.grid.GetSelectedCols()#cols is not used....yet!
            for row in rows:
                foundrows.append(int(self.grid.GetRowLabelValue(row)))
            for col in cols:
                value = self.grid.GetColLabelValue(col)
                foundcols.append(int(value[(value.find("[")+1):value.find("]")]))
        self.MakeModal(False) #got to get rid of the modal or else!!! DEATH
        self.Destroy()
        
#-----------------------------------------------------------------------

class GridDialog(wx.Dialog):
    #time to make the donuts.
    #I sepaRATED    the dialog and frame because I need the dialog for
    #data previewing and it must be modal .ShowModal()
    #returns X and delta-X to be iterated and displayed/viewed
    def __init__ (self, parent, headers, data):
        wx.Dialog.__init__(self, parent, -1, "PREVIEW", style = wx.CAPTION | wx.SYSTEM_MENU | wx.RESIZE_BORDER | wx.THICK_FRAME | wx.CLOSE_BOX)
        self.Button = wx.ID_CANCEL
        self.sizer2 = wx.BoxSizer(wx.HORIZONTAL)
        self.grid = wx.Grid(self, -1,size=(400,200))
        #add data to grid
        self.headers = headers
        cols = len(headers)
        rows = len(data)
        self.grid.CreateGrid(rows, cols)
        self.grid.SetColLabelAlignment(wx.ALIGN_CENTRE, wx.ALIGN_CENTRE)
        self.grid.BeginBatch()
        for col in range(parent.columnindex,(parent.columnindexdelta+1)):
            try:    self.grid.SetColLabelValue((col-parent.columnindex), "["+str(col)+"]:"+headers[(col-parent.columnindex)])
            except: self.grid.SetColLabelValue(col, str(col))
        for row in range(rows):
            self.grid.SetRowLabelValue(row,str(row+parent.rowindex))
            for col in range(cols):
                try:    self.grid.SetCellValue(row, col, data[row][col])
                except: pass
        self.grid.AutoSizeColumns(False) # size columns to data
        self.grid.EndBatch()
        #end grid manipulation
        self.buttons=[]
        self.buttons.append(wx.Button(self, ID_OK, "Ok"))
        self.buttons.append(wx.Button(self, ID_CANCEL, "Cancel"))
        self.sizer2.Add(self.buttons[0],1,wx.EXPAND)
        self.sizer2.Add(self.buttons[1],1,wx.EXPAND)
        # Use some sizers to see layout options
        self.sizer=wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.grid,1,wx.EXPAND)
        self.sizer.Add(self.sizer2,0,wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetAutoLayout(1)
        self.sizer.Fit(self)
        EVT_BUTTON(self, ID_CANCEL, self.OnCANCEL)
        EVT_BUTTON(self, ID_OK, self.OnOK)
        
    def OnCANCEL(self,e):
        self.Close(True)
        self.SetReturnCode(wx.ID_CANCEL)
          
    def OnOK(self,e): #,e means EVENT
        self.Close(True)
        self.SetReturnCode(wx.ID_OK)
        
#-----------------------------------------------------------------------
class ConfigureDialog(wx.Dialog):
    #time to make the donuts.
    #returns X and delta-X to be iterated and displayed/viewed
    def __init__ (self,parent,dialogmessage):
        wx.Dialog.__init__(self,parent,-1, dialogmessage,style = wx.DEFAULT_DIALOG_STYLE)
        self.Header = parent.HeaderRow
        self.Delimiter = parent.Delimiter
       # ----begin crap
        delimSizerBox = wx.BoxSizer(wx.VERTICAL)

        delims = {
            'Tab':       '\t',
            'Semicolon': ';',
            'Comma':     ',',
            'Space':     ' ',
            }

        self.delimChecks = {}
        for label, value in delims.items():
            self.delimChecks[value] = wx.CheckBox(self, -1, label)
            delimSizerBox.Add(self.delimChecks[value], 0, wx.ALL, 3)
            wx.EVT_CHECKBOX(self, self.delimChecks[value].GetId(), self.Validate)
        self.delimChecks['Other'] = wx.CheckBox(self, -1, 'Other:')
        wx.EVT_CHECKBOX(self, self.delimChecks['Other'].GetId(), self.Validate)

        self.otherDelim = wx.TextCtrl(self, -1, size = (20, -1))
        wx.EVT_TEXT(self, self.otherDelim.GetId(), self.OnCustomDelim)
        
        if self.delimChecks.has_key(self.Delimiter):
            self.delimChecks[self.Delimiter].SetValue(True)
        elif self.Delimiter is not None:
            self.delimChecks['Other'].SetValue(True)
            self.otherDelim.SetValue(self.Delimiter)
            
        delimSizerBox.AddMany([
                (self.delimChecks['Other'], 0, wx.ALL, 3),
                (self.otherDelim, 0, wx.ALIGN_CENTER),
                ])
        self.hasHeaderRow = wx.CheckBox(self, -1, "Header Row")
        self.HeaderRow = wx.TextCtrl(self, -1, size = (20, -1))
        self.HeaderRow.SetValue(str(self.Header))
        self.hasHeaderRow.SetValue(True)
        delimSizerBox.Add(self.hasHeaderRow, 3, wx.ALIGN_CENTER_VERTICAL)
        delimSizerBox.Add(self.HeaderRow, 4, wx.ALIGN_CENTER_VERTICAL)
        wx.EVT_CHECKBOX(self, self.hasHeaderRow.GetId(), self.SetHeader)
        wx.EVT_TEXT(self, self.HeaderRow.GetId(), self.SetHeader)
        #end crap
        self.sizer2 = wx.BoxSizer(wx.HORIZONTAL)
        self.buttons=[]
        self.buttons.append(wx.Button(self, ID_OK, "Ok"))
        self.buttons.append(wx.Button(self, ID_CANCEL, "Cancel"))
        self.sizer2.Add(self.buttons[0],1,wx.EXPAND)
        self.sizer2.Add(self.buttons[1],2,wx.EXPAND)
        # Use some sizers to see layout options
        self.sizer=wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(delimSizerBox,1,wx.EXPAND)
        self.sizer.Add(self.sizer2,0,wx.EXPAND)
        self.SetSizer(self.sizer)
        self.SetAutoLayout(1)
        self.sizer.Fit(self)
        self.Show(True)
        wx.EVT_BUTTON(self, ID_CANCEL, self.OnCANCEL)
        wx.EVT_BUTTON(self, ID_OK, self.OnOK)

    def Validate(self, event = None):
        other = self.otherDelim.GetValue()
        self.Delimiter = map(lambda i, other = other: i[0] != 'Other' and i[0] or other,
                        filter(lambda i: i[1].GetValue(), self.delimChecks.items()))
        self.Delimiter = "".join(self.Delimiter)
        for a in self.delimChecks:
            self.delimChecks[a].SetValue(False)
        if self.delimChecks.has_key(self.Delimiter):
            self.delimChecks[self.Delimiter].SetValue(True)
        elif other!="":
            self.delimChecks['Other'].SetValue(True)
            self.otherDelim.SetValue(self.Delimiter)

    def SetHeader(self, e):
        self.hasHeaderRow.SetValue(len(self.HeaderRow.GetValue()))
        self.Header = int(self.HeaderRow.GetValue())
        
    def OnCustomDelim(self, event = None):
        self.delimChecks['Other'].SetValue(len(self.otherDelim.GetValue()))
        self.Delimiter = self.otherDelim.GetValue()

    def GetDelimiter(self):
        return self.Delimiter

    def GetHeaderRow(self):
        return self.Header
    
    def OnCANCEL(self,e):
        self.Close()
        self.SetReturnCode(wx.ID_CANCEL)
          
    def OnOK(self,e): #,e means EVENT
        self.Close()
        self.SetReturnCode(wx.ID_OK)
        
#-----------------------------------------------------------------------
class MainWindow(wx.Frame):
    """ We simply derive a new class of Frame. """
    def __init__(self, parent, id, title):
        wx.Frame.__init__(self,parent,wx.ID_ANY, title, size = (1024,768),
                                     style=wx.DEFAULT_FRAME_STYLE|wx.NO_FULL_REPAINT_ON_RESIZE)
        # self.SetIcon(wx.Icon(ICONBITMAP, wx.BITMAP_TYPE_ICO))
        self.control = wx.TextCtrl(self, 1,size = (1024,768), style=wx.TE_MULTILINE | wx.TE_DONTWRAP )
        self.control.SetMaxLength(1000000)#allow a million characters for input on the control.
        
	self.InputFileName = "UNDEFINED"
        #the data the application needs to keep track of everything
        self.data = []
        self.Headers = []
        self.FormatData = [] #empty list for shuffling data
        self.resp = "Expression"
        self.dbsetup = ["","","","",""]
        #index of row,col for matches on search
        global foundrows #BOOST TO GLOBAL STATUS
        global foundcols #BOOST THIS TOO
        foundrows = []
        foundcols = []
        self.rows = 0
        self.cols = 0
        self.rowindex = 0
        self.columnindex = 0
        self.rowindexdelta = 1
        self.columnindexdelta = 0
        self.Delimiter = ',' #default delimit is comma
        self.HeaderRow = 0 #list pointer to the headers, default at line 0
        if sys.argv[1:] != [ ] : #load a file if specified on the command line
            count = 0
            args = sys.argv[1]
            path = args
            self.data = []
	    
	    self.InputFileName = os.getcwd() + r'\\' + path # to be used to save on exit
	    
            f = open(path,'r')
            loaddlg = wx.ProgressDialog("Loading File..",
                        "Progress",
                        maximum = getsize(path),
                        style = wx.PD_APP_MODAL | wx.PD_AUTO_HIDE)
            self.control.SetValue("")
            count = 0
            progress = 0
            self.FormatData = [] #empty list
            for line in f:
                count=count+1
                linelen = len(line)
                progress = progress + linelen
                if linelen>1:
                    self.data.append(line)
                else:
                    self.control.AppendText("Blank line skipped\n")
                    count = count - 1
                loaddlg.Update(progress)
            loaddlg.Destroy()
            self.HeaderRow = 0
            self.FormatData = DSV.organizeIntoLines( self.data[0], '"', 1)
            self.FormatData = DSV.importDSV( self.FormatData, self.Delimiter, '"', errorHandler = DSV.padRow)
            self.Headers = self.FormatData[0]
            self.columnindexdelta = len(self.Headers)-1
            self.rows = count-1
            self.rowindexdelta = self.rows
            self.control.AppendText("Data Loaded, " + str(count) + " rows")
            f.close()
        #end application data/defaults
        sizer=wx.BoxSizer(wx.VERTICAL)
        self.CreateStatusBar() # A Statusbar in the bottom of the window
	filemenu= wx.Menu()
        filemenu.Append(ID_OPEN, "&Open"," Open a file to edit")
        filemenu.AppendSeparator()
        filemenu.Append(ID_SAVE, "&Save As"," Save Current File")
        filemenu.AppendSeparator()
        filemenu.Append(ID_EXIT,"&Exit"," Terminate the program")
        datamenu = wx.Menu()
        #datamenu.Append(ID_OP1, "&Titan picklist", "Data row transform")
        #the above function is specialized use and will not do anything for you
        datamenu.Append(ID_OP3, "&Find Data", "RegEx Search for data")
        datamenu.Append(ID_OP4, "&Import Data", "Import main window")
        datamenu.Append(ID_OP5, "&Export Data", "Export to main window")
        #datamenu.Append(ID_OP6, "&DB Query", "Import DB Query Result")
        datamenu.AppendSeparator()
        datamenu.Append(ID_OP2, "&Data Options", "Configure data")
        #functions based on rows
        rowmenu = wx.Menu()
        rowmenu.Append(ID_ROW1, "&Drop", "Drop data rows")
        rowmenu.Append(ID_ROW2, "&Rows", "View data rows")
        rowmenu.Append(ID_ROW3, "Row &Filter", "Drop Matching rows" )
        #functions based on columns
        columnmenu = wx.Menu()
        columnmenu.Append(ID_COLUMN1, "&Drop", "Drop data columns")
        columnmenu.Append(ID_COLUMN2, "&Columns","Column based view")
        columnmenu.Append(ID_COLUMN3, "Column &Filter", "Drop Matching columns")
        # Creating the menubar.
        menuBar = wx.MenuBar()
        menuBar.Append(filemenu,"&File") # Adding the "filemenu" to the MenuBar
        menuBar.Append(rowmenu,"&Row")
        menuBar.Append(columnmenu,"&Column")
        menuBar.Append(datamenu,"&Data")
        self.SetMenuBar(menuBar)  # Adding the MenuBar to the Frame content.
        wx.EVT_MENU(self, ID_EXIT, self.OnExit)
        wx.EVT_MENU(self, ID_OPEN, self.OnOpen)
        wx.EVT_MENU(self, ID_ROW2, self.OnView)
        wx.EVT_MENU(self, ID_ROW1, self.OnDrop)
        wx.EVT_MENU(self, ID_ROW3, self.OnFilterRow)
        wx.EVT_MENU(self, ID_SAVE, self.OnSaveAs)
        wx.EVT_MENU(self, ID_OP1, self.OnPickList)
        wx.EVT_MENU(self, ID_OP2, self.OnConfigure)
        wx.EVT_MENU(self, ID_OP3, self.OnFindData)
        wx.EVT_MENU(self, ID_OP4, self.OnImportData)
        wx.EVT_MENU(self, ID_OP5, self.OnExportData)
        wx.EVT_MENU(self, ID_OP6, self.OnDBQuery)
        wx.EVT_MENU(self, ID_COLUMN1, self.OnGetColumn)
        wx.EVT_MENU(self, ID_COLUMN2, self.OnViewColumn)
        wx.EVT_MENU(self, ID_COLUMN3, self.OnFilterColumn)
	wx.EVT_CLOSE(self, self.OnEVT_CLOSE)

        #Layout sizers
        sizer.Add(self.control,3,wx.EXPAND)
        self.SetSizer(sizer)
        self.SetAutoLayout(1)
        sizer.Fit(self)
        self.Show(True)
       
    #--FUNCTION EXIT
    def OnExit(self,e):
	self.Close(True)  # Close the frame.
   
   
    def OnEVT_CLOSE(self,e):
 	if self.InputFileName != "UNDEFINED":
	   self.SaveAs(self.InputFileName)
	self.Destroy()

#--FUNCTION EXPORT DATA(to main window)

    
    def OnExportData(self,e):
        controlData = ""
        self.control.SetValue("")
        for line in self.data:
            if line[-1] == '\n':
                controlData = controlData + line
                #self.control.AppendText(line)
            else:
                controlData = controlData + line + '\n'
                #self.control.AppendText(line+'\n')
        self.control.SetValue(controlData)
        
 #--FUNCTION IMPORT DATA
    def OnImportData(self,e):#count the number of \n chars..and import that number of lines.
        RawData = ""
        NewData = []
        self.data = []
        data = self.control.GetValue()
        lines = string.count(data,"\n")
        self.data = DSV.organizeIntoLines(data,'"',lines+1)
        NewData = DSV.importDSV( self.data, self.Delimiter, '"', errorHandler = DSV.padRow)
        self.Headers = NewData[0]
        self.rows = len(NewData)
        self.rowindexdelta = self.rows
        self.control.SetValue("Data Imported, " + str(self.rows) + "rows")
        
#--FUNCTION LOAD DATA
    def OnOpen(self,e):
	dlg = wx.FileDialog(None, "Choose a file", os.getcwd(), "",
			    
                                   "CSV(*.csv)|*.csv|Text files (*.txt)|*.txt|All files (*.*)|*.*",
                                   wx.OPEN)
        if dlg.ShowModal() == wx.ID_OK:
            self.data = []
            NewData = ""
            path = dlg.GetPath()
	    self.InputFileName = path
	    # <TODO!!!!!>
            f = open(path,'r')
            
            loaddlg = wx.ProgressDialog("Loading File..",
                        "Progress",
                        maximum = getsize(path),
                        style = wx.PD_APP_MODAL | wx.PD_AUTO_HIDE)
            self.control.SetValue("")
            count=0
            progress = 0
            self.FormatData = [] #empty list
            dlg.Destroy()
            for line in f:
                count=count+1
                linelen = len(line)
                progress = progress + linelen
                if linelen>1:
                    self.data.append(line) 
                else:
                    self.control.AppendText("Blank line skipped\n")
                    count = count - 1
                loaddlg.Update(progress)
            self.HeaderRow = 0
            self.FormatData = DSV.organizeIntoLines( self.data[0], '"', 1)
            if len(self.data)<10000: #guess the delimiter
              delimitertest = DSV.guessDelimiter(self.data)
            else:
              delimitertest = DSV.guessDelimiter(self.data[0:10000])
            if delimitertest == "\t" and self.Delimiter != "\t": #test for tab as new delimiter when loading
              self.control.AppendText("Delimiter changed to <TAB>\n")
              self.Delimiter = "\t"
            elif delimitertest == "," and self.Delimiter != ",": #test for comma as new delimiter when loading
              self.control.AppendText("Delimiter changed to ,\n")
              self.Delimiter = ","
            elif delimitertest == ";" and self.Delimiter != ";": #test for semicolon as new delimiter when loading
              self.control.AppendText("Delimiter changed to ;\n")#other delimiters are left up to the user to pick as "other" in config
              self.Delimiter = ";"
            self.FormatData = DSV.importDSV( self.FormatData, self.Delimiter, '"', errorHandler = DSV.padRow)
            self.Headers = self.FormatData[0]
            self.columnindexdelta = len(self.Headers)-1
            self.rows=(count-1)
            self.rowindexdelta = self.rows
            loaddlg.Destroy()
            self.control.AppendText("Data Loaded, " + str(count) + " rows")
            self.SetTitle("SUPER-ALPHA-TAG-EDITOR: " + path)
            os.chdir(os.path.dirname(path))
            f.close()
            
            
#--FUNCTION DISPLAYROW(s)          
    def OnView(self,e):
        #rowinput = wxTextEntryDialog(self, "View", "(0-" + str(self.rows)+") rows/range:","", wxOK)
        rowinput = RangeDialog(self, "(0-" + str(self.rows)+") rows/range:", str(self.rowindex), str(self.rowindexdelta))
        RawData = []
        self.columnindex = 0
        self.columnmax = 0
        PickLine = ""
        Headers = []
        if rowinput.ShowModal() == wx.ID_OK:
            #rowinput.Destroy()
            self.rowindex, self.rowindexdelta = rowinput.GetValue()
            try:
                self.rowindex, self.rowindexdelta = rowinput.GetValue()
                if len(self.data[self.rowindex:(self.rowindexdelta+1)]) > 0:
                    RawData.append(self.data[self.HeaderRow])
                    RawData = RawData + self.data[self.rowindex:(self.rowindexdelta+1)]
                    self.control.SetValue("")
                    NewData = DSV.importDSV( RawData, self.Delimiter, '"', errorHandler = DSV.padRow)
                    #dump headers
                    self.Headers = NewData[0] #row 0 is always the header because I build up the data like that
                    self.columnindexdelta = 1 #len(self.Headers)
                    self.columnindex = 0
                    gridview = GridFrame(self, self.Headers, NewData[1:], [], range(len(self.Headers)))
                    self.control.SetValue("Viewing Rows")
                    gridview.Show(True)
                else:
                    self.control.SetValue("There is no data for rows "+ str(self.rowindex) + "-" + str(self.rowindexdelta) )
            except:
                pass
            rowinput.Destroy()
                   
#--FUNCTION DROP ROW(s)
    def OnDrop(self,e):
        rowinput = RangeDialog(self, "(0-" + str(self.rows)+") rows/range:", str(self.rowindex), str(self.rowindexdelta))
        if rowinput.ShowModal() == wx.ID_OK:
            try:
                rowindex, rowindexdelta = rowinput.GetValue()
                if self.rows>=rowindexdelta:
                    self.control.SetValue("")
                    if rowindexdelta-rowindex>DISPLAYLIMIT:
                        self.control.SetValue("Over " + str(DISPLAYLIMIT) + " lines dropped:\n")
                        self.control.AppendText("Dropped line " + str(rowindex) + ": " +self.data[rowindex])
                        self.control.AppendText("...to...\n")
                        self.control.AppendText("Dropped line " + str(rowindexdelta) + ": " +self.data[(rowindexdelta)])
                    else:
                        for count in range(rowindex,(rowindexdelta+1)):
                            self.control.AppendText("Dropped line " + str(count) + ": " +self.data[count])
                    del self.data[rowindex:(rowindexdelta+1)]
                    self.rows=(len(self.data)-1)
                    self.Headers = self.data[0]
                    self.HeaderRow = 0 #make sure the header index is valid again
                else:
                    self.control.SetValue("Specified index " + str(rowindex) + " is beyond the EOF(" + str(self.rows) + ")" )
            except:
                pass

#--FUNCTION SAVE DATA

    def OnSaveAs(self,e):
	dlg = wx.FileDialog(None, "Choose a file", os.getcwd(), "",
                                   "CSV(*.csv)|*.csv|Text files (*.txt)|*.txt|All files (*.*)|*.*",
                                   wx.SAVE)
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
	    self.SaveAs(path)
	    dlg.Destroy()
    
        
	
    def SaveAs(self, path):
	print "save path: " + path
	f = open(path,'w')
	savedlg = wx.ProgressDialog("Saving File..",
		    "Progress",
		    maximum = len(self.data),
		    style = wx.PD_APP_MODAL | wx.PD_AUTO_HIDE)
	count = 0
	print self.data
	for line in self.data:
	    if line[-1] == '\n':
		f.write(line)
		count = count+1
		savedlg.Update(count)
	    else:
		f.write(line+'\n')
		count = count+1
		savedlg.Update(count)
	savedlg.Destroy()
	self.control.SetValue("Data Saved, " + str(self.rows) + " rows")
	self.SetTitle("SUPER-ALPHA-TAG-EDITOR: " + path)
	os.chdir(os.path.dirname(path))
	f.close()
	
#--FUNCTION TRANSFORM DATA TO PICKLIST
    def OnPickList(self,e):
        NewData = []
        AlteredData = []
        currentline = []
        PickLine = ""
        oldSelect = ""
        RawData = self.data[self.HeaderRow]
        rowinput = RangeDialog(self, "(0-" + str(self.rows)+") rows/range:", str(self.rowindex), str(self.rowindexdelta))
        if rowinput.ShowModal() == wx.ID_OK:
            try:
                rowindex, rowindexdelta = rowinput.GetValue()
                self.control.SetValue("") 
                NewData = DSV.importDSV( self.data, self.Delimiter, '"', errorHandler = DSV.padRow)
                oldSelect = NewData[0][0]
                for theline in NewData:
                    for s in theline:
                        if s!='':
                            if s[-1]=='\n':
                                currentline.append(s[0:-1]) #eliminate newlines
                            else:
                                currentline.append(s)
                        else:
                            currentline.append(' ')
                    if oldSelect!=currentline[0]:
                        AlteredData.append(PickLine+"\n")
                        PickLine = "["+currentline[0]+"]`" + currentline[0]+ "`L`"
                        currentline[rowindex] = currentline[rowindex].replace(',','')
                        PickLine = PickLine + currentline[rowindex] + "~" + currentline[rowindexdelta]
                        oldSelect = currentline[0]
                        currentline=[]
                    else:
                        currentline[rowindex] = currentline[rowindex].replace(',','')
                        PickLine = PickLine + "," + currentline[rowindex] + "~" + currentline[rowindexdelta]
                        oldSelect = currentline[0]
                        currentline=[]
                AlteredData.append(PickLine+"\n")
                self.data = AlteredData
                self.rows = (len(AlteredData)-1)
                self.control.SetValue("Data has been altered, "+ str(self.rows)+"rows")
            except:
                pass
        
#--FUNCTION ELIMINATE DATA COLUMN(s)
    def OnGetColumn(self,e):
        rowinput = RangeDialog(self, "(0-" + str(len(self.Headers)-1)+") rows/range:", str(self.columnindex), str(self.columnindexdelta))
        if rowinput.ShowModal() == wx.ID_OK:
            try:
                colindex, colindexdelta = rowinput.GetValue()
                NewData = []
                AlteredData = []
                currentline = []
                PickLine = ""
                self.control.SetValue("")
                RawData = []
                RawData.append(self.data[self.HeaderRow])
                RawData = RawData + self.data[(self.HeaderRow+1):]
                self.control.SetValue("") 
                RawData = DSV.importDSV( RawData, self.Delimiter, '"', errorHandler = DSV.padRow)
                for line in RawData:
                    AlteredData.append(line[:colindex] + line[(colindexdelta+1):])
                #dump headers
                self.Headers = AlteredData[self.HeaderRow]
                self.columnindexdelta = len(self.Headers)
                gridview = GridDialog(self, self.Headers, AlteredData[(self.HeaderRow+1):10])
                if gridview.ShowModal() == wx.ID_OK:
                    dlg = wx.ProgressDialog("Applying changes..",
                            "Converting",
                            maximum = len(AlteredData),
                            style = wx.PD_APP_MODAL | wx.PD_AUTO_HIDE)
                    RawData = ""
                    NewData = []
                    count = 0
                    for line in AlteredData:
                        count = count + 1
                        for element in line:
                            RawData = RawData + element + self.Delimiter
                        RawData = RawData[:-1]
                        RawData = RawData + '\n'
                        NewData.append(RawData)
                        RawData = ""
                        dlg.Update(count)
                    self.data = NewData
                    dlg.Destroy()
                    self.control.AppendText("Data has been altered, "+ str(self.rows)+"rows")
                else:
                    self.control.SetValue("Changes cancelled")
            except:
                pass
            
#--FUNCTION DISPLAY DATA COLUMN(s)
    def OnViewColumn(self,e):
        rowinput = RangeDialog(self, "(0-" + str(len(self.Headers)-1) + ") rows/range:", str(self.columnindex), str(self.columnindexdelta))
        if rowinput.ShowModal() == wx.ID_OK:
            try:
                self.columnindex, self.columnindexdelta = rowinput.GetValue()
                NewData = []
                AlteredData = []
                currentline = []
                PickLine = ""
                self.control.SetValue("")
                AlteredData.append(self.Headers[self.columnindex:(len(self.Headers)+1+self.columnindex)])
                NewData = DSV.importDSV( self.data, self.Delimiter, '"', errorHandler = DSV.padRow)
                for line in NewData[self.rowindex:(self.rowindexdelta+1)]:
                    partofline = line[self.columnindex:(self.columnindexdelta+1)]
                    AlteredData.append(partofline)
                #dump headers
                gridview = GridFrame(self, self.Headers[self.columnindex:(len(self.Headers)+1+self.columnindex)], AlteredData[1:], [], range(self.columnindex,len(self.Headers)+1+self.columnindex))
                gridview.Show(True)
                self.control.SetValue("Viewed Columns")
            except:
                pass
            
#--FUNCTION CONFIGURE APPLICATION
    def OnConfigure(self, e):
        rowinput = ConfigureDialog(self, "Configure")
        if rowinput.ShowModal() == wx.ID_OK:
            self.Delimiter = rowinput.GetDelimiter()
            self.HeaderRow = rowinput.GetHeaderRow()
            self.rowindex = self.HeaderRow + 1
            self.control.SetValue("Configuration changed")
        else:
            self.control.SetValue("Config cancelled")
            
 #--FUNCTION FIND DATA IN ROW
    def OnFindData(self, e):
        global foundrows #BOOST
        global foundcols #BOOST!
        searchexp = FindDialog(self, "Find RegEx:")
        if searchexp.ShowModal() == wx.ID_OK:
            self.resp, caseexp = searchexp.GetValue()
            match = 0
            currentline = []
            foundrows = [] #make sure the matches are erased.
            foundcols = [] #erase all matches!
            if caseexp==False:
                self.resp = self.resp.lower()
            try:
                rp=re.compile(self.resp)
                NewDataRows = []
                self.control.SetValue("")
                NewData = []
                AlteredData = []
                currentline = []
                row = self.HeaderRow + 1
                for line in self.data[(self.HeaderRow+1):]:
                    match = 0
                    result = None
                    if self.resp == "":
                        if line == "":
                            result = 1
                    else:
                        if caseexp == False:
                            result = re.search(rp,line.lower())
                        else:
                            result = re.search(rp,line)
                    if result!=None:
                        match = 1
                    if match == 1:
                        AlteredData.append(self.data[row])
                        foundrows.append(row)
                    row = row + 1
                #dump headers
                self.control.AppendText("Displaying " + str(len(foundrows)) + " matches" )
                RawData = self.data[self.HeaderRow]
                for line in AlteredData: #spit out raw data
                    RawData = RawData + line
                AlteredData = DSV.organizeIntoLines( RawData, '"', self.rows)
                AlteredData = DSV.importDSV( AlteredData, self.Delimiter, '"', errorHandler = DSV.padRow)
                self.Headers = AlteredData[0]
                self.columnindexdelta = len(self.Headers)
                gridview = GridFrame(self, self.Headers, AlteredData[1:], foundrows, range(len(self.Headers)) )
                gridview.Show(True)
            except:
                dlg = wx.MessageBox("Invalid RegEx", "Not Found")
        else:
            self.control.SetValue("Search cancelled")
            
#--FUNCTION DROP FOUND ROWS
    def OnFilterRow(self, e):
        global foundrows
        if foundrows!=[]:
            foundrows.sort()
            foundrows.reverse()
            self.control.SetValue("Rows Dropped\n")
            #display last/first in classic reversal of reverse sort ;)
            self.control.AppendText( str(foundrows[-1]) + ": "+ self.data[foundrows[-1]] )
            self.control.AppendText("\nto..\n")
            self.control.AppendText( str(foundrows[0]) + ": "+ self.data[foundrows[0]] )
            for row in foundrows:
                del self.data[row]
                self.rows = self.rows - 1
            foundrows = [] #delete all the matches, or YOU'LL BE SORRY!
            RawData = DSV.organizeIntoLines( self.data[self.HeaderRow], '"', 1)
            AlteredData = DSV.importDSV( RawData, self.Delimiter, '"', errorHandler = DSV.padRow)
            self.Headers = AlteredData[0]
            
#--FUNCTION DROP FOUND COLS
    def OnFilterColumn(self, e):
        global foundcols
        if foundcols!=[]:
            foundcols.sort()
            foundcols.reverse()
            NewData = []
            AlteredData = []
            PickLine = ""
            RawData = []
            RawData.append(self.data[self.HeaderRow])
            RawData = RawData + self.data[(self.HeaderRow+1):]
            RawData = DSV.importDSV( RawData, self.Delimiter, '"', errorHandler = DSV.padRow)
            for line in RawData:
                for col in foundcols:
                    if len(line)>col:
                        del line[col]
                AlteredData.append(line)
            #dump headers
            self.Headers = AlteredData[self.HeaderRow]
            self.columnindexdelta = len(self.Headers)
            RawData = ""
            NewData = []
            #count = 0
            for line in AlteredData:
                #count = count + 1
                for element in line:
                    RawData = RawData + element + self.Delimiter
                RawData = RawData[:-1]
                RawData = RawData + '\n'
                NewData.append(RawData)
                RawData = ""
                #dlg.Update(count)
            self.data = NewData
            foundcols = [] #delete all the matches, or YOU'LL BE SORRY!
            self.control.SetValue("Columns Dropped\n")
            
    def OnDBQuery(self,e):
        QueryInput = DBQuery(self,"Query Params")
        if QueryInput.ShowModal() == wx.ID_OK:
            results = QueryInput.RowCount
            if results > 0:
                HeaderRow, self.data = QueryInput.GetValue()
                self.rows = len(self.data)-1
                self.HeaderRow = 0 #reset header, as per spec!
                self.Headers = DSV.importDSV( HeaderRow, self.Delimiter, '"', errorHandler = DSV.padRow)
                self.rowindexdelta = self.rows
                self.control.SetValue("Data Imported, " + str(results) + "rows")
            else:
                self.control.SetValue("No Results to import")
#-------------------------------------------------------------------------
            
app = wx.PySimpleApp()
title = "SUPER-ALPHA-TAG-EDITOR"
if sys.argv[1:] != [ ] :
    title = "SUPER-ALPHA-TAG-EDITOR: " + sys.argv[1]
frame = MainWindow(None, -1, title)
app.MainLoop()
