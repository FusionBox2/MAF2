import vmeUploader
from webServicesClient import MtomUpload
import msfParser
import os, sys, string, time, re ,shutil
import threading

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
        self.sendBinaryFile()
        self.sendXMLFile()
		
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

        upl.Upload()

        #launch external XML editor
        self.launchXMLEditor(self.dirOutgoing)

        #get free resource
        uri = self.getFreeResource()

        #send file
        self.sendBinaryFile()

        #modify xml with URI
        self.uriXMLSubstitution(uri)

        #send xml file
        self.sendXMLFile()

        print "Wainting..."
        while 1:
            # To simulate asynchronous I/O, we create a random number at
            # random intervals. Replace the following 2 lines with the real
            # thing.
            time.sleep(0.3)
            self.msg = self.msg + 1
            lista = [self.observer,self.msg]
            self.block.acquire()
            UploadHandler.queue.put(lista)
            self.block.release()
            if(self.msg == 100): break

    def launchXMLEditor(self, dir):
        print str(dir)
        files = os.listdir(dir)
        print files
        for file in files:
            if (re.search('\\.xml$',file)):
               xmlFile = file

        oldDir = os.getcwd()
        os.chdir("\"C:\\Program Files\\Peter's XML Editor\\")
        #print os.getcwd()
        command = "\"" + dir + "\\" +  xmlFile + "\""
        command = "pxe.exe " + command
        #print command
        os.system(command)
        os.chdir(oldDir)

    def getFreeResource(self):
        #here call module to get URI of first free resource
        return "test"

    def sendBinaryFile(self):
        files = os.listdir(self.dirOutgoing)
        print files
        binaryFile = ""
        for file in files:
            if (re.search('\\.xml$',file) == None):
               binaryFile = file
        #assert(binaryFile)
        print self.dirOutgoing + "\\" + binaryFile
        self.__sendFile(self.dirOutgoing + "\\" + binaryFile)
		
    def sendXMLFile(self):
        files = os.listdir(self.dirOutgoing)
        print files
        xmlFile = ""
        for file in files:
            if (re.search('\\.xml$',file)):
               xmlFile = file
        #assert(xmlFile)
        print self.dirOutgoing + "\\" + xmlFile
        self.__sendFile(self.dirOutgoing + "\\" + xmlFile)
	
    def uriXMLSubstitution(self, realURI):
        #substitute in xml uri value
        pass	

    def __sendFile(self,filename):
        instance = MtomUpload.MtomUpload()
        print "sendFile " + filename
        result = instance.Upload(filename)
        cheksum = result.chksum
        uri = result.uriFile
        pass
		
		
def createUploadHandler(queue, observer, dirCache, id):
    uploadHandler = UploadHandler(queue,observer, dirCache, id)
    uploadHandler.upload()
    
if __name__ == '__main__':
  createUploadHandler()
