import wx
import sys

class CustomGaugeWx(wx.BoxSizer):
    def __init__(self, parent, id = -1,  range = 100, pos = wx.DefaultPosition, size=wx.DefaultSize, style= wx.GA_HORIZONTAL, validator=wx.DefaultValidator, name=wx.GaugeNameStr, title="123456789101213" , orient = wx.VERTICAL, gaugeModality = "UPDATE", gaugePulse = 0):
        wx.BoxSizer.__init__(self,orient)
        self.gaugeModality = gaugeModality #bar is for upload "UPLOAD" or download "DOWNLOAD"
        self.gaugePulse = gaugePulse #if 0 Gauge is not in pulsing mode 
       
        self.sizer1 = wx.BoxSizer(wx.HORIZONTAL)
        self.initialLabel = wx.StaticText(parent, -1, label = "123456789101213" ,pos = wx.DefaultPosition, size = wx.DefaultSize, style = wx.ALIGN_LEFT)      
        self.endingLabel = wx.StaticText(parent, -1, label = "")
        self.gauge = wx.Gauge(parent, id, range, pos, size, style = wx.ALIGN_CENTER , validator=wx.DefaultValidator, name=wx.GaugeNameStr)
        self.png = None

        
        #upoad or download
        if (self.gaugeModality == "UPLOAD"): self.png = wx.Image(sys.path[0] + "/bitmaps/up.png", wx.BITMAP_TYPE_PNG).ConvertToBitmap()
        elif (self.gaugeModality == "DOWNLOAD"): self.png = wx.Image(sys.path[0] + "/bitmaps/down.png", wx.BITMAP_TYPE_PNG).ConvertToBitmap()
        
        #pulse or not
        #if(self.gaugePulse == 0): self.gauge.Pulse()
        
        
        self.image = wx.StaticBitmap(parent, -1, self.png, size = (self.png.GetWidth(), self.png.GetHeight()))
        
        self.sizer1.Add(self.initialLabel, 0, wx.ALIGN_LEFT | wx.ALIGN_CENTRE_VERTICAL)
        self.sizer1.Add(self.image, 0, wx.ALIGN_CENTER | wx.ALIGN_CENTRE_VERTICAL)
        self.sizer1.Add(self.gauge, 0, wx.ALIGN_CENTER )
        self.sizer1.AddSpacer(5)
        self.sizer1.Add(self.endingLabel, 1, wx.ALIGN_RIGHT | wx.ALIGN_CENTRE_VERTICAL)
        
        self.AddSpacer(5)
        self.line = wx.StaticLine(parent, -1, size = (self.gauge.GetSize()[0]*2 , 2))
        self.Add(self.sizer1, 0 , wx.ALIGN_LEFT)
        self.AddSpacer(5)
        self.Add(self.line, 0 , wx.ALIGN_CENTER)
        self.AddSpacer(5)
        
    
    def SetTitle(self, title):
        #15 letters max
        letterLimit = 15
        title = title[:letterLimit]
        spaces = (letterLimit - len(title))
        whiteSpace = "               "
        whiteSpace = whiteSpace[0:spaces+1]
        title = title + whiteSpace
        
        self.initialLabel.SetLabel(title)
    
    def SetEndingLabel(self, label):
        self.endingLabel.SetLabel(label)

def test():
    app = wx.PySimpleApp()    
    frame = wx.Frame(None, -1)
    gaugeContainer = CustomGaugeWx(frame, pos = (100,100),gaugeModality ="DOWNLOAD")
    #gaugeContainer.gauge.SetValue(50)
    frame.Show(True)
    app.MainLoop()

if __name__ == '__main__':
  test()