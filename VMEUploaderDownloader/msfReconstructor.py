import sys, os, time
import xml.dom.minidom as xd
import vmeDownloader

class MSFReconstructor():
    def __init__(self):
        self.currentDir = ''
        self.InputVMEBinaryDataFileName = ''
        self.InputVMEXMLFileName = ''
        self.FakeRootMSFFileName = ''
        self.OutputMSFFileName = ''
        self.OutputMSFFolderName = ''
        pass
    
    """def retrieveBinaryName2(self):
        #this function retrieve vtk, must be updated for other file types
        returnValue = ''
        file = open(self.InputVMEXMLFileName, 'r')
        dom = xd.parse(file)
        if dom.getElementsByTagName("fault"):
                return       
        for el in dom.getElementsByTagName('VItem'):
           for vitem in el.getElementsByTagName('URL'):
              returnValue = vitem.childNodes[0].nodeValue
              time.sleep(1)
              print returnValue
        file.close()
        
        return returnValue"""
    
    def retrieveBinaryName(self):
        #URL of the binary file are serialized in different part of the msf
        #so I have to manage 3 cases. In the future, we'll try to uniform
        #the URL position.
        isExternal = 0
        
        #this function retrieve vtk, must be updated for other file types
        returnValue = ""
        file = open(self.InputVMEXMLFileName, 'r')
        dom = xd.parse(file)
        if dom.getElementsByTagName("fault"):
                return       
        inputVme = dom.getElementsByTagName('Node')
        
        attrs = inputVme[0].attributes
        for attrName in attrs.keys():
            attrNode = attrs.get(attrName)
            attrValue = attrNode.nodeValue
            if (attrName  == "Type"):
              if (attrValue == "mafVMEExternalData"):
                  isExternal = 1
                  returnValue = ""
              else:                  
                 break
           
        child = dom.getElementsByTagName('TItem')
        for children in child:
            if (len(child) != 0):
                attrs = children.attributes                            
                for attrName in attrs.keys():
                    attrNode = attrs.get(attrName)
                    attrValue = attrNode.nodeValue
                    if (attrName  == "Name"): #in External Data files
                        if (attrValue == "EXTDATA_EXTENSION"):
                            tc = children.getElementsByTagName('TC')
                            extension = tc[0].childNodes[0].nodeValue
                            extFound = 1
                            print extension
                                    
                    if (attrName  == "Name"): #in External Data files
                        if (attrValue == "EXTDATA_FILENAME"):
                            tc = children.getElementsByTagName('TC')
                            fileName = tc[0].childNodes[0].nodeValue
                            returnValue = fileName + '.' + extension
                            print returnValue 
                            return returnValue 
                                
                                
        child = dom.getElementsByTagName('DataVector')
        if (len(child) != 0):
            attrs = child[0].attributes                            
            for attrName in attrs.keys():
                attrNode = attrs.get(attrName)
                attrValue = attrNode.nodeValue
                if (attrName  == "ArchiveFileName"): #in .zvtk files
                    returnValue = attrValue
                    return returnValue

              
            for el in child[0].getElementsByTagName('VItem'):
               for vitem in el.getElementsByTagName('URL'):
                  returnValue = vitem.childNodes[0].nodeValue
                  return returnValue
        else:
            retunValue = ""
            return returnValue
            
              
        file.close()     
        return returnValue
    
    def build(self):
        msfBuilder = vmeDownloader.MSFBuilder()
        msfBuilder.InputVMEBinaryDataFileName = self.InputVMEBinaryDataFileName
        msfBuilder.InputVMEXMLFileName = self.InputVMEXMLFileName
        msfBuilder.FakeRootMSFFileName = self.FakeRootMSFFileName
        msfBuilder.OutputMSFFileName = self.OutputMSFFileName
        msfBuilder.OutputMSFFolderName = self.OutputMSFFolderName
        
        """print self.currentDir
        print self.InputVMEBinaryDataFileName
        print self.InputVMEXMLFileName
        print self.FakeRootMSFFileName
        print self.OutputMSFFileName
        print self.OutputMSFFolderName"""
        msfBuilder.Build()
 

def reconstructMSF():
    msfR = MSFReconstructor()
    currentMSF = "NOT DEFINED"
    dataBinary = "NOT DEFINED"
    
    if(len(sys.argv)  != 4): 
        print "argv  != 4, MSFReconstructor exiting without doing anything..."
        return
    else:
        msfR.currentDir = sys.argv[1].replace("?", " ")
        msfR.scriptsDir = msfR.currentDir + r'..\..' #go back of two directories 
        msfR.InputVMEXMLFileName = msfR.currentDir + sys.argv[2]
        
        dataBinary = msfR.retrieveBinaryName()
        
        msfR.InputVMEBinaryDataFileName = msfR.currentDir + dataBinary
        print msfR.InputVMEBinaryDataFileName
        
        msfR.FakeRootMSFFileName = msfR.scriptsDir + r'\applicationData\fakeRoot.xml'
        msfR.OutputMSFFileName = msfR.currentDir + r'outputMAF.msf'
        msfR.OutputMSFFolderName = msfR.currentDir
        currentMSF = str(sys.argv[3].replace("?", " "))
        

    #create configuration file
    file = open(msfR.currentDir+"configuration.conf","w")
    file.write(currentMSF) # information regarding where is saved current MSF of Builder
    file.write(dataBinary)  # information regarding name of the databinary
    file.close()
    
    
    msfR.build()
    
if __name__ == '__main__':
    reconstructMSF()