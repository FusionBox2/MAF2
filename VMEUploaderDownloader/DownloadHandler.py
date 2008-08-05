import vmeDownloader
from webServicesClient import MtomDownload
import DownloadHandler
import os, sys, string, time, re ,shutil
import hashlib
import xml.dom.minidom as xd
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
            
            
            #-----MD5 chek-point-----#
            self.localChksum = self.md5(self.dirCache+self.srbData)
            print "Loocal checksum= " + self.localChksum
            time.sleep(2)
            
            self.remoteChksum = self.retrieveTagValue('L0000_resource_data_Dataset_LocalFileCheckSum')
            self.remoteChksum = self.remoteChksum.lower()
            print "remote checksum= " + self.remoteChksum
            time.sleep(2)
            
            if (self.localChksum == self.remoteChksum):
                print " "
                print "MD5 chkesum control successful!"
                print " "
                self.moveFileInMSFDirectory()
            else:
                print " "
                print "Error: MD5 chkesum control unsuccessful!"
                print " "
                time.sleep(5)
                return
                
                
        else:
            if(DownloadHandler.queue):
                percentage = 100
                lista = [self.observer,percentage]
                DownloadHandler.queue.put(lista)              
        
        pass
        
    def md5(self,fileName):
        #Compute md5 hash of the specified file
        m = hashlib.md5()
        try:
            fd = open(fileName,"rb")
        except IOError:
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


                      
def createDownloadHandler(queue, observer, dirCache, srbData , usr , pwd, urlServer, fileSize):
    downloadHandler = DownloadHandler(queue, observer, dirCache, srbData , usr , pwd, urlServer, fileSize)
    downloadHandler.download()
    
if __name__ == '__main__':
  createDownloadHandler(None, None, None, None, None, None, None, None)
