def retriveProxyParameters():
  proxyHost = ""
  proxyPort = 0
  try:
      file = open("vmeUploaderConnectionConfiguration.conf","r")
      proxyHost = file.readline() # proxy host
      proxyPort = file.readline()  # proxy port
      file.close()
  except:
      pass

  print proxyHost
  print proxyPort
  return (proxyHost,proxyPort)