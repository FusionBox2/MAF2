import vmeUploader
from webServicesClient import MtomUpload, MtomUploadURI, MtomSRBSize , xmlrpcDemoWS
import msfParser
from xml.dom import minidom
from xml.dom import Node
import os, sys, string, time, re ,shutil
import threading, thread, CustomThread
from Debug import Debug

class UploadHandler:
    queue = None
    def __init__(self, queue, observer , dirCache, id, usr , pwd, urlServer, manualTagFile):
        UploadHandler.queue = queue
        self.observer = observer
        self.dirCache = dirCache
        self.dirOutgoing = ""
        self.id = id
        self.currentUser = usr
        self.currentPassword = pwd
        self.urlServer = urlServer
        self.manualTagFile = manualTagFile
        self.msg = 0
        self.binaryFileSize = 0
        self.remoteTemporaryBinaryFileSize = 0
        self.BinaryURI = ''
        self.XMLURI = ""
        self.block = threading.Lock()
        self.threads = []
        self.existThread = 0
            
    def upload(self):
        self.createOutgoingDir()
               
        #get free resource (return URI string)
        self.BinaryURI = self.getFreeResource() #thread maybe
        #thread.start_new_thread(self.getFreeResource,())
        print self.BinaryURI
        #-1 percentage means progress pulsing 
        
        
        """
        percentage = -1
        while(1):
            lista = [self.observer,percentage]
            #print self.BinaryURI
            time.sleep(0.1)
            self.block.acquire()
            print "Testing  " + self.BinaryURI
            UploadHandler.queue.put(lista)
            self.block.release()
            if(self.BinaryURI != ''):
               break
        return #used for test
        """

        """
        if(self.BinaryURI == \"Services not available!!\"):
            print "Connection Problems..."
            return
        """

        self.createXMLAndBinary()
        
        #launch external XML editor
        if Debug:
            self.launchXMLEditor(self.dirOutgoing)

        #self.remoteTemporaryBinaryFileSize = self.getRemoteTemporaryBinaryFileSize()
        #send file
        thread.start_new_thread(self.sendBinaryFile,())      
        #self.sendBinaryFile() #until there is service monitor don't use thread
        
        
        
        print "Wainting for sending binary..."
        print "Total Size of Binary: " + str(self.binaryFileSize)
        binarySendResult = False
        oldPercentage = -1
        percentage = 0
        
        countTime = 0
        timeStep = 1.0
        while 1:
            # To simulate asynchronous I/O, we create a random number at
            # random intervals. Replace the following 2 lines with the real
            # thing.
            time.sleep(2.0)
            #if(countTime >= 1.0): countTime = 0;
            #else:
            #    countTime += 0.1
            #    continue
            
            if(self.existThread == 0):
               thread.start_new_thread(self.getRemoteTemporaryBinaryFileSize,())
            
            percentage = 100 * self.remoteTemporaryBinaryFileSize / self.binaryFileSize
            
            if(percentage == oldPercentage): continue
            oldPercentage = percentage
            
            
            print "percentage " + str(percentage) 
            lista = [self.observer,percentage]
            print "bytes: " + str(self.remoteTemporaryBinaryFileSize)
            print "p: " + str(percentage)
            self.block.acquire()
            UploadHandler.queue.put(lista)
            self.block.release()
            if(percentage >= 100):
                binarySendResult = True
                break
           
        #send xml file, perhaps here free source
        if(binarySendResult == True):
          print "Waiting for sending XML..."
          self.sendXMLFile()
        
          print "End Upload"
          print "Uploaded Binary in SRB: " + self.BinaryURI
          print "Uploaded XML on Biomedtown: " + self.XMLURI
          print "Uploaded by: " + self.currentUser
          print "In server url: " + self.urlServer
        
        else:
          print "Upload Error on Binary"
          
    def createOutgoingDir(self):
        curDir = sys.path[0]
        count = 0
        self.dirOutgoing = curDir + r'\Outgoing'
        directory = self.dirOutgoing + '\\' + str(count)
       
        while(os.path.exists(directory)):
           count = count + 1
           directory = self.dirOutgoing + '\\' + str(count)
        self.dirOutgoing = directory
        os.mkdir(directory)
        
        		
    def createXMLAndBinary(self):
        curDir = sys.path[0]
        upl = vmeUploader.vmeUploader()
        upl.InputMSFDirectory = self.dirCache
        upl.HandledAutoTagsListFileName = curDir + r'\handledAutoTagsList.csv'
        upl.UnhandledPlusManualTagsListFileName = curDir + '\\' + self.manualTagFile 
      
        upl.OutputFolderName = self.dirOutgoing
        upl.VmeToExtractID = int(self.id)
        upl.DatasetURI = self.BinaryURI

        upl.Upload()
        #get size of binary locally
        self.binaryFileSize = self.getBinaryFileSize()

    def launchXMLEditor(self, dir):
        oldDir = os.getcwd()
        os.chdir("\"C:\\Program Files\\Peter's XML Editor\\")
        #print os.getcwd()
        command = "\"" + dir + "\\" +  self.getXMLFile() + "\""
        command = "pxe.exe " + command
        #print command
        os.system(command)
        os.chdir(oldDir)

    def getFreeResource(self):
        #here call module to get URI of first free resource
        instance = MtomUploadURI.MtomUploadURI()
        serviceUrl = 'https://ws-lhdl.cineca.it/mafSRBUploadURI.cgi'        
        return instance.ListSrbDir(serviceUrl)
        #print 'Inside FreeResource Thread ' + self.BinaryURI


    def sendBinaryFile(self):
        os.rename(self.dirOutgoing + "\\" + self.getBinaryFile(),self.dirOutgoing + "\\" +self.BinaryURI)
        self.__sendFile(self.BinaryURI)
        print "Sending Thread Finished"
		
    def sendXMLFile(self):
        self.XMLURI = self.BinaryURI + "_" +self.getXMLFile()
        os.rename(self.dirOutgoing + "\\" + self.getXMLFile(),self.dirOutgoing + "\\" + self.XMLURI)
        #self.__sendFile(self.XMLURI)
        oldDir = os.getcwd()
        os.chdir(self.dirOutgoing)
        
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setCredentials(self.currentUser, self.currentPassword)
        ws.setServer(self.urlServer)
        out = ws.run('xmlupload', self.XMLURI)
        
        os.chdir(oldDir)

    def __sendFile(self,filename):
        oldDir = os.getcwd()
        os.chdir(self.dirOutgoing)
        
        instance = MtomUpload.MtomUpload()
        result = instance.Upload(filename,'https://ws-lhdl.cineca.it/mafSRBUpload.cgi')
        
        cheksum = result.chksum
        uri = result.uriFile
        
        os.chdir(oldDir)
        
        
    def getRemoteTemporaryBinaryFileSize(self):
        self.block.acquire()
        self.existThread = 1
        self.block.release()
        result = None
        try:
            instance = MtomSRBSize.MtomSize()
            serviceUrl = 'https://ws-lhdl.cineca.it/mafSRBSize.cgi'
            result = instance.ListSrbDir(self.BinaryURI, serviceUrl)
            self.remoteTemporaryBinaryFileSize = result;
            print "SIZE Thread Finished "
        except:
            pass
        self.block.acquire()
        self.existThread = 0
        self.block.release() 
        #return result;
        

    def getXMLFile(self):
        files = os.listdir(self.dirOutgoing)
        #print files
        xmlFile = ""
        for file in files:
            if (re.search('\\.xml$',file)):
               xmlFile = file
        return xmlFile
    
    def getBinaryFile(self):
        files = os.listdir(self.dirOutgoing)
        #print files
        binaryFile = ""
        for file in files:
            if (re.search('\\.xml$',file) == None):
               binaryFile = file
        return binaryFile
    
    def getBinaryFileSize(self):
        return os.stat(self.dirOutgoing + "\\" +self.getBinaryFile()).st_size
		
def createUploadHandler(queue, observer, dirCache, id , usr , pwd, urlServer, manualTagFile):
    uploadHandler = UploadHandler(queue,observer, dirCache, id, usr , pwd, urlServer, manualTagFile)
    uploadHandler.upload()
    
if __name__ == '__main__':
  createUploadHandler()
