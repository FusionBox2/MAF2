import sys, time
from xml.dom import minidom
import msfParser


class lhpVMEBinaryDataChecker:
    
    """Check if a vme has binary data associated:
    If the vme does not exist, then return None and print an empty string        
    If no binary data is found, then return None and print an empty string
    else print the binary data file local name and return it 
    """
    
    def __init__(self):                              
        self.inputMSFAbsFileName = None 
        self.inputVmeId = None
               
    def GetBinaryDataLocalFileName(self):
        doc = minidom.parse(self.inputMSFAbsFileName)
        rootNode = doc.documentElement
        
        p = msfParser.msfParser()
        
        vme = p.GetVmeNodeById(rootNode, self.inputVmeId)
        
        #if vme does not exist, then return None and print an empty string
        if vme == None:
            print ""
            return ""
                    
        vmeBinaryDataLocalFileName = p.GetVMEDataURLList(vme)
        
        #if no binary is found, then return None and print an empty string
        if (len(vmeBinaryDataLocalFileName) == 0):
            print ""
            return ""
        else:
        # else print the binary vmeBinaryDataLocalFileName file local name
            print vmeBinaryDataLocalFileName[0]
            return vmeBinaryDataLocalFileName[0]

def run(inputMSFAbsFileName, vmeId):
    lcbn = lhpVMEBinaryDataChecker()
    lcbn.inputMSFAbsFileName = inputMSFAbsFileName
    lcbn.inputVmeId = vmeId
    return lcbn.GetBinaryDataLocalFileName()
    
# python.exe  lhpVMEBinaryDataChecker.py "D:/vapps_merge_target/Branch22/test/test.msf" -1 

def main():
    args = sys.argv[1:]
    if len(args) != 2:
        print 'usage: lhpVMEBinaryDataChecker.py msfAbsPath vmeID'
        print """Check if a vme with vmeID in msfAbsPath msf file has binary data associated:
If the vme does not exist, then return "" (empty string) and print an "" (empty string)        
If no binary data is found, then return "" (empty string) and print "" (empty string)
else print and return the binary data file local name 
"""
    
        sys.exit(-1)
    return run(sys.argv[1], sys.argv[2])


if __name__ == '__main__':
    main()
