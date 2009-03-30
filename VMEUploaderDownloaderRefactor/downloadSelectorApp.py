# -*- coding: iso-8859-1 -*-
import wx
import sys

provider = wx.SimpleHelpProvider()
wx.HelpProvider_Set(provider)

import downloadSelectorFrame


class App(wx.App):
    def OnInit(self):
        wx.InitAllImageHandlers()
        self.user = sys.argv[0]
        self.password = sys.argv[1]
        self.fromSandbox = sys.argv[2]
        
        if  int(self.fromSandbox) == 0:
            self.main = downloadSelectorFrame.downloadSelectorFrame(None,-1,title='Download from Basket of ' + self.user ,app = self)
        else:
            self.main = downloadSelectorFrame.downloadSelectorFrame(None,-1,title='Download from Sandbox of ' + self.user ,app = self)
            
        self.main.Show()
        self.SetTopWindow(self.main)
        return 1

def main():
    usage_msg = '''Usage: %s user, password, fromBasket''' % sys.argv[0]
    if(len(sys.argv) != 4):
       print 'Error :\n' + usage_msg
       sys.exit(1)
       
    if(len(sys.argv) == 4):
        sys.argv = sys.argv[1:]
        #print sys.argv
        application = App(0)
        application.MainLoop()
        application.ExitMainLoop()

if __name__ == '__main__':
    main()
