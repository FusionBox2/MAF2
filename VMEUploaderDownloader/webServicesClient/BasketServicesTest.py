import unittest
import xmlrpcDemoWS
import os, commands, md5

class XmlUploadDownloadTest(unittest.TestCase):
    """"""
    def testUploadDownload(self):

        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        testFile = "testXml.msf"

        #list
        print "list basket"
        out = ws.run('listbasket')
        self.assertEqual(out, True)
        print "done!"
        
        #add
        print "add to basket"
        out = ws.run('updatebasket')
        self.assertEqual(out, True)
        print "done!"


        #list
        print "list basket"
        out = ws.run('listbasket')
        self.assertEqual(out, True)
        print "done!"
            
        #delete 
        print "delete from basket"
        out= ws.run('deletefrombasket')
        self.assertEqual(out, True)
        print "done!"

if __name__ == '__main__':
    unittest.main()
