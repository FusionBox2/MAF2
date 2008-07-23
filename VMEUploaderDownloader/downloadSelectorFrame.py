# -*- coding: iso-8859-1 -*-
# Don't modify comment 

import wx
#[inc]add your include files here
import listBasket
#[inc]end your include

class downloadSelectorFrame(wx.Frame):
    def __init__(self,parent,id = -1,title='',pos = wx.Point(1,1),size = wx.Size(410,210),style = wx.DEFAULT_FRAME_STYLE,name = 'frame', app = None):
        self.App = app
        self.lBasket = listBasket.listBasket()
        self.lBasket.SetCredentials(self.App.user, self.App.password)
        pre=wx.PreFrame()
        self.OnPreCreate()
        pre.Create(parent,id,title,pos,size,style,name)
        self.PostCreate(pre)
        self.initBefore()
        self.VwXinit()
        self.initAfter()
        self.Selections = []
        

    def __del__(self):
        self.Ddel()
        return

    def VwXinit(self):
        self.Show(False)
        self.panel = wx.Panel(self,-1,wx.Point(-5,-5),wx.Size(435,200))
        self.downloadButton = wx.Button(self.panel,-1,"",wx.Point(141,151),wx.Size(130,20))
        self.downloadButton.SetLabel("Download")
        self.Bind(wx.EVT_BUTTON,self.downloadButton_VwXEvOnButtonClick,self.downloadButton)
        self.checkList = wx.CheckListBox(self.panel,-1,wx.Point(41,11),wx.Size(332,133),[])
        self.FillList()
        self.verticalSizer = wx.BoxSizer(wx.VERTICAL)
        self.horizontalCheckListSizer = wx.BoxSizer(wx.HORIZONTAL)
        self.horizontalButtonSizer = wx.BoxSizer(wx.HORIZONTAL)
        
        self.horizontalCheckListSizer.Add(self.checkList,1,wx.CENTER|wx.EXPAND|wx.FIXED_MINSIZE,3)
        self.horizontalButtonSizer.Add(self.downloadButton,1,wx.CENTER|wx.EXPAND|wx.FIXED_MINSIZE,3)
        self.verticalSizer.Add(self.horizontalCheckListSizer,3,wx.CENTER|wx.EXPAND|wx.FIXED_MINSIZE,3)
        self.verticalSizer.Add(self.horizontalButtonSizer,1,wx.CENTER|wx.EXPAND|wx.FIXED_MINSIZE,3)
        
        self.panel.SetSizer(self.verticalSizer);self.panel.SetAutoLayout(1);self.panel.Layout();
        self.Refresh()
        return
    
    def FillList(self):
        self.lBasket.getListFromBasket()
        write = False
        card = 0
        for count in range(0,len(self.lBasket.IdList)):
          write = not write
          card+1
          if(write == True): #show only VME name and not datasource-...
               self.checkList.Insert(self.lBasket.IdList[count],card)
             

    def VwXDelComp(self):
        return

#[win]add your code here
    def downloadButton_VwXEvOnButtonClick(self,event): #init function
        #[271]Code event VwX...Don't modify[271]#
        #add your code here
        totalNumberOfSelected = 0
        self.Selections = []
        for count in range(0,self.checkList.GetCount()):
            if(self.checkList.IsChecked(count)):
                #self.Selections.append(self.checkList.GetItems()[count])
                self.Selections.append(self.lBasket.IdList[(count*2)+1])#get datasource-... corrisponding to VME name checked
        #wx.MessageBox("Selections: " + str(self.Selections))
        if(len(self.Selections) == 0): 
            wx.MessageBox("Must be selected some vme")
            return
        #if(len(self.Selections) != 1): 
        #    wx.MessageBox("You can Select for Now 1 only vme (temporarly)")
        #    return
        self.lBasket.IdListSelected = self.Selections #copy selection list for listBasket selected list
        self.lBasket.writeIdListSelectedOnFile()
        self.App.ExitMainLoop()
        return #end function

    def OnPreCreate(self):
        #add your code here

        return

    def initBefore(self):
        #add your code here

        return

    def initAfter(self):
        #add your code here
        self.Centre() 
        self.Show()
        return

    def Ddel(self): #init function
        #[ f9]Code VwX...Don't modify[ f9]#
        #add your code here

        return #end function

#[win]end your code
