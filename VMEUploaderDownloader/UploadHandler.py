import vmeUploader
#import webServicesClient.MtomUpload
import msfParser
import os, sys, string, time, re
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
        upl.DictionaryFileName = curDir + r'\testDictionaries\testDictionary.txt'
        upl.OutputFolderName = curDir + r'\Outgoing\\'
        
        count = 0
        directory = upl.OutputFolderName + str(count)
		
        while(os.path.exists(directory)):
           count = count + 1
           directory = upl.OutputFolderName + str(count)
        
        self.dirOutgoing = directory
        upl.OutputFolderName = directory
        upl.VmeToExtractID = int(self.id)
        upl.Upload()
		
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
			
    def sendBinaryFile(self):
        files = os.listdir(self.dirOutgoing)
        #print files
        binaryFile = ""
        for file in files:
            if (re.search('\\.xml$',file) == None):
               binaryFile = file
        #assert(binaryFile)
        #print binaryFile
        self.__sendFile(binaryFile)
		
    def sendXMLFile(self):
        files = os.listdir(self.dirOutgoing)
        #print files
        xmlFile = ""
        for file in files:
            if (re.search('\\.xml$',file)):
               xmlFile = file
        assert(xmlFile)
        print xmlFile
        self.__sendFile(xmlFile)
	
    def uriXMLSubstitution(self, uri, xml):
        #substitute in xml uri value
        pass	

    def __sendFile(self,filename):
        #MtomUpload.MtomUpload().Upload(filename)
        pass
		
		
def createUploadHandler(queue, observer, dirCache, id):
    uploadHandler = UploadHandler(queue,observer, dirCache, id)
    uploadHandler.upload()
    
if __name__ == '__main__':
  createUploadHandler()
