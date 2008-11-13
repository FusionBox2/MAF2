#simple illustration of threading module
# this is the server

import socket # networking module
import threading
import time
from Debug import Debug



# class for threads, subclassed from threading.Thread class
class Server(threading.Thread):
  # v and vlock are now class variables
  
  id = 0 # I want to give an ID number to each thread, starting at 0

  def __init__(self, clntsock, observer):
    # invoke constructor of parent class
    threading.Thread.__init__(self)
    # add instance variables
    self.myid = Server.id
    Server.id += 1
    self.myclntsock = clntsock
    self.command = ''
    self.observer = observer
    # this function is what the thread actually runs; the required name
    # is run(); threading.Thread.start() calls threading.Thread.run(),
    # which is always overridden, as we are doing here
  
  def decompose(self, data):
      """ Create a list of parameters arrived by client """
      #0 modality "UPLOAD" or "DOWNLOAD"
      #1 vme id id if UPLOAD , size if DOWNLOAD
      #2 msf dir
      #3 user
      #4 pwd
      #5 server url where upload files
      #6 original Id in UPLOAD, XML data URI in DOWNALOAD
      #7 vme has link in UPLOAD, SRB data URI in DOWNALOAD
      #8 vme with child (UPLOAD)
      #9 file for upload rollback (UPLOAD)
      #10 XML resource URI (UPLOAD)
      #11 vme Name (UPLAOD)
      
      arguments = data.split(" ") 
      sendList = []
      count = 0
      lastArgument = ""
      for i in arguments:
        if(count < 11):
          sendList.append(i)
        else:
          if(lastArgument != ''):
              lastArgument = lastArgument + ' ' + i
          else:
              lastArgument = lastArgument + i
        count = count + 1
      sendList.append(lastArgument)
      if (sendList[0] == "UPLOAD"):
           sendList[11] = sendList[11].replace("??", " ") # VME name with spaces arrives with "??" instead of them
      sendList[2] = sendList[2].replace("??", " ") # directory with spaces arrives with "??" instead of them
      if Debug:
          print sendList[2]
      return sendList
      
  def run(self):
    while 1:
       # receive letter from client, if it is still connected 
       k = self.myclntsock.recv(1024)
       if(k != ''): 
       	 #decompose message and create list
       	 self.observer.createThread(self.decompose(k))
       self.command = k
       self.myclntsock.sendall(self.command)
    self.myclntsock.close()
        
def createServer(observer = None, port = 50000, numberOfClients = 10):
	# set up Internet TCP socket
	lstn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	# bind lstn socket to this port
	lstn.bind(('', port))
	# start listening for contacts from clients (at most 1 at a time)
	lstn.listen(10)
	
	nclnt = int(numberOfClients) # number of clients
	
	mythreads = [] # list of all the threads
	# accept calls from the clients

	while 1:
	  # wait for call, then get a new socket to use for this client,
	  # and get the client's address/port tuple (though not used)
	  (clnt,ap) = lstn.accept()
	  # make a new instance of the class Server
	  s = Server(clnt,observer)
	  # keep a list all threads
	  mythreads.append(s)
	  # threading.Thread.start calls threading.Thread.run(), which we
	  # overrode in our definition of the class Server
	  s.start()

	  
	# shut down the server socket, since it's not needed anymore
	lstn.close()
	
	# wait for all threads to finish
	for s in mythreads:
	  s.join()
	
    print 'threads all ended'
	
if __name__ == '__main__':
  createServer()