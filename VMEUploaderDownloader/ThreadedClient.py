"""
This recipe describes how to handle asynchronous I/O in an environment where
you are running Tkinter as the graphical user interface. Tkinter is safe
to use as long as all the graphics commands are handled in a single thread.
Since it is more efficient to make I/O channels to block and wait for something
to happen rather than poll at regular intervals, we want I/O to be handled
in separate threads. These can communicate in a threasafe way with the main,
GUI-oriented process through one or several queues. In this solution the GUI
still has to make a poll at a reasonable interval, to check if there is
something in the queue that needs processing. Other solutions are possible,
but they add a lot of complexity to the application.

Created by Jacob Hall?n, AB Strakt, Sweden. 2001-10-17
"""
import GuiFactory
import time
import CustomThread
import random
import Queue
import UploadHandler
import thread
import Server
import Lock
import sys

class ThreadedClient:
    """
    Launch the main part of the GUI and the worker thread. periodicCall and
    endApplication could reside in the GUI part, but putting them here
    means that you have all the thread controls in a single place.
    """
    def __init__(self, master, stringAppType, periodicProcedure):
        """
        Start the GUI and the asynchronous threads. We are in the main
        (original) thread of the application, which will later be used by
        the GUI. We spawn a new thread for the worker.
        """
        self.master = master
        self.port = int(sys.argv[1])
        self.threads = []
        self.periodicProcedure = periodicProcedure
        #create server
        thread.start_new_thread(Server.createServer,(self,self.port,1))

        #lock file
        self.lock = Lock.Lock(sys.path[0] + "\\activeLock.lhp")

        # Create the queue
        self.queue = Queue.Queue()

        # Set up the GUI part
        guiFactory = GuiFactory.GuiFactory()
        self.gui = (guiFactory.createGui(stringAppType))(master, self.queue, self.endApplication)

        # Set up the thread to do asynchronous I/O
        # More can be made if necessary
        

        # Start the periodic call in the GUI to check if the queue contains
        # anything
        self.periodicCall()

    def periodicCall(self):
        """
        Check every 100 ms if there is something new in the queue.
        """
        if(self.periodicProcedure):        
            self.gui.processIncoming()
            self.periodicProcedure(300, self.periodicCall)
        
    def createThread(self, tupla):
        #now is only implemented update
        #if tupla contain a flag for update or download it can be use the same structure
        self.createThreadForUpdate(tupla)
        
    def createThreadForUpdate(self, tupla):
        self.gui.createBar()
        self.gui.createLabel(tupla[2]) #tupla[2] is vme name
        self.threads.append(CustomThread.CustomThread(func=self.workerThreadUpload, args = (self.gui.bars[len(self.gui.bars)-1],tupla[1], tupla[0])))
        self.threads[len(self.threads)-1].start()
    
    def createThreadForDownload(self, tupla):
        pass
        
    def workerThreadUpload(self, observer, dirCache , id):
        """
        This is where we handle the asynchronous I/O. For example, it may be
        a 'select()'.
        One important thing to remember is that the thread has to yield
        control.
        """
        try:
            UploadHandler.createUploadHandler(self.queue, observer, dirCache , id)
        except:
            pass
        

    def endApplication(self):
        #delete file
        self.lock.deleteLockFile()

    def __del__(self):
        self.endApplication()
        
def test(stringAppType):
  try:
      if(stringAppType == "wx"):
          import wx
          app = wx.PySimpleApp()
          client = ThreadedClient(app , stringAppType, None)
          app.MainLoop()
      elif(stringAppType == "tk"):
          import Tkinter
          root = Tkinter.Tk()
          client = ThreadedClient(root ,stringAppType, root.after)
          root.mainloop()
  except:
      pass


if __name__ == '__main__':
  test("wx")