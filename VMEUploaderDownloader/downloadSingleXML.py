import os, sys, shutil, time
from webServicesClient import xmlrpcDemoWS
from base64 import decodestring
import xml.dom.minidom as xd
from lhpDefines import *
from Debug import Debug

class downloadSingleXML():
    def __init__(self):
        
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
       
    if(len(sys.argv) == 5):
        sys.argv = sys.argv[1:]
        #print sys.argv
        dsXML = downloadSingleXML()
        dsXML.user = sys.argv[0]
        dsXML.password = sys.argv[1]
        dsXML.fileToDownload = sys.argv[2]
        dsXML.incomingCacheDir = str(sys.argv[3]).replace("?", " ")
        dsXML.downloadXMLFromBasket()
        dsXML.datasetSRBURI = dsXML.retrieveTagValue('L0000_resource_data_Dataset_DatasetURI')
        dsXML.datasetFileSize = dsXML.retrieveTagValue('L0000_resource_data_Size_FileSize')        
        dsXML.moveFileInIncomingCacheDirectory()
        
        
        # datatset SRB Uri: BEWARE !!! This print value is used by the Client application!
        # DO NOT CHANGE! 
        print dsXML.datasetSRBURI
        
        # dataset file size: BEWARE !!! This print value is used by the Client application!
        # DO NOT CHANGE!  
        print dsXML.datasetFileSize
        
    else:
        print """
        Usage: 
        downloadSingleXML.py
        testuser # substitute user name
        6w8DHF # substitute user password
        dataresource-8660 # substitute xml test data present in the repository
        os.getcwd()+"\\testDownload\\" # substitute incoming cache dir
       
        Output:
        # datatset SRB Uri: BEWARE !!! This print value is used by the Client application!
        datasetSRBURI
        
        # dataset file size: BEWARE !!! This print value is used by the Client application!
        datasetFileSize
       
       """
if __name__ == '__main__':
    main()