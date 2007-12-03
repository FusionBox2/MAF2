import wx
import sys

class CustomGaugeWx(wx.BoxSizer):
    def __init__(self, parent, id = -1,  range = 100, pos = wx.DefaultPosition, size=wx.DefaultSize, style= wx.GA_HORIZONTAL, validator=wx.DefaultValidator, name=wx.GaugeNameStr, title="Valori" , orient = wx.VERTICAL,gaugeType = "U"):
        wx.BoxSizer.__init__(self,orient)
        self.gaugeType = gaugeType #bar is for upload "U" or download "D" 
        
        self.sizer1 = wx.BoxSizer(wx.HORIZONTAL)
        self.label = wx.StaticText(parent, -1, label = title)
        self.gauge = wx.Gauge(parent, id, range, pos, size, style, validator, name)
        
        png = wx.Image(sys.path[0] + "/bitmaps/up.png", wx.BITMAP_TYPE_PNG).ConvertToBitmap()
        self.image = wx.StaticBitmap(parent, -1, png, size = (png.GetWidth(), png.GetHeight()))
        
        self.sizer1.Add(self.label, 2, wx.ALIGN_LEFT | wx.ALIGN_CENTRE_VERTICAL)
        self.sizer1.Add(self.image, 0, wx.ALIGN_CENTER | wx.ALIGN_CENTRE_VERTICAL)
        self.sizer1.Add(self.gauge, 3, wx.ALIGN_RIGHT)
        
        self.AddSpacer(5)
        self.line = wx.StaticLine(parent, -1, size = (self.gauge.GetSize()[0]*2 , 2))
        self.Add(self.sizer1, 3 , wx.ALIGN_CENTER)
        self.AddSpacer(5)
        self.Add(self.line, 0 , wx.ALIGN_CENTER)
        self.AddSpacer(5)
        
    
    def SetTitle(self, title):
        self.label.SetLabel(title)

def test():
    app = wx.PySimpleApp()    
    frame = wx.Frame(None, -1)
    gaugeContainer = CustomGauge(frame, pos = (100,100))
    gaugeContainer.gauge.SetValue(50)
    frame.Show(True)
    app.MainLoop()

if __name__ == '__main__':
  test()