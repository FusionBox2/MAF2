import sys,time
from xml.dom import minidom
import msfParser

doc = minidom.parse(sys.argv[1])
rootNode = doc.documentElement

p = msfParser.msfParser()

vme = p.GetVmeNodeById(rootNode,int(sys.argv[2]))

data = p.GetVMEDataURLList(vme)

#if no binary is found, then return an empty string
if (len(data) == 0):
    print ""
else:
    print data[0]

