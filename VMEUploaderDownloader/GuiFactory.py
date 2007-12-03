import GuiWx
import GuiTk

class GuiFactory:
      def __init__(self):
          self.guis = {}
          self.fillTable()

      def register(self, string, gui):
          self.guis[string] = gui

      def fillTable(self):
          self.register("wx", GuiWx.GuiPart)
          self.register("tk", GuiTk.GuiPart)

      def createGui(self, string):
          return self.guis[string]

def testWx():
    import wx , Queue
    def endFunction(): print "endingWx"
    app = wx.PySimpleApp()
    guiFactory = GuiFactory()
    gui = (guiFactory.createGui("wx"))(None , Queue.Queue(), endFunction)
    app.MainLoop()

def testTk():
    import Tkinter, Queue
    def endFunction(): print "endingTk"
    root = Tkinter.Tk()
    guiFactory = GuiFactory()
    gui = (guiFactory.createGui("tk"))(root , Queue.Queue(), endFunction)
    root.mainloop()

if __name__ == '__main__':
  testTk()
  testWx()