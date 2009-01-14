import vmeDownloader
from webServicesClient import MtomDownload
import os, sys, string, time, re ,shutil
import hashlib
import msvcrt
import xml.dom.minidom as xd
from stat import ST_SIZE 
import threading, thread, CustomThread
from lhpDefines import *
from Debug import Debug
import wx
import time

class DownloadHandler:
    queue = None
    def __init__(self, queue, observer, dirCache, srbData, usr , pwd, urlServer, isLast, fileSize):
        DownloadHandler.queue = queue
        self.observer = observer
        self.dirCache = dirCache
        self.srbData = srbData
        self.currentUser = usr
        self.currentPassword = pwd
        self.urlServer = urlServer
        self.fileSize = fileSize
        self.isLast = isLast
        self.block = threading.Lock()
        self.localChksum = ""
        self.remoteChksum = ""
        self.proxyHost = ""
        self.proxyPort = 0
        pass
    
    def __download(self):
        #self.retrieveConnectionsParameters()
        self.proxyHost, self.proxyPort = retriveProxyParameters()

        oldDir = os.getcwd()
        if(self.dirCache != None):
           os.chdir(self.dirCache)
        if Debug:
            print os.getcwd()

        serviceUrl = 'https://ws-lhdl.cineca.it/mafSRBDownload.cgi'
        mtomD = MtomDownload.MtomDownload()

        if Debug:
            print "->"+ self.proxyHost + "<-"
            print "->"+ str(self.proxyPort) + "<-"
         
        try:
            mtomD.Download(self.srbData,serviceUrl,self.proxyHost,self.proxyPort)
            if Debug:
                print "Inside Download Thread"
        except:
            if Debug:
                print "-------Error calling mafSRBDownload.cgi----------" 
            self.block.acquire()
            if(DownloadHandler.queue):
                percentage = 120 #120 for 'Error!'           
                lista = [self.observer,percentage]
                DownloadHandler.queue.put(lista)
            self.block.release()
            return
        
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
            if Debug:
                print "Move file in current MSF dir"
            #rename file, reading information by conf file
            try:
              file = open(self.dirCache+"configuration.conf","r")
            except:
              if Debug:
                  print "------Unable to Open ConfigurationFile-----------"
              return
            
            fullPathInMSF = file.read()
            file.close()
            if Debug:
                print "move " + self.dirCache+self.srbData + " in " + fullPathInMSF
            
            if(os.path.exists(fullPathInMSF)):
                os.remove(fullPathInMSF)
                
            pos = fullPathInMSF.rfind('/')
            binaryname = fullPathInMSF[pos:]
            os.rename(self.dirCache+self.srbData, self.dirCache+binaryname)
            pass
        
    def download(self):
        if Debug:
            print "DownloadHandler inside download:"
        
        if(self.srbData != "."):
            thread.start_new_thread(self.__download,()) #here start download thread
            percentage = -1
            previousTime = time.time()
            previousFileDimension = 0
            totalDataDownloaded = 0
            totalTime = 0
            lastSpeed = 0
            
            while(1):
              currentFileDimension = self.controlLocalFileDimension()
              percentage = 100 * float(currentFileDimension)/float(self.fileSize)
              
              currentTime = time.time()
              elapsedTime = currentTime - previousTime
              transferredDimension = currentFileDimension-previousFileDimension
              totalDataDownloaded = totalDataDownloaded + transferredDimension                 
              totalTime = totalTime + elapsedTime
              remainingTime = "unknown"
              remainingData = float(self.fileSize) - float(totalDataDownloaded)
              
              if elapsedTime != 0:
                 lastSpeed = transferredDimension / elapsedTime 
           
              previousTime = currentTime
              previousFileDimension = currentFileDimension
                
              if lastSpeed != 0:
                 remainingTime = remainingData / lastSpeed                     
              
              if Debug:
                  print  ""
                  print  "########################################"
                  print "in time: "  + str(elapsedTime) 
                  
                  print "transferred: " + str(transferredDimension)
                  print ""
                  
                  print "percent transferred:" + str(percentage)  + "%"
                  
                  print "and data transferred: " + str(totalDataDownloaded) 
                  
                  print "in total time: " + str(totalTime)
                                    
                  print str(remainingData) + " remaining"
                  
                  print ""
                  
                  if elapsedTime != 0:
                    print "last speed: " + str(lastSpeed)
                  else:
                    print "last speed: unknown"  
                  
                
                  if lastSpeed != 0:
                     print "remaining time: " + str(remainingTime)                                          
                  else:
                     print "remaining time: unknown"
                     
                    
                  print  "########################################"
                  
              time.sleep(0.3)
              self.block.acquire()
              if(DownloadHandler.queue):
                  lista = [self.observer,percentage, remainingTime]
                  DownloadHandler.queue.put(lista)
              self.block.release()
              if(percentage >= 100):
                  break
            
            if Debug:
                print str(self.controlLocalFileDimension())

            #-----MD5 check-point-----#
            self.localChksum = self.md5(self.dirCache+self.srbData)
            if Debug:
                print "Local checksum=  " + self.localChksum
            
            try:
                self.remoteChksum = self.retrieveTagValue('L0000_resource_data_Dataset_LocalFileCheckSum')
            except:
                if Debug:
                    print "------------Checksum not found------------"
                self.block.acquire()
                if(DownloadHandler.queue):
                    percentage = 120 #110 for 'Error!'
                    lista = [self.observer,percentage]
                    DownloadHandler.queue.put(lista)
                self.block.release()
                return
            self.remoteChksum = self.remoteChksum.lower()
            if Debug:
                print "Remote checksum= " + self.remoteChksum
            
            if (self.localChksum == self.remoteChksum):
                if Debug:
                    print " "
                    print "MD5 checksum control successful!"
                    print " "
                if(DownloadHandler.queue):
                    self.block.acquire()
                    percentage = 110 #110 for 'Completed!'
                    lista = [self.observer,percentage]
                    DownloadHandler.queue.put(lista)
                    self.block.release()
                    self.moveFileInMSFDirectory()    
   
            else:
                if Debug:
                    print " "
                    print "Error: MD5 checksum control unsuccessful!"
                    print " "
                
                if(DownloadHandler.queue):
                    self.block.acquire()
                    percentage = 120 #120 for 'Error!'
                    lista = [self.observer,percentage]
                    DownloadHandler.queue.put(lista)
                    self.block.release()                      
                return 
        else: 
            if(DownloadHandler.queue):
                self.block.acquire()
                percentage = 110 #110 for 'Completed!'
                lista = [self.observer,percentage]
                DownloadHandler.queue.put(lista)
                self.block.release() 
                
        if Debug:
            print "IsLast: " + str(self.isLast)
        if (self.isLast == "false"):
            #replace status for resource correctly downloaded
            if(os.path.exists(sys.path[0] + '\\status.txt')):
                  while 1:
                        size = os.path.getsize(sys.path[0] + '\\status.txt')
                        statusFile = open(sys.path[0] + '\\status.txt', 'a')
                        try:
                            msvcrt.locking(statusFile.fileno(), msvcrt.LK_RLCK, size)
                            statusFile.write('ended\n')
                            statusFile.close()  
                            break                                      
                        except:
                            counter = counter+1 #to avoid deadlock
                            statusFile.close()
                            if(counter == 10):
                                if Debug:
                                  print "----------Can not read in status.lhp-----------"
                                pass
                        
        else:
          if(os.path.exists(sys.path[0] + '\\status.txt')):
              while 1:
                    size = os.path.getsize(sys.path[0] + '\\status.txt')
                    statusFile = open(sys.path[0] + '\\status.txt', 'a')
                    try:
                        msvcrt.locking(statusFile.fileno(), msvcrt.LK_RLCK, size)
                        statusFile.write('lastEnded\n')
                        statusFile.close()  
                        break                                      
                    except:
                        counter = counter+1 #to avoid deadlock
                        statusFile.close()
                        if(counter == 10):
                            if Debug:
                              print "----------Can not read in status.lhp-----------"
                            pass
        
        processStarted = 0
        processEnded = 0
        startedCount = 0
        endedCount = 0
        lastStartedCount = 0
        lastEndedCount = 0
        lastStarted = False
        size = os.path.getsize(sys.path[0] + '\\status.txt')
        statusFile = open(sys.path[0] + '\\status.txt', 'r')
        try:
            msvcrt.locking(statusFile.fileno(), msvcrt.LK_RLCK, size)
            for line in statusFile:
                if line == "started\n":
                    startedCount += 1
                    if processStarted == 0:
                        processStarted += 1
                    if lastStarted == True:
                        processStarted += 1
                        #lastEnded = False
                        lastStarted = False
                elif line == "ended\n":
                    endedCount +=1   
                elif line == "lastStarted\n":
                    lastStarted = True
                    lastStartedCount += 1
                    if processStarted == 0:
                        processStarted += 1
                elif line == "lastEnded\n":
                    #lastEnded = True
                    lastEndedCount +=1
                    processEnded += 1
                  
                  
            statusFile.close()  
            
            if Debug:     
                print "processStarted :" + str(processStarted)     
                print "processEnded :" + str(processEnded)     
                print "startedCount :" + str(startedCount)     
                print "endedCount :" + str(endedCount)     
                print "lastStartedCount :" + str(lastStartedCount)     
                print "lastEndedCount :" + str(lastEndedCount)                   
            if processStarted == processEnded and startedCount == endedCount and lastStartedCount == lastEndedCount:
                allEnded = True

        except:
            counter = counter+1 #to avoid deadlock
            statusFile.close()
            if(counter == 10):
                if Debug:
                      print "----------Can not read in status.txt-----------"
                pass                    
        if allEnded == True:
            self.block.acquire()  
            percentage = 130 #130 for 'ALL COMPLETE!'
            lista = [self.observer,percentage]
            DownloadHandler.queue.put(lista)
            self.block.release()
            if(os.path.exists(sys.path[0] + '\\status.txt')):
                os.remove(sys.path[0] + '\\status.txt')
                if Debug:
                    print "status.txt removed"
    
        
    def md5(self,fileName):
        #Compute md5 hash of the specified file
        m = hashlib.md5()
        try:
            fd = open(fileName,"rb")
        except IOError:
            if Debug:
                print "Unable to open the file in readmode:", filename
            return
        content = fd.readlines()
        fd.close()
        for eachLine in content:
            m.update(eachLine)
        return m.hexdigest()
    
    def retrieveTagValue(self, tag):
        returnValue = ''
        try:
           file = open(self.dirCache + 'outputMAF.msf', 'r')
        except :
            return "Unable to open the file in readmode"
        dom = xd.parse(file)
        if dom.getElementsByTagName("fault"):
                return       
        for el in dom.getElementsByTagName('TItem'):
           if(el.attributes != None and el.attributes.get('Name')):
              attrNode = el.attributes.get('Name')
              attrValue = attrNode.nodeValue
              if(attrValue == tag):
                  childToFind = el.getElementsByTagName('TC')[0] # first element of childList
                  returnValue = childToFind.firstChild.nodeValue
        file.close()
        
        return returnValue


                      
def createDownloadHandler(queue, observer, dirCache, srbData, usr , pwd, urlServer, isLast, fileSize):
    downloadHandler = DownloadHandler(queue, observer, dirCache, srbData, usr, pwd, urlServer, isLast, fileSize)
    downloadHandler.download()
    
if __name__ == '__main__':
  createDownloadHandler(None, None, None, None, None, None, None, None)
