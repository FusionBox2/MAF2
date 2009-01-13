import vmeUploaderOnly
from webServicesClient import MtomUpload, MtomUploadURI, MtomSRBSize , xmlrpcDemoWS
import msfParser
import lhpRemoveResource
from xml.dom import minidom
from xml.dom import Node
import xml.dom.minidom as xd
import os, sys, string, time, re , shutil
import threading, thread, CustomThread
import fileUtilities
import urllib
import msvcrt
import wx
import fileinput
from lhpDefines import *
from Debug import Debug
import shutil
import time

class UploadHandler:
    queue = None
    def __init__(self, queue, observer , dirCache, id, usr , pwd, urlServer, originalId, hasLink, withChild, msfListFile, XMLURI, isLast, vmeName):
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
        self.isBinaryDataPresent = False
        self.binaryName = ''
        self.BinaryURI = "NOT PRESENT"
        self.isLast = isLast
        self.block = threading.Lock()
        self.threads = []
        self.hasLink = hasLink
        self.withChild = withChild
        self.msfListFile = msfListFile
        self.XMLURI = XMLURI
        self.vmeName = vmeName
        self.existThread = 0
        self.remoteChksum = ""
        self.localChksum = ""
        self.uri = ""
        self.proxyHost = ""
        self.proxyPort = 0
        
        
            
    def upload(self):
        self.proxyHost, self.proxyPort = retriveProxyParameters()
        if Debug:
            print "->" + self.proxyHost + "<-"
            print "->" + str(self.proxyPort) + "<-"
       
        self.createOutgoingDir()
        
        self.binarySendResult = False
        self.isBinaryDataPresent = self.isBinaryPresent()      
        
        #Check if VME has a binary data        
        if(self.isBinaryDataPresent == True):
            
            #get free resource (return URI string)
            self.getFreeResource() #thread maybe
            if Debug:
                print self.BinaryURI
            #thread.start_new_thread(self.getFreeResource,())
            
            self.createXMLAndBinary()
                    
            binaryFileName = self.getBinaryFile()
            if Debug:
                print "binary name: " + str(binaryFileName)
            if (binaryFileName != ""):
               
                if Debug:
                    print self.BinaryURI
                #-1 percentageUploaded means progress pulsing 
                    
                """
                percentageUploaded = -1
                while(1):
                    lista = [self.observer,percentageUploaded, isLast]
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
                    
                thread.start_new_thread(self.sendBinaryFile, ())      
                
                if Debug:
                    print "Wainting for sending binary..."
                    print "Total Size of Binary: " + str(self.binaryFileSize)
                
                #binarySendResult = False
                oldPercentage = - 1
                percentageUploaded = 0
                timeStep = 0.5
                
                uploadSpeed = GetHeuristicUploadSpeedEstimateInKBPerSecond("..\\..")
                
                tStart = time.time() 
                
                uploadTimeEstimate = self.binaryFileSize / uploadSpeed
                
                tFinish = tStart + uploadTimeEstimate        
                
                if Debug:
                    print "binaryFileSize: " + str(self.binaryFileSize)
                    print "tStart: " + str(tStart)
                    print "uploadTimeEstimate: " + str(uploadTimeEstimate)
                    print "tFinish: " + str(tFinish)
                    
                uploadedOverTotal = 0 
                timeElapsed = 0
                
                while 1:
                  
                    time.sleep(timeStep)
                     
                    if(self.existThread == 0):
                        thread.start_new_thread(self.getRemoteTemporaryBinaryFileSize, ())
                    
                    uploadedOverTotal = (timeElapsed / uploadTimeEstimate)
                    percentageUploaded = 90 * float(uploadedOverTotal)
                    
                    timeElapsed = time.time() - tStart
                    
                    remainingTime = uploadTimeEstimate - timeElapsed 
                    
                    if remainingTime < 0:
                        remainingTime = 0
                        
                    if Debug:
                        print "timeElapsed: " + str(timeElapsed)
                        print "uploadedOverTotal: " + str(uploadedOverTotal)
                        print "percentageUploaded: " + str(percentageUploaded) + "%"
                        print "uploaded data size: " + str(uploadedOverTotal*self.binaryFileSize)
                        print "remainingTime: " + str(remainingTime)
                        print ""
                        print "binarySendResult: " + str(self.binarySendResult) 
                   
                    lista = [self.observer, percentageUploaded, remainingTime]
                    
                    
                    self.block.acquire()
                    UploadHandler.queue.put(lista)
                    self.block.release()
                    
                    if(self.binarySendResult == True):
                        self.block.acquire()
                        lista = [self.observer, 90, -10]
                        UploadHandler.queue.put(lista)
                        self.block.release()
                        break
        else:
        
            self.createXMLAndBinary()
            self.binarySendResult = True
                       
        #---MD5 check-point-----------------------------------#
        if(self.isBinaryDataPresent == True and self.binarySendResult == True):
            if Debug:
                print "Local checksum=  " + self.localChksum
                print "Remote checksum= " + self.remoteChksum
            if (self.localChksum == self.remoteChksum):
                if Debug:
                    print " "
                    print "-------MD5 checksum control successful!-----------"
                    print " "
            else:
                if Debug:
                    print " "
                    print "-------Error: MD5 checksum control unsuccessful!--------"
                    print " "
                percentageUploaded = 120 #120 for 'error!'
                lista = [self.observer, percentageUploaded]
                self.block.acquire()  
                UploadHandler.queue.put(lista)
                self.block.release()
                self.binarySendResult = False
                errorFile = open(sys.path[0] + '\\ErrorFound.lhp', 'a')
                errorFile.write('Checksum Error uploading binary data of VME: ' + self.vmeName + '.')
                errorFile.close() 
                if(self.msfListFile != "noMsf"):
                    self.removeUploadedXml()
        #---------------------------------------------------#
        
        #send xml file, perhaps here free source
        if(self.binarySendResult == True):
          self.sendXMLFile()
          percentageUploaded = 110 #110 for 'complete!'
          lista = [self.observer, percentageUploaded]
          self.block.acquire()  
          UploadHandler.queue.put(lista)
          self.block.release()
          allEnded = False
 
          if (self.isLast == "false"):
              #replace status for resource correctly uploadedOverTotal
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
                            counter = counter + 1 #to avoid deadlock
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
                            counter = counter + 1 #to avoid deadlock
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
                          lastStarted = False
                  elif line == "ended\n":
                      endedCount += 1   
                  elif line == "lastStarted\n":
                      lastStarted = True
                      lastStartedCount += 1
                      if processStarted == 0:
                          processStarted += 1
                  elif line == "lastEnded\n":
                      lastEndedCount += 1
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
              counter = counter + 1 #to avoid deadlock
              statusFile.close()
              if(counter == 10):
                  if Debug:
                        print "----------Can not read in status.txt-----------"
                  pass                  
          if allEnded == True:
              self.block.acquire()  
              percentageUploaded = 130 #130 for 'ALL COMPLETE!'
              lista = [self.observer, percentageUploaded]
              UploadHandler.queue.put(lista)
              self.block.release()
              if(os.path.exists(sys.path[0] + '\\status.txt')):
                  os.remove(sys.path[0] + '\\status.txt')
                  if Debug:
                      print "status.txt removed" 
                  
    
        
        
          if Debug:
              print "End Upload"
              print "Uploaded Binary in SRB: " + self.BinaryURI
              print "Uploaded XML on Biomedtown: " + self.XMLName
              print "Uploaded by: " + self.currentUser
              print "In server url: " + self.urlServer
              print "--------Upload successful :D :D :D --------"
          
          if Debug:
              print "--------cleaning up cache directories--------"
              print "cache dir is: " + str(self.dirCache) 
              print "outgoing dir is " + str(self.dirOutgoing)
              print "current dir is: " + str(os.getcwd())
          os.chdir(self.dirOutgoing + r"\..\..")
          if Debug:
              print "changing to: " + str(os.getcwd())
              print "removing " + str(self.dirCache) 
          fileUtilities.rmdir_recursive(self.dirCache)
          
          if (os.path.isdir(self.dirCache) == False):
            if Debug:
                print "done!"
          else:
            if Debug:
                print "cannot remove " + str(self.dirCache)
        

          if Debug:
              print "removing " + str(self.dirOutgoing)
          fileUtilities.rmdir_recursive(self.dirOutgoing)
          
          if (os.path.isdir(self.dirOutgoing) == False):
            if Debug:
                print "done!"
          else:
            if Debug:
                print "cannot remove " + str(self.dirOutgoing)
          if Debug:
              print "--------clean up cache directories successful--------"
              
          
        else:
          percentageUploaded = 120 #120 for 'error!'
          lista = [self.observer, percentageUploaded]
          self.block.acquire()  
          UploadHandler.queue.put(lista)
          self.block.release()
          if Debug:
              print "--------Error uploading binary data----------------"
              

                  
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

        upl.OutputFolderName = self.dirOutgoing
        upl.VmeToExtractID = int(self.id)
        upl.originalId = self.originalId
        upl.DatasetURI = self.BinaryURI
        upl.hasLink = self.hasLink
        upl.withChild = self.withChild
        upl.vmeName = self.vmeName
        upl.hasBinary = self.isBinaryDataPresent
        upl.binaryName = self.binaryName
        upl.user = self.currentUser
        if(self.msfListFile != 'noMsf'):
            upl.IsUploadMSF = True
            
        self.localChksum = upl.Upload()
        if(self.localChksum == '' and self.isBinaryDataPresent == True and self.msfListFile != 'noMsf'):
            percentage = 120 #120 for 'error!'
            lista = [self.observer, percentage]
            self.block.acquire()  
            UploadHandler.queue.put(lista)
            self.block.release()
            self.removeUploadedXml()
            return
            
        
        #get size of binary locally
        if (self.isBinaryDataPresent == True):
            self.binaryFileSize = self.getBinaryFileSize()

    def launchXMLEditor(self, dir):
        oldDir = os.getcwd()
        os.chdir("\"C:\\Program Files\\Peter's XML Editor\\")
        #print os.getcwd()
        command = "\"" + dir + "\\" + self.getXMLFile() + "\""
        command = "pxe.exe " + command
        #print command
        os.system(command)
        os.chdir(oldDir)
        
    def getFreeResource(self):
        #here call module to get URI of first free resource
        instance = MtomUploadURI.MtomUploadURI()
        serviceUrl = 'https://ws-lhdl.cineca.it/mafSRBUploadURI.cgi'

        try:
            result = instance.ListSrbDir(serviceUrl, self.proxyHost, self.proxyPort)
        except:
            if Debug:
                print "------Error calling mafSRBUploadURI.cgi----------"  
            return ""
        if result == "":
            percentage = 120 #120 for 'error!'
            lista = [self.observer, percentage]
            self.block.acquire()  
            UploadHandler.queue.put(lista)
            self.block.release()
            errorFile = open(sys.path[0] + '\\ErrorFound.lhp', 'a')
            errorFile.write('Error calling mafSRBUploadURI.cgi.')
            errorFile.close() 
            if(self.msfListFile != "noMsf"):
                self.removeUploadedXml()
                return
                             
        self.BinaryURI = result
        #print 'Inside FreeResource Thread ' + self.BinaryURI


    def sendBinaryFile(self):
        os.rename(self.dirOutgoing + "\\" + self.getBinaryFile(), self.dirOutgoing + "\\" + self.BinaryURI)
        self.__sendFile(self.BinaryURI)
        if Debug:
            print "Sending Thread Finished"
		
    def sendXMLFile(self):
        os.rename(self.dirOutgoing + "\\" + self.getXMLFile(), self.dirOutgoing + "\\" + self.XMLURI)
        oldDir = os.getcwd()
        os.chdir(self.dirOutgoing)
        
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setCredentials(self.currentUser, self.currentPassword)
        ws.setServer(self.urlServer)
        ws.ProxyURL = self.proxyHost
        ws.ProxyPort = self.proxyPort
        
        try:
            out = ws.run('xmlupload', self.XMLURI, self.vmeName)[1]
        except:
            if Debug:
                print "-----------Error in xmlupload service------------"
            
            percentage = 120 #120 for 'error!'
            lista = [self.observer, percentage]
            self.block.acquire()  
            UploadHandler.queue.put(lista)
            self.block.release()
            return
            #
    #    self.__removeSRBData(self.BinaryURI)
        
        dom = xd.parseString(out)
        if dom.getElementsByTagName("fault"):
            if Debug:
                print "-----------Error in xmlupload service------------"
            for el in dom.getElementsByTagName("string"):
                for node in el.childNodes:  
                    error = node.data

            if Debug:
                print error 
            percentage = 120 #120 for 'error!'
            lista = [self.observer, percentage]
            self.block.acquire()  
            UploadHandler.queue.put(lista)
            self.block.release()
            #write a file used by builder to catch error and stop MSF upload
            errorFile = open(sys.path[0] + '\\ErrorFound.lhp', 'a')
            errorFile.write('Error in xmlupload service uploading VME: ' + self.vmeName + '.')
            errorFile.write(error)
            errorFile.close() 
            if(os.path.exists(sys.path[0] + '\\' + self.msfListFile)):
                while 1:
                    size = os.path.getsize(sys.path[0] + '\\' + self.msfListFile)
                    msfList = open(sys.path[0] + '\\' + self.msfListFile, 'a')
                    try:
                        msvcrt.locking(msfList.fileno(), msvcrt.LK_RLCK, size)
                        msfList.write(self.XMLURI + '\n')
                        if Debug:
                            print 'data: ' + self.XMLURI  
                        msfList.close()
                        break
                    except:
                        counter = counter + 1 #to avoid deadlock
                        msfList.close()
                        pass
                    if(counter == 3):

                        if Debug:
                            print "----------Can not write in " + self.msfList + "-----------"
                        msfList.close()
                        os.remove(sys.path[0] + '\\' + self.msfListFile)
                        return
            if(self.msfListFile != "noMsf"):
                self.removeUploadedXml()
            
   
  #          self.__removeSRBData(self.BinaryURI)
            return
        for el in dom.getElementsByTagName("string"):
            for node in el.childNodes:  
                self.XMLName = node.data
        pass
    
        counter = 0
        if(self.msfListFile != 'noMsf' and str(self.id) != '-1'):
            #better to use a locked file
            if(os.path.exists(sys.path[0] + '\\' + self.msfListFile)):
                while 1:
                    size = os.path.getsize(sys.path[0] + '\\' + self.msfListFile)
                    msfList = open(sys.path[0] + '\\' + self.msfListFile, 'a')
                    try:
                        msvcrt.locking(msfList.fileno(), msvcrt.LK_RLCK, size)
                        msfList.write(self.XMLName + '\n')
                        msfList.close()
                        break
                    except:
                        counter = counter + 1 #to avoid deadlock
                        msfList.close()
                        pass
                    if(counter == 3):

                        if Debug:
                            print "----------Can not write in " + self.msfList + "-----------"
                        msfList.close()
                        os.remove(sys.path[0] + '\\' + self.msfListFile)
                        return            
                
        if(str(self.id) == '-1'):
            if(os.path.exists(sys.path[0] + '\\' + self.msfListFile)):
                os.remove(sys.path[0] + '\\' + self.msfListFile)
                    
                          
        if Debug:
            print self.XMLName
        
    def isBinaryPresent(self):
        result = False
        
        #if node is root VME, no binary is present
        if (str(self.id) == "-1"):
            if Debug:
                print "\nNo binary data"
            return result
        
        oldDir = os.getcwd()
        os.chdir(self.dirCache)
        files = os.listdir(self.dirCache)
    
        msfFileNameList = []
        for file in files:
           if re.search('\\.msf$', file):
              msfFileNameList.append(file)
        assert(len(msfFileNameList) == 1)
        
        msfFileName = msfFileNameList[0]
        domDocument = minidom.parse(msfFileName)
        msfRootNode = domDocument.documentElement
        
        rootNode = msfRootNode
        
        if Debug:
            print "\ninput MSF Directory: " + self.dirCache
            print "\ninput MSF filename: " + msfFileName
            print "\nExtracting vme with ID: " + str(self.id) + '\n' 
        
        msfDOMParserInstance = msfParser.msfParser()
        
        # get the vme node
        outVmeNode = msfDOMParserInstance.GetVmeNodeById(rootNode, self.id)
        fileNameList = msfDOMParserInstance.GetVMEDataURLList(outVmeNode)
        if (len(fileNameList) == 1 and len(fileNameList[0]) != 0):
            self.binaryName = fileNameList[0]
            result = True
            if Debug:
                print "Binary data present"
               
        os.chdir(oldDir)  
        return result
        
    
            
    def __sendFile(self, filename):
        
        oldDir = os.getcwd()
        os.chdir(self.dirOutgoing)
        
        if Debug:
            print "->" + self.proxyHost + "<-"
            print "->" + str(self.proxyPort) + "<-"
        
        instance = MtomUpload.MtomUpload()
        try:
            result = instance.Upload(filename, 'https://ws-lhdl.cineca.it/mafSRBUpload.cgi', self.proxyHost, self.proxyPort)
        except:
            if Debug:
                print "--------Error calling mafSRBUpload.cgi-----------"
            return
            
        self.remoteChksum = result.chksum
        self.remoteChksum = self.remoteChksum.lower()
        if Debug:
            print "Remote checksum: " + str(self.remoteChksum)
        self.uri = result.uriFile
        if Debug:
            print "URI File: " + str(result.uriFile)
        self.binarySendResult = True
        os.chdir(oldDir)
        
  #  def __removeSRBData(self,filename):
  #      
  #      oldDir = os.getcwd()
  #      os.chdir(self.dirOutgoing)
  #      
  #      print "->"+ self.proxyHost + "<-"
  #      print "->"+ str(self.proxyPort) + "<-"
  #      
  #      instance = MtomDelete.MtomDelete()
  #      result = instance.Delete(filename,'https://ws-lhdl.cineca.it/mafSRBDelete.cgi',self.proxyHost,self.proxyPort)
   

#        time.sleep(1)
#        os.chdir(oldDir)
        
    
    def getRemoteTemporaryBinaryFileSize(self):
        """
         Works only inside CINECA. Using an heuristic instead of this: see Upload() function
        """    
    
        self.block.acquire()
        self.existThread = 1
        self.block.release()
        result = None
        
        if Debug:
            print "->" + self.proxyHost + "<-"
            print "->" + str(self.proxyPort) + "<-"
        
        try:
            instance = MtomSRBSize.MtomSize()

            serviceUrl = 'https://ws-lhdl.cineca.it/mafSRBSize.cgi'

                
            result = instance.ListSrbDir(self.BinaryURI, serviceUrl, self.proxyHost, self.proxyPort)
            self.remoteTemporaryBinaryFileSize = result;

                        
            if Debug:
                print "SIZE Thread Finished "
        except:
            return
        self.block.acquire()
        self.existThread = 0
        self.block.release() 
        #return result;
        
    def removeUploadedXml(self):
        self.proxyHost, self.proxyPort = retriveProxyParameters()
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setCredentials(self.currentUser, self.currentPassword)
        ws.ProxyURL = self.proxyHost
        ws.ProxyPort = self.proxyPort
        
        if(self.msfListFile != 'noMsf'):
            #better to use a locked file
            if(os.path.exists(sys.path[0] + '\\' + self.msfListFile)):
                msfList = open(sys.path[0] + '\\' + self.msfListFile, 'r')
                for line in msfList.readlines():
                    if Debug:
                        print "removing file: " + line
                    if(line != ""):
                        line = line.replace ("\n", "")
                        ws.setServer(self.urlServer)
                        
                        #check if resource exists on repository
                        page = urllib.urlopen(self.urlServer + line)
                        pagedata = page.read()
                        result = pagedata.find('Resource not found')
                        if(result != - 1):
                            continue
                        try:
                            out = ws.run('xmldelete', line)[1]
                        except:
                            if Debug:
                                print "-----Error in xmldelete service---------" 
                            continue
                            
            
                        dom = xd.parseString(out)
                        if dom.getElementsByTagName("fault"):
                            if Debug:
                                print "-----------Error in xmldelete service------------"
                            for el in dom.getElementsByTagName("string"):
                                for node in el.childNodes:  
                                    error = node.data
                            if Debug:
                                print error
                            continue
        
            os.remove(sys.path[0] + '\\' + self.msfListFile)           


    def getXMLFile(self):
        files = os.listdir(self.dirOutgoing)
        #print files
        xmlFile = ""
        for file in files:
            if (re.search('\\.xml$', file)):
               xmlFile = file
        return xmlFile

    def getBinaryFile(self):
        files = os.listdir(self.dirOutgoing)
        #print files
        binaryFile = ""
        for file in files:
            if (re.search('\\.xml$', file) == None):
               binaryFile = file
        return binaryFile
    
    def getBinaryFileSize(self):
        return os.stat(self.dirOutgoing + "\\" + self.getBinaryFile()).st_size
    
        
def GetHeuristicUploadSpeedEstimateInKBPerSecond(dataDirPath="."):
    
    testFile = str(dataDirPath) + "\\vmeUploaderTestData\\uploadSpeedProbeData\\uploadSpeedProbeData.vtk"
    testFileSize = os.stat(testFile).st_size
    instanceURI = MtomUploadURI.MtomUploadURI()
    serviceUrl = 'https://ws-lhdl.cineca.it/mafSRBUploadURI.cgi'
    binaryURI = "NOT PRESENT"
    proxyHost = ""
    proxyPort = 0
     
    freename = instanceURI.ListSrbDir(serviceUrl, proxyHost, proxyPort)
    
    if Debug:
        print "testFile: " + testFile
        print "freeName: " + freename
        print "size: " + str(testFileSize)
        
    shutil.copyfile(testFile, freename)
    instanceUP = MtomUpload.MtomUpload()
    
    startT = time.time()
   
    result = instanceUP.Upload(freename, 'https://ws-lhdl-dev.cineca.it:12443/mafSRBUpload.cgi', proxyHost, proxyPort)
    
    endT = time.time()
    
    tElapsed = endT - startT
    
    speed = testFileSize / tElapsed
    
    shutil.move(freename, testFile)
    
    checksum = result.chksum
    uri = result.uriFile

    heuristicSpeedInBytes = speed
    
    if Debug:
        print "result: " + str(result)        
        print "cheksum: " + str(checksum)
        print "uri: " + str(uri)
        print "speed:" + str(heuristicSpeedInBytes)
    
    return heuristicSpeedInBytes

		
def createUploadHandler(queue, observer, dirCache, id , usr , pwd, urlServer, originalId, hasLink, withChild, msfListFile, XMLURI, isLast, vmeName):
    uploadHandler = UploadHandler(queue, observer, dirCache, id, usr , pwd, urlServer, originalId, hasLink, withChild, msfListFile, XMLURI, isLast, vmeName)
    uploadHandler.upload()
    
    
if __name__ == '__main__':
  createUploadHandler()
