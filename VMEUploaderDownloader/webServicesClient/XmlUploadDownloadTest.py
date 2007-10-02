import unittest
import xmlrpcDemoWS
import os, commands, md5

class XmlUploadDownloadTest(unittest.TestCase):
    """"""
    def testUploadDownload(self):

        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        testFile = "testXml.msf"

        #delete
        try:
            ws.run('delete', testFile)
            #commands.getoutput("./xmlrpcDemoWS.py delete %s" % (testFile))
        except:
            pass

        #upload
        out = ws.run('xmlupload', testFile)
        #out = commands.getoutput("./xmlrpcDemoWS.py xmlupload %s" % (testFile))
        self.assertEqual(out, True)

        #copy
        testFileOLD = testFile + "OLD"
        #os.stat(testFile)
        os.renames(testFile, testFileOLD)
        #commands.getoutput("mv %s %s" % (testFile, testFileOLD))

        #download
        out = ws.run('xmldownload', testFile)
        #out = commands.getoutput("./xmlrpcDemoWS.py xmldownload %s" % (testFile))
        self.assertEqual(out, True)

        #md5
        f1 = open(testFile, 'rb')
        f2 = open(testFileOLD, 'rb')
        self.assertEqual(md5.new(f1.read()).digest(), md5.new(f2.read()).digest())
        f1.close()
        f2.close()

if __name__ == '__main__':
    unittest.main()
