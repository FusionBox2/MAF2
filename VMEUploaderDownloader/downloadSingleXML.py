import os, sys, shutil, time
from webServicesClient import xmlrpcDemoWS
from base64 import decodestring
import xml.dom.minidom as xd

class downloadSingleXML():
    def __init__(self):
        self.user = sys.argv[0]
        self.password = sys.argv[1]
        self.fileToDownload = sys.argv[2]
        self.incomingCacheDir = str(sys.argv[3]).replace("?", " ")
        
        self.datasetSRBURI = 'testSRBURI'
        self.datasetFileSize = -1
        pass
    
    def downloadXMLFromBasket(self):
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setCredentials(self.user, self.password)
        ws.setServer('http://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2')        
        self.Result = ws.run('xmldownload', self.fileToDownload)
        
    def retrieveTagValue(self, tag):
        returnValue = ''
        try:
           file = open(self.fileToDownload, 'r')
        except :
            return ""
        dom = xd.parse(file)
        if dom.getElementsByTagName("fault"):
                return       
        for el in dom.getElementsByTagName('TItem'):
           if(el.attributes != None and el.attributes.get('Name')):
              attrNode = el.attributes.get('Name')
              attrValue = attrNode.nodeValue
              if(attrValue == tag):
                  returnValue = el.childNodes[0].childNodes[0].firstChild.nodeValue
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
       sys.argv.append("GRDPt8") #substitute
       sys.argv.append('Data_63_exportedVME.xml') #xml test present in repository
       sys.argv.append(os.getcwd()+"\\testDownload\\")
       
    if(len(sys.argv) == 5):
        sys.argv = sys.argv[1:]
        #print sys.argv
        dsXML = downloadSingleXML()
        dsXML.downloadXMLFromBasket()
        
        dsXML.datasetSRBURI = dsXML.retrieveTagValue('L0000_resource_data_Dataset_DatasetURI')
        dsXML.datasetFileSize = dsXML.retrieveTagValue('L0000_resource_data_Dataset_FileSize')
        
        dsXML.moveFileInIncomingCacheDirectory()
        
        print dsXML.datasetSRBURI
        print dsXML.datasetFileSize
        

if __name__ == '__main__':
    main()