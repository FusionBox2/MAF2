import wx
import CustomGaugeWx
import Queue , time
import os, sys
import wx.lib.scrolledpanel as scrolled
import subprocess
import threading, thread, CustomThread
from Debug import Debug


# GAUGE
NOT_PULSE = 0
PULSE = 1


class GuiPart(wx.Frame):
    def __init__(self, master, queue, endCommand):
        wx.Frame.__init__(self, None, -1, size=(500, 400), style = wx.DEFAULT_FRAME_STYLE|wx.RESIZE_BORDER|wx.MAXIMIZE_BOX)
        self.master = master
        self.queue = queue 
        self.complete = 0 #set to 1 when all VME ahve benn uploaded
        
        # Set up the GUI
        self.SetTitle("Upload / DownLoad Manager")
        self.scrolledPanel = scrolled.ScrolledPanel(self, -1, size=(140, 300),
                                 style = wx.TAB_TRAVERSAL|wx.SUNKEN_BORDER, name="panel1" )
        self.fgs1 = wx.BoxSizer(wx.VERTICAL)
        
        self.bars = []
        self.endingBars = []
        #self.console = wx.Button(self.scrolledPanel, 10,  'Waiting...', (150,0), (100,25))
        self.staticLabel = wx.StaticText(self.scrolledPanel, -1,  'Uploads (and Downloads)', (150,15), (200,25))
        self.staticLine = wx.StaticLine(self.scrolledPanel, -1, (25,35), (360,2))
       
        self.fgs1.Add(self.staticLabel, 0,  wx.ALIGN_CENTER_HORIZONTAL)
        self.fgs1.Add(self.staticLine, 0 , wx.ALIGN_CENTER_HORIZONTAL)
    
        self.scrolledPanel.SetSizer( self.fgs1 )
    
        self.CreateStatusBar()
        self.uploadNumber = 0
        self.downloadNumber = 0
        #self.SetStatusText("Upload :" + str(self.uploadNumber) + "    " + "Download :" + str(self.downloadNumber))

        # responds to exit symbol x on frame title bar
        self.Bind(wx.EVT_CLOSE, self.OnClose)
        
        #wx.EVT_BUTTON(self,10,self.OnCreateBar)
        self.EVT_RESULT(self.OnCreateBar)
        self.Show()
        # Add more GUI stuff here
        self.quitApplication = endCommand
        
        self.timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.processIncoming)
        self.timeToCall = 100
        self.timer.Start(milliseconds=self.timeToCall, oneShot=True)
        self.barCreated = False
        self.ModalityGauge = "UPLOAD" 
 
    
    def EVT_RESULT(self,func):
        self.Connect(-1,-1,10,func)
   
    def OnCreateBar(self, event):
        self.__createBar()
        
    def OnClose(self, event):
        if (self.complete != 1):
            answer = wx.MessageBox("Uploader/Downloader is still working. Quit?", "Confirm", wx.STAY_ON_TOP | wx.OK | wx.CANCEL )
            if (answer == wx.OK):
                self.Destroy()    
                answer.Destroy()
 
        if (self.complete == 1):
            self.Destroy()
            

    def __createBar(self):
        self.bars.append(CustomGaugeWx.CustomGaugeWx(self.scrolledPanel, len(self.bars), 100, (160, (len(self.bars)+1) * 50), (self.staticLine.GetSize()[0]/2.0, 25), gaugeModality = self.ModalityGauge ,gaugePulse = NOT_PULSE))
        self.fgs1.Add(self.bars[len(self.bars)-1],flag=wx.CENTER)
        
        #self.scrolledPanel.SetAutoLayout(1)
        self.scrolledPanel.SetupScrolling()
        self.Refresh()

        self.barCreated = True
        if(self.timer.IsRunning() == False): self.timer.Start(milliseconds=self.timeToCall, oneShot=True)
        pass
    
    def createBar(self, modality):
        self.ModalityGauge = modality
        event = wx.PyEvent(10)
        event.SetEventType(10)
        self.barCreated = False
        wx.PostEvent(self, event)
        while(self.barCreated == False):
            pass
            
                  
    def createLabel(self , title):  
        self.bars[len(self.bars)-1].SetTitle(title)
        pass
        
    def __del__(self):
        self.quitApplication()
        wx.Frame.__del__(self)

    def processIncoming(self, args = None):
        """
        Handle all the messages currently in the queue (if any).
        """
        self.timer.Stop()
        while self.queue.qsize():
            try:
                lista = self.queue.get(0)
                #lista is composed by observer (the gauge) and a value
                #if value is -1 or gauge as option gaugepulse, is pulsing
                #else set the value of the progress
                                
                    
                if(lista[0].gaugePulse == 0 and lista[1] != -1):
                   lista[0].gauge.SetValue(int(lista[1]))
                   if len(lista) == 3:
                    remainingTime = round(float(lista[2]),1)
                    lista[0].SetRemainingTimeLabel(str(remainingTime) + 's left')
                   
                else:
                   lista[0].gauge.Pulse()
                   #here calculate time
                    
                   lista[0].SetEndingLabel(str(lista[1]))
                    
                if(lista[1] == 110):
                    self.complete = 0
                    lista[0].SetEndingLabel('Completed!')
                    self.queue.task_done(0)
                self.Refresh()
                if(lista[1] == 120):
                    self.complete = 0
                    lista[0].SetEndingLabel('Error!')
                    self.queue.task_done(0)
                self.Refresh()
                if(lista[1] == 130):
                    wx.MessageBox("Upload/Download Complete!", wx.MessageBoxCaptionStr, wx.STAY_ON_TOP | wx.OK)
                    self.complete = 1
                    self.queue.task_done(0)
                self.Refresh()

            except:
                pass
        else:
            try:
                self.uploadNumber = self.downloadNumber = 0
                finished = True
                for i in self.bars:
                    if(i.gauge.GetValue() != 110 or i.gauge.GetValue() != 120):
                        finished = False
                        self.uploadNumber += 1
                #self.SetStatusText('Upload :' + str(self.uploadNumber) + '    ' + 'Download :' + str(self.downloadNumber))
                if (finished == False): 
                    self.timer.Start(milliseconds=self.timeToCall, oneShot=True)
            except:
			    pass

def test():
    def endFunction(): print "ending"
    import Queue
    app = wx.PySimpleApp()
    gui = GuiPart(None , Queue.Queue(), endFunction)
    app.MainLoop()

if __name__ == '__main__':
  test()