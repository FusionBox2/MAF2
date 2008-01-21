# simple illustration of thread module
# this is the client; usage is

# python clnt.py server_address port_number
import socket # networking module
import sys

# create Internet TCP socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

host = sys.argv[1] # server address
port = int(sys.argv[2]) # server port
id = sys.argv[3] # vme id
msf = sys.argv[4] # msf dir
user = sys.argv[5] #user
password = sys.argv[6] #pwd
urlServer = sys.argv[7] #server url where upload files

name = ""
count = 0
for i in sys.argv:
  if(count > 7):
    if(name == ""):
      name = i
    else:
      name = name + ' ' + i #vme name
  count = count + 1



# connect to server
s.settimeout(20)
s.connect((host, port))


# compose message
k = id + ' ' + msf + ' ' + user + ' ' + password + ' ' + urlServer + ' '+ name
s.sendall(k) # send k to server
# if stop signal, then leave loop
v = s.recv(1024) # receive v from server (up to 1024 bytes)
print v

s.close() # close socket