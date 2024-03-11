import os
import sys
import stat

class Lock:
	def __init__(self, fileName):
	  self.pid = -1
	  self.fileName = fileName
	  self.createLockFile()
	  self.file = None
	
	def createLockFile(self):
  	#create lock file
		self.pid = os.getpid()
		# Create a file object:
		# in "write" mode
		self.file = open(self.fileName,"w")
		self.file.write(str(self.pid))
		
		self.file.close() # this is icing, you can just exit and this will be
		#os.chmod(self.fileName, stat.S_IREAD)
		# handled automagically.
	
	def deleteLockFile(self):
		if(os.path.exists(self.fileName)):
			#os.chmod(self.fileName, stat.S_IWRITE)
			os.remove(self.fileName)