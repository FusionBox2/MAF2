import vmeUploader
from webServicesClient import MtomUpload
import msfParser
from xml.dom import minidom
from xml.dom import Node
import os, sys, string, time, re ,shutil
import threading, thread

class UploadHandler:
    queue = None
    def __init__(self, queue, observer , dirCache, id):
        UploadHandler.queue = queue
        self.observer = observer
        self.dirCache = dirCache
        self.dirOutgoing = ""
        self.id = id	
        self.msg = 0
        self.block = threading.Lock()
		
    def upload(self):
        self.createXMLAndBinary()
        
        #launch external XML editor
        self.launchXMLEditor(self.dirOutgoing)

        #send file
        #thread.start_new_thread(self.sendBinaryFile,())
        self.sendBinaryFile() #until there is service monitor don't use thread

        print "Wainting..."
        #while 1:
            # To simulate asynchronous I/O, we create a random number at
            # random intervals. Replace the following 2 lines with the real
            # thing.
        #    time.sleep(0.2)
        #    self.msg = self.msg + 20
        self.msg = 100
        lista = [self.observer,self.msg]
        self.block.acquire()
        UploadHandler.queue.put(lista)
        self.block.release()
        #    if(self.msg == 100): break

        #send xml file
        self.sendXMLFile()
        
        print "End Upload" 

		
    def createXMLAndBinary(self):
        curDir = sys.path[0]
        upl = vmeUploader.vmeUploader()
        upl.InputMSFDirectory = self.dirCache
        upl.HandledAutoTagsListFileName = curDir + r'\handledAutoTagsList.txt'
        upl.UnhandledPlusManualTagsListFileName = curDir + r'\unhandledPlusManualTagsList.txt'
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
        uri = self.getFreeResource()
        
        upl.DatasetURI = uri

        upl.Upload()

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
        return "DaQuaPrendoURI"

    def sendBinaryFile(self):
        files = os.listdir(self.dirOutgoing)
        #print files
        binaryFile = ""
        for file in files:
            if (re.search('\\.xml$',file) == None):
               binaryFile = file
        #assert(binaryFile)
        #print self.dirOutgoing + "\\" + binaryFile
        self.__sendFile(self.dirOutgoing + "\\" + binaryFile)
		
    def sendXMLFile(self):
        self.__sendFile(self.dirOutgoing + "\\" + self.getXMLFile())

    def __sendFile(self,filename):
        instance = MtomUpload.MtomUpload()
        print filename
        result = instance.Upload(filename)
        cheksum = result.chksum
        uri = result.uriFile
        pass

    def getXMLFile(self):
        files = os.listdir(self.dirOutgoing)
        #print files
        xmlFile = ""
        for file in files:
            if (re.search('\\.xml$',file)):
               xmlFile = file
        return xmlFile	
		
def createUploadHandler(queue, observer, dirCache, id):
    uploadHandler = UploadHandler(queue,observer, dirCache, id)
    uploadHandler.upload()
    
if __name__ == '__main__':
  createUploadHandler()
