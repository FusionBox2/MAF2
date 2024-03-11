from Debug import Debug
def retriveProxyParameters():
  proxyHost = ""
  proxyPort = 0
  try:
      file = open("vmeUploaderConnectionConfiguration.conf","r")
      proxyHost = file.readline() # proxy host
      proxyPort = int(file.readline())  # proxy port
      proxyHost = proxyHost.replace(" ", "")
      proxyHost = proxyHost.replace("\n", "")
      file.close()
  except:
      pass

  if Debug:
      print proxyHost
      print proxyPort
  return (proxyHost,proxyPort)