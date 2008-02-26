import unittest
import xmlrpcDemoWS
import os, commands, md5

class XmlUploadDownloadTest(unittest.TestCase):
    """"""
    def testUploadDownload(self):

        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.Username = 'testuser'
        ws.Password = 'GRDPt8'
        ws.ServerURL = 'http://devel.fec.cineca.it:12680/town/Members/portal_admin/test-lhp2'
        testFile = "testXml.msf"

        #delete
        print "delete remote " + testFile + "..."
        try:
            ws.run('xmldelete', testFile)
        except:
            pass
        print "done!"

        #upload
        print "upload " + testFile + "..."
        out = ws.run('xmlupload', testFile)
        self.assertEqual(out[0], True)
        print "done!"
        
        #copy
        testFileOLD = testFile + "OLD"
        try:
            print "try to remove local " + testFileOLD + "..."
            os.remove(testFileOLD)
        except:
            pass
        print "rename local " + testFile + " as " + testFileOLD
        os.renames(testFile, testFileOLD)
        print "done!"
        
        #download
        print "download remote " + testFile + "..."
        out = ws.run('xmldownload', testFile)
        self.assertEqual(out, True)
        print "done!"
        
        #md5
        print "md5 checking..."
        f1 = open(testFile, 'rb')
        f2 = open(testFileOLD, 'rb')
        self.assertEqual(md5.new(f1.read()).digest(), md5.new(f2.read()).digest())
        f1.close()
        f2.close()
        print "done!"
            
if __name__ == '__main__':
    unittest.main()
