import sys,time
from xml.dom import minidom
import msfParser

doc = minidom.parse(sys.argv[1])
rootNode = doc.documentElement

p = msfParser.msfParser()

vme = p.GetVmeNodeById(rootNode,int(sys.argv[2]))

data = p.GetVMEDataURLList(vme)

print data[0]

