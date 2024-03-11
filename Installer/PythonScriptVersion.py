import sys, time
import updatePythonVersion

#print sys.version       # version info in string
pythonInfos = sys.version_info  # version info in tuple 

#other controls
version = str(pythonInfos[0]) + "." + str(pythonInfos[1])# + "." + str(pythonInfos[2])
print version

#create file to prove that python exists in current machine
file = open("pythonVersion.txt","w")
file.write(version)	
file.close() # this is icing, you can just exit and this will be

if(version != "2.5"):
  print "############ Warning : Python Version must be 2.5 ##############"
  print "############ Check that python 2.5 is installed and is present in PATH environment variable ##############"
  print "############ Several features can't work with different versions  ##############"
  updatePythonVersion.openPythonDownloadPage()
  time.sleep(3)
else:
  pass

try:
  import wx
except:
  print "############ Warning : wxPython not present ##############"
  print "############ Several features can't work without wxPython  ##############"
  updatePythonVersion.openWxPythonDownloadPage()
  time.sleep(3)


