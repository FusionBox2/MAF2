import vmeDownloader
from webServicesClient import MtomDownload
import DownloadHandler
import os, sys, string, time, re ,shutil
from stat import ST_SIZE 
import threading, thread, CustomThread
from lhpDefines import *
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
        self.proxyHost = ""
        self.proxyPort = 0
        pass
    
    def __download(self):
        #self.retrieveConnectionsParameters()
        self.proxyHost, self.proxyPort = retriveProxyParameters()

        oldDir = os.getcwd()
        if(self.dirCache != None):
           os.chdir(self.dirCache)
        print os.getcwd()

        serviceUrl = 'https://ws-lhdl.cineca.it/mafSRBDownload.cgi'
        mtomD = MtomDownload.MtomDownload()

        print "->"+ self.proxyHost + "<-"
        print "->"+ str(self.proxyPort) + "<-"

        mtomD.Download(self.srbData,serviceUrl,self.proxyHost,self.proxyPort)
        print "Inside Download Thread"
        
        os.chdir(oldDir)    
        
        pass
    
    """def retrieveConnectionsParameters(self):
        #proxy host
        #proxy port
        try:
            print "FOUND"
            file = open("vmeUploaderConnectionConfiguration.conf","r")
            self.proxyHost = file.readline() # proxy host
            self.proxyPort = file.readline()  # proxy port
            file.close()
        except:
            print "NOT FOUND"
            self.proxyHost = ""
            self.proxyPort = 0
            pass
        pass"""
    
    def controlLocalFileDimension(self):
        if(os.path.exists(self.dirCache+self.srbData)):
            return os.stat(self.dirCache+self.srbData).st_size
        else:
            return 0
        
    
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
        
        if(self.srbData != "."):
            thread.start_new_thread(self.__download,()) #here start download thread
            percentage = -1
            while(1):
              percentage = 100 * float(self.controlLocalFileDimension())/float(self.fileSize)
              print percentage
              time.sleep(0.3)
              self.block.acquire()
              if(DownloadHandler.queue):
                  lista = [self.observer,percentage]
                  DownloadHandler.queue.put(lista)
              self.block.release()
              if(percentage >= 100):
                  break
            
            print str(self.controlLocalFileDimension())
            self.moveFileInMSFDirectory()
        else:
            if(DownloadHandler.queue):
                percentage = 100
                lista = [self.observer,percentage]
                DownloadHandler.queue.put(lista)              
        
        pass

                      
def createDownloadHandler(queue, observer, dirCache, srbData , usr , pwd, urlServer, fileSize):
    downloadHandler = DownloadHandler(queue, observer, dirCache, srbData , usr , pwd, urlServer, fileSize)
    downloadHandler.download()
    
if __name__ == '__main__':
  createDownloadHandler(None, None, None, None, None, None, None, None)
