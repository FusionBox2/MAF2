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
import UploadHandler, DownloadHandler
from webServicesClient import xmlrpcDemoWS
from Debug import Debug
import xml.dom.minidom as xd
from lhpDefines import *
import thread
import Server
import Lock
import sys, os

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
        print "create server"
        #lock file
        self.lock = Lock.Lock(sys.path[0] + "\\activeLock.lhp")
        print "create lock"
        # Create the queue
        self.queue = Queue.Queue()

        # Set up the GUI part
        guiFactory = GuiFactory.GuiFactory()
        self.gui = (guiFactory.createGui(stringAppType))(master, self.queue, self.endApplication)
        print "create gui"
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
            self.periodicProcedure(100, self.periodicCall)
        
    def createThread(self, tuplaFromServer):
        #now is only implemented update
        #if tupla contain a flag for update or download it can be use the same structure
        #tuplaFromServer is
        #0 is modality (UPLOAD or DOWNLOAD)
        #1 is id for UPLOAD, SRB file size for DOWNLOAD 
        #2 is directory
        #3 is usr
        #4 is pwd
        #5 is serverUrl
        #6 is originalId in UPLOAD, SRB data URI in DOWNLOAD
        #7 is hasLink (UPLOAD)
        #8 is withChild (UPLOAD)
        #9 file for upload rollback (UPLOAD)
        #10 is XML resource URI (UPLOAD)
        #11 is vme name (UPLOAD)
        
        if(tuplaFromServer[0] == "UPLOAD"):
           self.createThreadForUpdate(tuplaFromServer)
        elif (tuplaFromServer[0] == "DOWNLOAD"):
           self.createThreadForDownload(tuplaFromServer)
           
        
    def createThreadForUpdate(self, tuplaFromServer):
        #tuplaFromServer is
        #0 is modality UPLOAD
        #1 is id, 
        #2 is directory
        #3 is usr
        #4 is pwd
        #5 is serverUrl
        #6 is originalId
        #7 is hasLink
        #8 is withChild
        #9 file for upload rollback (UPLOAD)        
        #10 is XML resource URI
        #11 is vme name
        
        self.gui.createBar(tuplaFromServer[0])
        self.gui.createLabel(tuplaFromServer[11]) #tupla[10] is vme name         
        self.threads.append(CustomThread.CustomThread(func=self.workerThreadUpload, args = (self.gui.bars[len(self.gui.bars)-1],tuplaFromServer[2], tuplaFromServer[1],tuplaFromServer[3],tuplaFromServer[4], tuplaFromServer[5], tuplaFromServer[6], tuplaFromServer[7], tuplaFromServer[8], tuplaFromServer[9], tuplaFromServer[10], tuplaFromServer[11])))
        self.threads[len(self.threads)-1].start()
    
    def createThreadForDownload(self, tuplaFromServer):
        #tuplaFromServer is
        #0 is modality DOWNLOAD)
        #1 is SRB file size
        #2 is directory
        #3 is usr
        #4 is pwd
        #5 is serverUrl
        #6 is is XML data URI 
        #7 is is SRB data URI 
        
        self.userName = tuplaFromServer[3]
        self.password = tuplaFromServer[4]
        vmeName = self.GetVmeName(tuplaFromServer[6])
        self.gui.createBar(tuplaFromServer[0])
        self.gui.createLabel(vmeName)    
               
        self.threads.append(CustomThread.CustomThread(func=self.workerThreadDownload, args = (self.gui.bars[len(self.gui.bars)-1],tuplaFromServer[2], tuplaFromServer[7], tuplaFromServer[3],tuplaFromServer[4],tuplaFromServer[5],tuplaFromServer[1])))
        self.threads[len(self.threads)-1].start()
        
    def workerThreadUpload(self, observer, dirCache , id, usr, pwd, urlServer, originalId, hasLink, withChild, msfListFile, XMLURI, vmeName):
        """
        This is where we handle the asynchronous I/O. For example, it may be
        a 'select()'.
        One important thing to remember is that the thread has to yield
        control.
        """
      
        try:
            UploadHandler.createUploadHandler(self.queue, observer, dirCache, id, usr, pwd, urlServer, originalId, hasLink, withChild, msfListFile, XMLURI, vmeName)
        except:
            pass
        
    def workerThreadDownload(self, observer, dirCache , srbData, usr, pwd, urlServer, filesize):
        """
        This is where we handle the asynchronous I/O. For example, it may be
        a 'select()'.
        One important thing to remember is that the thread has to yield
        control.
        """
        #try:
            #UploadHandler.createDownloadHandler(self.queue, observer, dirCache , id, usr, pwd, urlServer)
        #except:
        try:
            DownloadHandler.createDownloadHandler(self.queue ,observer, dirCache , srbData, usr, pwd, urlServer, filesize)
        except:
            pass
        
    def GetVmeName(self, XMLUri):
        self.proxyHost, self.proxyPort = retriveProxyParameters()
        if Debug:
            print "->"+ self.proxyHost + "<-"
            print "->"+ str(self.proxyPort) + "<-"
        
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setServer('https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2/' + XMLUri)
        ws.setCredentials(self.userName, self.password)
        ws.ProxyURL = self.proxyHost
        ws.ProxyPort = self.proxyPort
    
        if Debug:
            print "XML URI = " + XMLUri 
        out = ws.run('gettitle', XMLUri)[1]
        
        self.vmeName = []
        dom = xd.parseString(out)
        if dom.getElementsByTagName("fault"):
            print "------Error in gettitle service-------"
            for el in dom.getElementsByTagName("string"):
                for node in el.childNodes:  
                    error = node.data
            if Debug:
                print error
            
            #write a file used by builder to catch error 
            errorFile = open(os.getcwd() + '\\ErrorFound.lhp', 'w')
            errorFile.write('Error in gettitle service.\n')   
            errorFile.write(error)   
            errorFile.close()
            return
        for el in dom.getElementsByTagName("string"):
            for node in el.childNodes:  
                self.vmeName.append(node.data)            
        pass
        
        if Debug:
            print "VME Name: " + self.vmeName[0]
        return self.vmeName[0]
        

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