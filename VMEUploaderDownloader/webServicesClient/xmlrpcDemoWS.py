#!/usr/bin/env python
################################################################
# XML-RPC Client Demo (PYTHON) to send request to Zope Server  #
# written by Francesco Benincasa <f.benincasa@scsolutions.it>  #
################################################################

import httplib, urlparse, string
from base64 import encodestring, decodestring
import xml.dom.minidom as xd

class xmlrpc_demoWS:
    """"""

    def __init__(self):
        """"""

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

        self.xmlDelete = \
    '''<?xml version="1.0"?>
     <methodCall>
      <methodName>manage_delObjects</methodName>
       <params>
       <param>
        <value><string>%s</string></value>
       </param>
       </params>
     </methodCall>''' % (kw['filename'])

        wh_file = ''

        if bod == 'xmlupload':
            body = self.xmlUpload
        elif bod == 'xmldownload':
            body = self.xmlDownload
        elif bod == 'delete':
            body = self.xmlDelete

        # internet location (temporary BiomedTown development instance)
        wh_url = url

        wh_url += wh_file

        content_type = 'text/xml'

        #xml request
        urlparts = urlparse.urlsplit(wh_url)
        host = urlparts[1]
        selector = urlparts[2]

        #print 'Connecting to %s' % str(wh_url)
        h = httplib.HTTPConnection(host)
        h.putrequest('POST', selector)
        h.putheader('content-type', content_type)
        h.putheader('content-length', str(len(body)))
        h.putheader("AUTHORIZATION", "Basic %s" % string.replace(
                                encodestring("%s:%s" % (username, password)),
                                "\012", ""))
        h.endheaders()
        h.send(body)
        #print 'Sending body: ... \n%s\n' % (str(body))
        #print 'Sending body: ... '
        if bod == 'xmldownload':
            res = h.getresponse().read()
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
            res = h.getresponse().read()
            dom = xd.parseString(res)
            if dom.getElementsByTagName("fault"):
                return res
            else:
                return True

    def run(self, command, filename):
        """"""

        args = {}
    
        # username and password of a test user
        username = 'portal_admin'
        password = 'r1zz0l1'
    
        #url = 'http://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository'
        url = 'http://devel.fec.cineca.it:12680/town/Members/portal_admin/test-lhp2'
    
        if command == 'xmlupload':
            args['download'] = ''
            f = file(filename,'rb')
            args['upload'] = f.read()
            args['id'] = f.name
            args['title'] = f.name
            args['description'] = f.name
            args['filename'] = ''
            f.close()
        elif command == 'xmldownload':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = filename
        elif command == 'delete':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = filename
            args['download'] = ''
        else:
            print 'Error :\n' + usage_msg
            sys.exit(1)

        #ws = xmlrpc_demoWS(**args)
        return self.post_multipart(command, url, username, password, **args)

if __name__ == '__main__':
    import sys
    usage_msg = '''Usage: %s <option>
where option can be:
upload <filename.zmsf> - upload zmsf file
download <filename.zmsf> - download zmsf file
''' % sys.argv[0]

    if len(sys.argv) != 3:
        print 'Error :\n' + usage_msg
        sys.exit(1)

    command = sys.argv[1]
    filename = sys.argv[2]

    ws = xmlrpc_demoWS()
    print ws.run(command, filename)

#    args = {}
#
#    # username and password of a test user
#    username = 'portal_admin'
#    password = 'r1zz0l1'
#
#    #url = 'http://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository'
#    url = 'http://devel.fec.cineca.it:12680/town/Members/portal_admin/test-lhp2'
#
#    filename = sys.argv[2]
#
#    if sys.argv[1] == 'xmlupload':
#        args['download'] = ''
#        f = file(filename,'rb')
#        args['upload'] = f.read()
#        args['id'] = f.name
#        args['title'] = f.name
#        args['description'] = f.name
#        args['filename'] = ''
#        f.close()
#    elif sys.argv[1] == 'xmldownload':
#        args['id'] = ''
#        args['title'] = ''
#        args['description'] = ''
#        args['upload'] = ''
#        args['filename'] = ''
#        args['download'] = filename
#    elif sys.argv[1] == 'delete':
#        args['id'] = ''
#        args['title'] = ''
#        args['description'] = ''
#        args['upload'] = ''
#        args['filename'] = filename
#        args['download'] = ''
#    else:
#        print 'Error :\n' + usage_msg
#        sys.exit(1)
#
#    ws = xmlrpc_demoWS(**args)
#    print ws.post_multipart(sys.argv[1], url, username, password)
