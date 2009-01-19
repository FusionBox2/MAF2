import wx
import sys

Debug = 1

class CustomGaugeWx(wx.BoxSizer):
    def __init__(self, parent, id = -1,  range = 100, pos = wx.DefaultPosition, size=wx.DefaultSize, style= wx.GA_HORIZONTAL, validator=wx.DefaultValidator, name=wx.GaugeNameStr, title="123456789101213" , orient = wx.VERTICAL, gaugeModality = "UPDATE", gaugePulse = 0):
        wx.BoxSizer.__init__(self,orient)
        
        self.gaugeModality = gaugeModality #bar is for upload "UPLOAD" or download "DOWNLOAD"
        self.gaugePulse = gaugePulse #if 0 Gauge is not in pulsing mode 
       
        self.sizer1 = wx.BoxSizer(wx.VERTICAL)
        self.vmeNameStaticText = wx.StaticText(parent, -1, label = "123456789012345678901234567890" ,pos = wx.DefaultPosition, size = wx.DefaultSize, style = wx.ALIGN_LEFT)      
        self.endingLabel = wx.StaticText(parent, -1, label = "             ")
        self.remainingTimeLabel = wx.StaticText(parent, -1, label = "             ")
        self.gauge = wx.Gauge(parent, id, range, pos, size, style = wx.ALIGN_CENTER , validator=wx.DefaultValidator, name=wx.GaugeNameStr)
        self.png = None
        
        #upoad or download
        if (self.gaugeModality == "UPLOAD"): self.png = wx.Image(sys.path[0] + "/bitmaps/up.png", wx.BITMAP_TYPE_PNG).ConvertToBitmap()
        elif (self.gaugeModality == "DOWNLOAD"): self.png = wx.Image(sys.path[0] + "/bitmaps/down.png", wx.BITMAP_TYPE_PNG).ConvertToBitmap()
        
        self.image = wx.StaticBitmap(parent, -1, self.png, size = (self.png.GetWidth(), self.png.GetHeight()))
        self.sizer1.AddSpacer(10)
        self.sizer1.Add(self.vmeNameStaticText)
        if Debug:
            print self.vmeNameStaticText
        self.sizer1.AddSpacer(10)
        self.sizer1.Add(self.image)
        self.sizer1.AddSpacer(10)
        self.sizer1.Add(self.gauge)
        self.sizer1.AddSpacer(10)
        self.sizer1.Add(self.endingLabel)
        self.sizer1.AddSpacer(10)
        self.sizer1.Add(self.remainingTimeLabel)
        self.sizer1.AddSpacer(10)
        self.line = wx.StaticLine(parent, -1, size = (self.gauge.GetSize()[0]*2 , 2))
        self.Add(self.sizer1, 0)
        self.AddSpacer(10)
        self.Add(self.line)
        
    
    def SetTitle(self, title):
        self.vmeNameStaticText.SetLabel(title)
    
    def SetEndingLabel(self, label):
        self.endingLabel.SetLabel(label)
        
    def SetRemainingTimeLabel(self, label):
        self.remainingTimeLabel.SetLabel(label)

def test():
    app = wx.PySimpleApp()    
    frame = wx.Frame(None, -1)
    gaugeContainer = CustomGaugeWx(frame, pos = (100,100),gaugeModality ="DOWNLOAD")
    #gaugeContainer.gauge.SetValue(50)
    frame.Show(True)
    app.MainLoop()

if __name__ == '__main__':
  test()