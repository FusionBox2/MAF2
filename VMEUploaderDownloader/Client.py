# simple illustration of thread module
# this is the client; usage is

# python clnt.py server_address port_number
import socket # networking module
import sys

# create Internet TCP socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#used by client
host = sys.argv[1] # server address
port = int(sys.argv[2]) # server port

#sended
modality = sys.argv[3] #modality "UPLOAD" or "DOWNLOAD"
id = sys.argv[4] # vme id id if UPLOAD , size if DOWNLOAD
msf = sys.argv[5] # msf dir
user = sys.argv[6] #user
password = sys.argv[7] #pwd
urlServer = sys.argv[8] #server url where upload files
originalId = sys.argv[9] #original Id in UPLOAD, VME name in DOWNLAOD
hasLink = sys.argv[10] #vme has link in UPLOAD, SRB data URI in DOWNALOAD
if (modality == "UPLOAD"):
    withChild = sys.argv[11] #vme with child (UPLOAD)
    name = sys.argv[12] #vme Name (UPLAOD)

    
#name = ""
#count = 0
#for i in sys.argv:
#  if(count > 11):
#    if(name == ""):
#      name = i
#    else:
#      name = name + ' ' + i #vme name if UPLOAD, dataURI if DOWNLOAD
#  count = count + 1



# connect to server
s.settimeout(20)
s.connect((host, port))



# compose message
if (modality == "UPLOAD"):
    k = modality + ' ' + id + ' ' + msf + ' ' + user + ' ' + password + ' ' + urlServer + ' '  + originalId + ' ' + hasLink+ ' ' + withChild + ' ' + name
else:
    k = modality + ' ' + id + ' ' + msf + ' ' + user + ' ' + password + ' ' + urlServer + ' '  + originalId + ' ' + hasLink
     
s.sendall(k) # send k to server
# if stop signal, then leave loop
v = s.recv(1024) # receive v from server (up to 1024 bytes)
print v

s.close() # close socket