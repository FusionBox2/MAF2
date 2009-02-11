import os, sys, shutil, time
from webServicesClient import xmlrpcDemoWS
from base64 import decodestring
import xml.dom.minidom as xd
from lhpDefines import *
from Debug import Debug

class downloadSingleXML():
    def __init__(self):
        self.user = sys.argv[0]
        self.password = sys.argv[1]
        self.fileToDownload = sys.argv[2]
        self.incomingCacheDir = str(sys.argv[3]).replace("?", " ")
        
        self.datasetSRBURI = 'testSRBURI'
        self.datasetFileSize = -1
        self.proxyHost = ""
        self.proxyPort = 0
        pass
    
    def downloadXMLFromBasket(self):
        self.proxyHost, self.proxyPort = retriveProxyParameters()

        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setCredentials(self.user, self.password)
        ws.setServer('https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2')        
        ws.ProxyURL = self.proxyHost
        ws.ProxyPort = self.proxyPort
        self.Result = ws.run('xmldownload', self.fileToDownload)
        
    def retrieveTagValue(self, tag):
        returnValue = ''
        try:
           file = open(self.fileToDownload, 'r')
        except :
            return ""
        dom = xd.parse(file)
        if dom.getElementsByTagName("fault"):
            if Debug:
                print "------Error in xmldownload service--------"
            return       
        for el in dom.getElementsByTagName('TItem'):
           if(el.attributes != None and el.attributes.get('Name')):
              attrNode = el.attributes.get('Name')
              attrValue = attrNode.nodeValue
              if(attrValue == tag):
                  childToFind = el.getElementsByTagName('TC')[0] # first element of childList
                  returnValue = childToFind.firstChild.nodeValue
                  if Debug:
                      print returnValue
        file.close()
        
        return returnValue
    
    def moveFileInIncomingCacheDirectory(self):
        shutil.move(self.fileToDownload, self.incomingCacheDir + self.fileToDownload)
        pass

def main():
    
    if(len(sys.argv) != 5): #for test
       sys.argv = []
       sys.argv.append("Program")
       sys.argv.append("testuser") #substitute
       sys.argv.append("6w8DHF") #substitute
       sys.argv.append('dataresource-8660') #xml test present in repository
       sys.argv.append(os.getcwd()+"\\testDownload\\")
       
    if(len(sys.argv) == 5):
        sys.argv = sys.argv[1:]
        #print sys.argv
        dsXML = downloadSingleXML()
        dsXML.downloadXMLFromBasket()
        
        dsXML.datasetSRBURI = dsXML.retrieveTagValue('L0000_resource_data_Dataset_DatasetURI')
        dsXML.datasetFileSize = dsXML.retrieveTagValue('L0000_resource_data_Size_FileSize')
        
        dsXML.moveFileInIncomingCacheDirectory()
        
        print "dtatset SRB Uri: " + dsXML.datasetSRBURI
        print "dataset file size: "  + dsXML.datasetFileSize
        
        assert(dsXML.datasetSRBURI == "data_4085")
        assert(dsXML.datasetFileSize == "437")

if __name__ == '__main__':
    main()