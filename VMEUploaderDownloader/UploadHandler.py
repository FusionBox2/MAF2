import vmeUploaderOnly
from webServicesClient import MtomUpload, MtomUploadURI, MtomSRBSize , xmlrpcDemoWS
import msfParser
from xml.dom import minidom
from xml.dom import Node
import xml.dom.minidom as xd
import os, sys, string, time, re ,shutil
import threading, thread, CustomThread
import fileUtilities
from lhpDefines import *
from Debug import Debug

class UploadHandler:
    queue = None
    def __init__(self, queue, observer , dirCache, id, usr , pwd, urlServer,\
                 originalId, hasLink, withChild, vmeName):
        UploadHandler.queue = queue
        self.observer = observer
        self.dirCache = dirCache
        self.dirOutgoing = ""
        self.id = id
        self.currentUser = usr
        self.currentPassword = pwd
        self.urlServer = urlServer
        self.originalId = originalId
        self.msg = 0
        self.binaryFileSize = 0
        self.remoteTemporaryBinaryFileSize = 0
        self.BinaryURI = "NOT PRESENT"
        self.XMLURI = ""
        self.block = threading.Lock()
        self.threads = []
        self.hasLink = hasLink
        self.withChild = withChild
        self.vmeName = vmeName
        self.existThread = 0
        self.remoteChksum = ""
        self.localChksum = ""
        self.uri = ""
        self.proxyHost = ""
        self.proxyPort = 0
            
    def upload(self):
        self.proxyHost, self.proxyPort = retriveProxyParameters()
        print "->"+ self.proxyHost + "<-"
        print "->"+ str(self.proxyPort) + "<-"
                
        self.createOutgoingDir()
        
        binarySendResult = False
        
        #Check if VME has a binary data        
        if(self.isBinaryPresent() == "true"):
            
            #get free resource (return URI string)
            self.BinaryURI = self.getFreeResource() #thread maybe
            #thread.start_new_thread(self.getFreeResource,())
            
            self.createXMLAndBinary()
                    
            binaryFileName = self.getBinaryFile()
            print "binary name: " + str(binaryFileName)
            if (binaryFileName != ""):
               
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
    

                
                #launch external XML editor
                if Debug:
                    self.launchXMLEditor(self.dirOutgoing)
        
                #self.remoteTemporaryBinaryFileSize = self.getRemoteTemporaryBinaryFileSize()
                #send file
            
                thread.start_new_thread(self.sendBinaryFile,())      
                #self.sendBinaryFile() #until there is service monitor don't use thread
                
                print "Wainting for sending binary..."
                print "Total Size of Binary: " + str(self.binaryFileSize)
                #binarySendResult = False
                oldPercentage = -1
                percentage = 0
                
                countTime = 0
                timeStep = 0.5
                while 1:
                    # To simulate asynchronous I/O, we create a random number at
                    # random intervals. Replace the following 2 lines with the real
                    # thing.
                    time.sleep(timeStep)
                    #if(countTime == 1.0): countTime = 0;
                    #else:
                    #    countTime += timeStep
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
        else:
            self.createXMLAndBinary()
            binarySendResult = True
            
            
        #---MD5 check-point-----------------------------------#
        if(self.isBinaryPresent() == "true" and binarySendResult == True):
            print "Local checksum=  " + self.localChksum
            print "Remote checksum= " + self.remoteChksum
            if (self.localChksum == self.remoteChksum):
                print " "
                print "MD5 checksum control successful!"
                print " "
                time.sleep(2)
            else:
                print " "
                print "Error: MD5 checksum control unsuccessful!"
                print " "
                binarySendResult = False
                time.sleep(2)
        #---------------------------------------------------#
            
        
        
          
        #send xml file, perhaps here free source
        if(binarySendResult == True): #and self.checksumControl() == "true"):
          print "Waiting for sending XML..."
          self.sendXMLFile()
          
          #Fake progress bar used when no binary data is uploaded
          percentage = 100          
          lista = [self.observer,percentage]
          UploadHandler.queue.put(lista)
        
          print "End Upload"
          print "Uploaded Binary in SRB: " + self.BinaryURI
          print "Uploaded XML on Biomedtown: " + self.XMLName
          print "Uploaded by: " + self.currentUser
          print "In server url: " + self.urlServer
          print "--------Upload Successful :D :D :D --------"
          
          print "--------cleaning up cache directories--------"
          print "cache dir is: " + str(self.dirCache) 
          print "outgoing dir is " + str(self.dirOutgoing)
          
          print "current dir is: " + str(os.getcwd())
          os.chdir(self.dirOutgoing + r"\..\..")
          print "changing to: " + str(os.getcwd())
          print "removing " + str(self.dirCache) 
          fileUtilities.rmdir_recursive(self.dirCache)
          
          if (os.path.isdir(self.dirCache) == False):
            print "done!"
          else:
            print "cannot remove " + str(self.dirCache)
        

          print "removing " + str(self.dirOutgoing)
          fileUtilities.rmdir_recursive(self.dirOutgoing)
          
          if (os.path.isdir(self.dirOutgoing) == False):
            print "done!"
          else:
            print "cannot remove " + str(self.dirOutgoing)
          print "--------clean up cache directories successful--------"
          
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
        upl = vmeUploaderOnly.vmeUploaderOnly()
        upl.InputMSFDirectory = self.dirCache
        #upl.HandledAutoTagsListFileName = curDir + r'\handledAutoTagsList.csv'
        #upl.UnhandledPlusManualTagsListFileName = curDir + '\\' + self.originalId 
      
        upl.OutputFolderName = self.dirOutgoing
        upl.VmeToExtractID = int(self.id)
        upl.originalId = self.originalId
        upl.DatasetURI = self.BinaryURI
        upl.hasLink = self.hasLink
        upl.withChild = self.withChild
        upl.vmeName = self.vmeName
        self.localChksum = upl.Upload()
        
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
        return instance.ListSrbDir(serviceUrl, self.proxyHost, self.proxyPort)
        #print 'Inside FreeResource Thread ' + self.BinaryURI


    def sendBinaryFile(self):
        os.rename(self.dirOutgoing + "\\" + self.getBinaryFile(),self.dirOutgoing + "\\" +self.BinaryURI)
        self.__sendFile(self.BinaryURI)
        print "Sending Thread Finished"
		
    def sendXMLFile(self):
        self.XMLURI = self.vmeName + "_" +self.getXMLFile()
        os.rename(self.dirOutgoing + "\\" + self.getXMLFile(),self.dirOutgoing + "\\" + self.XMLURI)
        #self.__sendFile(self.XMLURI)
        oldDir = os.getcwd()
        os.chdir(self.dirOutgoing)
        
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setCredentials(self.currentUser, self.currentPassword)
        ws.setServer(self.urlServer)
        ws.ProxyURL = self.proxyHost
        ws.ProxyPort = self.proxyPort
        
        out = ws.run('xmlupload', self.XMLURI, self.vmeName)[1]
        
        dom = xd.parseString(out)
        if dom.getElementsByTagName("fault"):
                return
        for el in dom.getElementsByTagName("string"):
            for node in el.childNodes:  
                self.XMLName = node.data
        pass
    
        print self.XMLName
            
        #writes binary URI in a file in VMEUploaderDownloader dir
       # oldDir = os.getcwd()
        os.chdir(self.dirOutgoing + r"\..\..") 
        file = open(self.vmeName + self.id, 'w')
        file.write(self.XMLName)
        file.close()
        
        os.chdir(oldDir)
        
    def isBinaryPresent(self):
        result = "false"
        
        #if node is root VME, no binary is present
        if (str(self.id) == "-1"):
            print "\nNo binary data"
            return result
        
        oldDir = os.getcwd()
        os.chdir(self.dirCache)
        files = os.listdir(self.dirCache)
    
        msfFileNameList = []
        for file in files:
           if re.search('\\.msf$',file):
              msfFileNameList.append(file)
        assert(len(msfFileNameList)  == 1)
        
        msfFileName = msfFileNameList[0]
        domDocument = minidom.parse(msfFileName)
        msfRootNode = domDocument.documentElement
        
        rootNode = msfRootNode
        
        print "\ninput MSF Directory: " + self.dirCache
        print "\ninput MSF filename: " + msfFileName
        print "\nExtracting vme with ID: " + str(self.id) + '\n' 
        
        msfDOMParserInstance = msfParser.msfParser()
        
        # get the vme node
        outVmeNode = msfDOMParserInstance.GetVmeNodeById(rootNode, self.id)
        fileNameList = msfDOMParserInstance.GetVMEDataURLList(outVmeNode)
        if (len(fileNameList) == 1 and len(fileNameList[0]) != 0):
            result = "true"
               
        os.chdir(oldDir)  
        print "Binary data present"
        return result
            
    def __sendFile(self,filename):
        
        oldDir = os.getcwd()
        os.chdir(self.dirOutgoing)
        
        print "->"+ self.proxyHost + "<-"
        print "->"+ str(self.proxyPort) + "<-"
        
        instance = MtomUpload.MtomUpload()
        result = instance.Upload(filename,'https://ws-lhdl.cineca.it/mafSRBUpload.cgi',self.proxyHost,self.proxyPort)
        
        self.remoteChksum = result.chksum
        self.remoteChksum = self.remoteChksum.lower()
        print "Remote checksum: " + str(self.remoteChksum)
        self.uri = result.uriFile
        print "URI File: " + str(result.uriFile)
        time.sleep(1)
        os.chdir(oldDir)
        
        
    def getRemoteTemporaryBinaryFileSize(self):
        
        self.block.acquire()
        self.existThread = 1
        self.block.release()
        result = None
        
        print "->"+ self.proxyHost + "<-"
        print "->"+ str(self.proxyPort) + "<-"
        
        try:
            instance = MtomSRBSize.MtomSize()
            serviceUrl = 'https://ws-lhdl.cineca.it/mafSRBSize.cgi'
            result = instance.ListSrbDir(self.BinaryURI, serviceUrl, self.proxyHost, self.proxyPort)
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
    
    
		
def createUploadHandler(queue, observer, dirCache, id , usr , pwd, urlServer, originalId, hasLink, withChild, vmeName):
    uploadHandler = UploadHandler(queue,observer, dirCache, id, usr , pwd, urlServer, originalId, hasLink, withChild, vmeName)
    uploadHandler.upload()
    
if __name__ == '__main__':
  createUploadHandler()
