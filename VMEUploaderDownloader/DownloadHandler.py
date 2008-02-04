import vmeDownloader
import os, sys, string, time, re ,shutil
import threading, thread, CustomThread
#from webServicesClient import MtomDownload
from Debug import Debug


class DownloadHandler:
    queue = None
    def __init__(self, queue, observer, dirCache, srbData , usr , pwd, urlServer, fileSize):
        DownloadHandler.queue = queue
        self.observer = observer
        self.dirCache = dirCache
        self.srbData = srbData
        self.currentUser = usr
        self.currentPassword = pwd
        self.urlServer = urlServer
        self.fileSize = fileSize
        self.block = threading.Lock()
        
        #test
        self.count = 0
    
    def Testfunction(self):
        while(self.count < 100):
            self.count = self.count +0.001
            
    def __download(self):
        oldDir = os.getcwd()
        os.chdir(self.dirCache)
        print os.getcwd()
        
        #mtomD = MtomDownload.MtomDownload()
        #mtomD.Download(self.fileSize)
        
        #for now create a fake file to simulate the download      
        file = open(self.srbData,"w")
        file.close()
        
        os.chdir(oldDir)    
        
        pass
    
    def controlLocalFileDimension(self):
        size = 0
        size = os.stat(self.srbData)[6]
        return size
    
    def moveFileInMSFDirectory(self):
        print "Move file in current MSF dir"
        #rename file, reading information by conf file
        try:
          file = open(self.dirCache+"configuration.conf","r")
        except:
          print "Unable to Open ConfigurationFile"
          return
        
        fullPathInMSF = file.read()
        file.close()
        print "move " + self.dirCache+self.srbData + " in " + fullPathInMSF
        
        if(os.path.exists(fullPathInMSF)):
            os.remove(fullPathInMSF)
        
        shutil.move(self.dirCache+self.srbData, fullPathInMSF)
        pass
        
    def download(self):
        print "DownloadHandler inside download:"
        #print self.observer
        #print self.dirCache
        #print self.srbData
        #print self.currentUser
        #print self.currentPassword
        #print self.urlServer
        #print self.fileSize
        self.__download()
        #thread.start_new_thread(self.__download,()) #here start download thread
        thread.start_new_thread(self.Testfunction,())
        #print self.controlLocalFileDimension()
        percentage = -1
        while(self.count < 100):
          #percentage = float(self.controlLocalFileDimension())/self.fileSize
          
          #print self.count
          time.sleep(0.1)
          self.block.acquire()
          if(DownloadHandler.queue):
              lista = [self.observer,self.count]
              DownloadHandler.queue.put(lista)
          self.block.release()
        
        self.moveFileInMSFDirectory()
        
        pass
                      
def createDownloadHandler(queue, observer, dirCache, srbData , usr , pwd, urlServer, fileSize):
    downloadHandler = DownloadHandler(queue, observer, dirCache, srbData , usr , pwd, urlServer, fileSize)
    downloadHandler.download()
    
if __name__ == '__main__':
  createDownloadHandler(None, None, None, None, None, None, None, None)
