#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Roberto Mucci <r.mucci@cineca.it>
#-----------------------------------------------------------------------------

from webServicesClient import xmlrpcDemoWS
import xml.dom.minidom as xd
import os, time, shutil
from lhpDefines import *
import StringIO
import urllib
import zipfile


import urllib, urllib2, base64, re, os, cookielib, sys
from HttpsProxy import *

class binaryImporter:
      
    def __init__(self):
        
        self.isAnimated = sys.argv[0]
        self.absOldItemURL = sys.argv[1].replace("???", " ") #URL with spaces arrives with "???" instead of them
        self.absNewItemURL = sys.argv[2].replace("???", " ") #URL with spaces arrives with "???" instead of them
        pass
        
    def CopyBinary(self):
        """ 
           Copy binary data into MSF folder
        """
        
        while 1:
            if os.path.exists(self.absOldItemURL):
                break
        
        if (self.isAnimated == "false"):
            shutil.copy(self.absOldItemURL, self.absNewItemURL)
             #tell to the user that binary data is ready?
        
        else:
            baseName = os.path.split(self.absNewItemURL)
            id = baseName[1].split('.')[1]          
            baseName = baseName[1].split('.')[0]
            fileIn = zipfile.ZipFile(self.absOldItemURL, "r")
            fileOut = zipfile.ZipFile(self.absNewItemURL, "w")
            now = time.localtime(time.time())[:6]
            
            for name in fileIn.namelist():
                data = fileIn.read(name)
                ext = name.split('.')[2]    
                id = int(id) + 1
                newName = baseName + '.' + str(id) + '.' + ext
                info = zipfile.ZipInfo(newName)
                info.date_time = now
                info.compress_type = zipfile.ZIP_DEFLATED
                fileOut.writestr(info, data)
         
            fileIn.close()
            fileOut.close()

            return    
  
def main():
    
    usage_msg = '''Usage: %s isAnimated, absOldItemURL, absNewItemURL ''' % sys.argv[0]
    if(len(sys.argv) <3): 
         print 'Error :\n' + usage_msg
         sys.exit(1)

       
    if(len(sys.argv) >= 3):
        sys.argv = sys.argv[1:]
        #print sys.argv
        importer = binaryImporter()
        importer.CopyBinary()

if __name__ == '__main__':
    main()
        