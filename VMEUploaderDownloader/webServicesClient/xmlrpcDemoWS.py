#!/usr/bin/env python2.4
################################################################
# XML-RPC Client Demo (PYTHON) to send request to Zope Server  #
# written by Francesco Benincasa <f.benincasa@scsolutions.it>  #
################################################################

import httplib, urlparse, string
from base64 import encodestring, decodestring
import xml.dom.minidom as xd

from HttpsProxy import *
import os
import urllib, urllib2, base64, re, os, cookielib, sys

Debug = False

class xmlrpc_demoWS:
    """"""

    def __init__(self):
        """"""
        self.ProxyURL = ''
        self.ProxyPort = 0
        self.Username = ''
        self.Password = ''
        self.ServerURL = ''

    def post_multipart(self, bod='search', url='', username='', password='', **kw):
        """
        Return the server's response page.
        """

        #xml request
        self.xmlUpload = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>XMLUpload</methodName>
       <params>
       <param>
        <value><string>%s</string></value>
       </param>
       <param>
        <value><string>%s</string></value>
       </param>
       <param>
        <value><string>%s</string></value>
       </param>
       <param>
        <value><string>%s</string></value>
       </param>
       </params>
     </methodCall>'''% (kw['id'], kw['title'], kw['description'], string.replace(encodestring(kw['upload']),"\012",""))
     
        self.xmlUploadDataresource = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>XMLUploadDataresource</methodName>
       <params>
       <param>
        <value><string>%s</string></value>
       </param>
       <param>
        <value><string>%s</string></value>
       </param>
       <param>
        <value><string>%s</string></value>
       </param>
       <param>
        <value><string>%s</string></value>
       </param>
       </params>
     </methodCall>'''% (kw['id'], kw['title'], kw['description'], string.replace(encodestring(kw['upload']),"\012",""))


        self.xmlDownload = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>XMLDownload</methodName>
       <params>
       <param>
        <value><string>%s</string></value>
       </param>
       </params>
     </methodCall>'''% (kw['download'])

        self.xmlDownloadCheck = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>XMLDownloadCheck</methodName>
       <params>
       <param>
       <value><struct>
       %s
       </struct></value>
       </param>
       </params>
     </methodCall>''' % (kw['listitems'])

        self.xmlDelete = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>XMLDelete</methodName>
       <params>
       <param>
        <value><string>%s</string></value>
       </param>
       </params>
     </methodCall>''' % (kw['filename'])

        self.listBasket = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>listBasket</methodName>
       <params>
       <param>
       </param>
       </params>
     </methodCall>'''

        self.listSandbox= \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>listSandbox</methodName>
       <params>
       <param>
       </param>
       </params>
     </methodCall>'''

        self.updateBasket = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>updateBasket</methodName>
       <params>
       <param>
       <value><array><data>
       %s
       </data></array></value>
       </param>
       </params>
     </methodCall>''' % (kw['listitems'])

        self.deleteFromBasket = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>deleteFromBasket</methodName>
       <params>
       <param>
       <value><array><data>
       %s
       </data></array></value>
       </param>
       </params>
     </methodCall>''' % (kw['listitems'])

        self.xmlRead = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>xml_read</methodName>
       <params>
       <param>
        <value><string>%s</string></value>
       </param>
       </params>
     </methodCall>''' % (kw['listitems'])

        self.xmlEdit = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>xml_edit</methodName>
       <params>
       <param>
       <value><struct>
       %s
       </struct></value>
       </param>
       </params>
     </methodCall>''' % (kw['listitems'])

        self.getTitle = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>Title</methodName>
       <params>
       <param>
       </param>
       </params>
     </methodCall>'''

        self.createResource = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>createResource</methodName>
       <params>
       <param>
       </param>
       </params>
     </methodCall>'''

        self.authService = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>authService</methodName>
       <params>
       <param>
       </param>
       </params>
     </methodCall>'''

        wh_file = ''

        if bod == 'xmlupload':
            body = self.xmlUpload
        elif bod == 'xmluploadDataresource':
            body = self.xmlUploadDataresource
        elif bod == 'xmldownload':
            body = self.xmlDownload
        elif bod == 'xmldownloadcheck':
            body = self.xmlDownloadCheck
        elif bod == 'xmldelete':
            body = self.xmlDelete
        elif bod == 'listbasket':
            body = self.listBasket
        elif bod == 'listsandbox':
            body = self.listSandbox
        elif bod == 'updatebasket':
            body = self.updateBasket
        elif bod == 'deletefrombasket':
            body = self.deleteFromBasket
        elif bod == 'xmledit':
            body = self.xmlEdit
        elif bod == 'xmlread':
            body = self.xmlRead
        elif bod == 'gettitle':
            body = self.getTitle
        elif bod == 'createresource':
            body = self.createResource
        elif bod == 'authservice':
            body = self.authService

        if Debug: print "++++++\n" + body + "\n"

        #####################
        self.cj = cookielib.CookieJar()

        proxy_url = self.ProxyURL
        if self.ProxyPort == 0:
            proxy_port = ''
        else: proxy_port = str(self.ProxyPort)

        p = '%s:%s' % (proxy_url, proxy_port)

        if proxy_url != '' and proxy_port != '':

            if Debug: print "You are using proxy: " + p

            self.opener = \
              urllib2.build_opener(
              ConnectHTTPHandler(proxy=p),
              ConnectHTTPSHandler(proxy=p),
              urllib2.HTTPCookieProcessor(self.cj))
        else:
            self.opener = \
              urllib2.build_opener(urllib2.HTTPCookieProcessor(self.cj))

        urllib2.install_opener(self.opener)  
        
        

        if Debug: print "Connecting to URL: " + url

        req = urllib2.Request(url=url,data=body)

        content_type = 'text/xml'

        # Basic authentication code
        base64string = encodestring('%s:%s' % (self.Username, self.Password))[:-1]
        authheader =  "Basic %s" % base64string
        req.add_header("Authorization", authheader)
        req.add_header('content-type', content_type)
        req.add_header('content-length', str(len(body)))

        # open the url
        if Debug: print 'Sending body: ... \n%s\n' % (str(body))
        response = urllib2.urlopen(req)

        #if Debug: print 'Sending body: ... '
        if bod == 'xmldownload':
            res = response.read()
            dom = xd.parseString(res)
#            for el in dom.getElementsByTagName("string"):
#                for node in el.childNodes:
            if dom.getElementsByTagName("fault"):
                return res
            fname = dom.getElementsByTagName("string")[0].childNodes[0].data
            fdata = \
              decodestring(dom.getElementsByTagName("string")[1].childNodes[0].data)
            ##dom.getElementsByTagName("string")[1].childNodes[0].setData('XXXX')
            f = open(fname, 'wb')
            f.write(fdata)
            f.close()
            return True
        else:
            res = response.read()
            dom = xd.parseString(res)
            if dom.getElementsByTagName("fault"):
                return False, res
            else:
                return True, res

    def setCredentials(self, user, password):
        self.Username = user
        self.Password = password
    
    def setServer(self, serverURL):
        self.ServerURL = serverURL  
    
    def setProxy(self, proxyUrl, proxyPort):
        self.ProxyURL = proxyUrl
        self.ProxyPort = proxyPort
        
    def run(self, command, filename='', title='', description=''):
        """"""

        args = {}
    
        # username and password of a test user
        username = self.Username
        password = self.Password
    
        
        # development server
        url = self.ServerURL

        if Debug: print "COMMAND: %s" % command

        args['listitems'] = ''

        if command == 'xmluploadDataresource':
            args['download'] = ''
            f = file(filename,'rb')
            args['upload'] = f.read()
            #args['id'] = '' #f.name
            args['id'] = f.name
            args['title'] = title
            args['description'] = description
            args['filename'] = ''
            f.close()
            #self.setServer(ws.ServerURL + '/' + f.name)
        elif command == 'xmlupload':
            args['download'] = ''
            f = file(filename,'rb')
            args['upload'] = f.read()
            #args['id'] = '' #f.name
            args['id'] = f.name
            args['title'] = title
            args['description'] = description
            args['filename'] = ''
            f.close()
        elif command == 'xmldownload':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = filename
        elif command == 'xmldownloadcheck':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = ''
            for item in filename.split(":"):
                args['listitems'] += \
                 '<member><name>%s</name><value><string>%s</string></value></member>' % \
                 (item.split(',')[0],item.split(',')[1])
        elif command == 'xmldelete':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = filename
            args['download'] = ''
        elif command == 'listbasket':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = ''
        elif command == 'updatebasket':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = ''
            args['listitems'] = ''
            for item in filename.split(','):
              args['listitems'] += '<value><string>%s</string></value>' % item
        elif command == 'deletefrombasket':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = ''
            args['listitems'] = ''
            for item in filename.split(','):
              args['listitems'] += '<value><string>%s</string></value>' % item
        elif command == 'xmlread':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = ''
            args['listitems'] = filename.split(',')[1]
        elif command == 'xmledit':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = ''
            args['listitems'] = \
             '<member><name>name</name><value><string>%s</string></value></member><member><name>new</name><value><string>%s</string></value></member>' % \
             (filename.split(',')[1],filename.split(',')[2])
        elif command == 'gettitle':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = ''
            args['listitems'] = ''
        elif command == 'createresource':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = ''
            args['listitems'] = ''
        elif command == 'authservice':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = ''
            args['listitems'] = ''
        elif command == 'listsandbox':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = ''
        else:
            if Debug: print 'Error: command not found\n'
            sys.exit(1)

        if Debug: print command, url, username, password, str(args)
        #ws = xmlrpc_demoWS(**args)
        return self.post_multipart(command, url, username, password, **args)

if __name__ == '__main__':
    import sys
    usage_msg = '''Usage: %s <option>
where option can be:
xmlupload <id> - upload data resource
xmldownload <id> - download data resource
xmldelete <id> - delete data resource
listBasket - list user's basket items
xmlread -
xmledit -
gettitle -
createresource -
authservice - 
listsandbox - 
''' % sys.argv[0]

    if len(sys.argv) not in (2,3):
        if Debug: print 'Error :\n' + usage_msg
        sys.exit(1)

    command = sys.argv[1]
    if len(sys.argv) == 3:
        filename = sys.argv[2]
    else: filename = ''

    if Debug: print "---", filename

    ws = xmlrpc_demoWS()
    if sys.argv[1] in ('xmlread','xmledit','gettitle','xmluploadDataresource'):
        ws.setServer(ws.ServerURL + filename.split(',')[0])
    print ws.run(command, filename)
