#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import sys
sys.path.append('./webServicesClient')

import xmlrpcDemoWS
    
class lhpCSVDictionaryDownloader(xmlrpcDemoWS.xmlrpc_demoWS):
    
    def __init__(self):
        """"""
    
    def run(self, command, filename):
        """Download the dictionary from the url folder"""    
        args = {}
    
        # username and password of a test user
        username = 'lhpparabuild'
        password = '2bf5ZM'
    
        # production server parabuild user
        url = 'http://www.biomedtown.org/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries'
        
        if command == 'xmldownload':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = filename
        else:
            print 'Error :\n' + usage_msg
            sys.exit(1)
    
        return self.post_multipart(command, url, username, password, **args)

    def run2(self, command, filename):
        """This is used just for comparison: this is working correctly"""    
        args = {}
            # username and password of a test user
        username = 'testuser'
        password = 'GRDPt8'
    
        # production server
        # url = 'http://www.biomedtown.org/biomed_town/LHDL/users/repository/lhprepository'
        
        # development server
        url = 'http://devel.fec.cineca.it:12680/town/Members/portal_admin/test-lhp2'

        if command == 'xmldownload':
            args['id'] = ''
            args['title'] = ''
            args['description'] = ''
            args['upload'] = ''
            args['filename'] = ''
            args['download'] = filename
        else:
            print 'Error :\n' + usage_msg
            sys.exit(1)
    
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
