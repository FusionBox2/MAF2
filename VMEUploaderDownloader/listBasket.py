from webServicesClient import xmlrpcDemoWS
import xml.dom.minidom as xd
import os, time
from lhpDefines import *
from Debug import Debug

class listBasket:
    def __init__(self):
        self.IdList = [] #create original list of basket vmes
        self.IdListSelected = [] #this list will be copied from handle object
        self.fileName = "ToDownload.txt"
        self.currentUser = ''
        self.currentPassword = ''
        self.Result = None
        self.proxyHost = ""
        self.proxyPort = 0
        pass
    
    def SetCredentials(self, user , password):
        self.currentUser = user
        self.currentPassword = password
    
    
    def getListFromBasket(self):
        self.removeIdListSelectedFile()
        
        self.proxyHost, self.proxyPort = retriveProxyParameters()
        
        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setServer('https://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository2')
        ws.setCredentials(self.currentUser, self.currentPassword)
        ws.ProxyURL = self.proxyHost
        ws.ProxyPort = self.proxyPort

        if Debug:
            print "->"+ ws.ProxyURL + "<-"
            print "->"+ str(ws.ProxyPort) + "<-"
        
        try:
            self.Result = ws.run('listbasket')[1]
        except Exception, e:
             print "ERROR %s" % str(e)
        self.__createListFromReultingXML()
        #print self.IdList
        pass
    
    def __createListFromReultingXML(self):
        #add element to self.IdList
        #self.IdList.append("Id1.xml")
        #self.IdList.append("Id2.xml")
        dom = xd.parseString(self.Result)
        if dom.getElementsByTagName("fault"):
            print "-----Error in listbasket service---------"
            return
        for el in dom.getElementsByTagName("string"):
            for node in el.childNodes:  
                self.IdList.append(node.data)
        pass
    
    def removeIdListSelectedFile(self):
        if(os.path.exists(self.fileName)):
            os.remove(self.fileName)
        pass

    def writeIdListSelectedOnFile(self):
        #create file with selected vme's from basket
        file = open(self.fileName,"w")
        for id in self.IdListSelected:
            file.write(str(id))
            if(id != self.IdListSelected[len(self.IdListSelected)-1]):
                file.write("\n")
        
        file.close()
        pass

def test():
    lb = listBasket()
    lb.getListFromBasket()
    print "Id List: " + str(lb.IdList)

if __name__ == '__main__':
  test()
