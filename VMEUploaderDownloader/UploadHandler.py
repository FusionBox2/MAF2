import vmeUploader
from webServicesClient import MtomUpload, MtomUploadURI, MtomSRBSize , xmlrpcDemoWS
import msfParser
from xml.dom import minidom
from xml.dom import Node
import os, sys, string, time, re ,shutil
import threading, thread
from Debug import Debug

class UploadHandler:
    queue = None
    def __init__(self, queue, observer , dirCache, id):
        UploadHandler.queue = queue
        self.observer = observer
        self.dirCache = dirCache
        self.dirOutgoing = ""
        self.id = id	
        self.msg = 0
        self.binaryFileSize = 0
        self.remoteTemporaryBinaryFileSize = 0
        self.BinaryURI = ""
        self.XMLURI = ""
        self.block = threading.Lock()
		
    def upload(self):
        self.createXMLAndBinary()
        
        #launch external XML editor
        if Debug:
            self.launchXMLEditor(self.dirOutgoing)

        #self.remoteTemporaryBinaryFileSize = self.getRemoteTemporaryBinaryFileSize()
        #send file
        thread.start_new_thread(self.sendBinaryFile,())      
        #self.sendBinaryFile() #until there is service monitor don't use thread
        
        print "Wainting..."
        print "Total Size of Binary: " + str(self.binaryFileSize)
        while 1:
            # To simulate asynchronous I/O, we create a random number at
            # random intervals. Replace the following 2 lines with the real
            # thing.
            time.sleep(0.5)
            self.remoteTemporaryBinaryFileSize = self.getRemoteTemporaryBinaryFileSize()
            percentage = 100 * self.remoteTemporaryBinaryFileSize / self.binaryFileSize
            lista = [self.observer,percentage]
            self.block.acquire()
            UploadHandler.queue.put(lista)
            self.block.release()
            if(percentage == 100): break

        #send xml file, perhaps here free source
        self.sendXMLFile()
        
        print "End Upload"
        print "Uploaded Binary in SRB: " + self.BinaryURI
        print "Uploaded XML on Biomedtown: " + self.XMLURI

		
    def createXMLAndBinary(self):
        curDir = sys.path[0]
        upl = vmeUploader.vmeUploader()
        upl.InputMSFDirectory = self.dirCache
        upl.HandledAutoTagsListFileName = curDir + r'\handledAutoTagsList.csv'
        upl.UnhandledPlusManualTagsListFileName = curDir + r'\unhandledPlusManualTagsList.csv'
        upl.OutputFolderName = curDir + r'\Outgoing'
        
        count = 0
        directory = upl.OutputFolderName + '\\' + str(count)
		
        while(os.path.exists(directory)):
           count = count + 1
           directory = upl.OutputFolderName + '\\' + str(count)
        
        self.dirOutgoing = directory
        upl.OutputFolderName = directory
        upl.VmeToExtractID = int(self.id)

        #get free resource (return URI string)
        self.BinaryURI = self.getFreeResource() 
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
        result = instance.ListSrbDir()
        return result

    def sendBinaryFile(self):
        os.rename(self.dirOutgoing + "\\" + self.getBinaryFile(),self.dirOutgoing + "\\" +self.BinaryURI)
        self.__sendFile(self.BinaryURI)
		
    def sendXMLFile(self):
        self.XMLURI = self.BinaryURI + "_" +self.getXMLFile()
        os.rename(self.dirOutgoing + "\\" + self.getXMLFile(),self.dirOutgoing + "\\" + self.XMLURI)
        #self.__sendFile(self.XMLURI)
        oldDir = os.getcwd()
        os.chdir(self.dirOutgoing)
        
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        out = ws.run('xmlupload', self.XMLURI)
        
        os.chdir(oldDir)

    def __sendFile(self,filename):
        oldDir = os.getcwd()
        os.chdir(self.dirOutgoing)
        
        instance = MtomUpload.MtomUpload()
        result = instance.Upload(filename)
        
        cheksum = result.chksum
        uri = result.uriFile
        
        os.chdir(oldDir)
        
        
    def getRemoteTemporaryBinaryFileSize(self):
        instance = MtomSRBSize.MtomSize()
        result = instance.ListSrbDir(self.BinaryURI)
        return result

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
		
def createUploadHandler(queue, observer, dirCache, id):
    uploadHandler = UploadHandler(queue,observer, dirCache, id)
    uploadHandler.upload()
    
if __name__ == '__main__':
  createUploadHandler()
