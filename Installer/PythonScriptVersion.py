import sys, time

#print sys.version       # version info in string
pythonInfos = sys.version_info  # version info in tuple 

#other controls
version = str(pythonInfos[0]) + "." + str(pythonInfos[1]) + "." + str(pythonInfos[2])
print version
if(version != "2.5.0"):
  print "############ Warning : Python Version must be 2.5.0 ##############"
  print "############ Several features can't work with different versions  ##############"
  time.sleep(3)
else:
  pass

try:
  import wx
except:
  print "############ Warning : wxPython not present ##############"
  time.sleep(3)


