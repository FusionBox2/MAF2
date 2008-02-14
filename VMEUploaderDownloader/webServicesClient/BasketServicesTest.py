import unittest
import xmlrpcDemoWS
import os, commands, md5

class XmlUploadDownloadTest(unittest.TestCase):
    """"""
    def testUploadDownload(self):

        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        testitems = "Data_57_exportedVME.xml,Data_63_exportedVME.xml"
	res1 = '''<?xml version='1.0'?>
<methodResponse>
<params>
<param>
<value><array><data>
</data></array></value>
</param>
</params>
</methodResponse>
'''
	
	res2 = '''<?xml version='1.0'?>
<methodResponse>
<params>
<param>
<value><array><data>
<value><string>Data_57_exportedVME.xml</string></value>
<value><string>Data_63_exportedVME.xml</string></value>
</data></array></value>
</param>
</params>
</methodResponse>
'''

        #ws.setCredentials('PutUser','PutPassword')
        #list
        print "list basket"
        out = ws.run('listbasket')
        self.assertEqual(out[0], True)
        self.assertEqual(out[1], res1)
        #print out[1]
        print "done!"
        
        #add
        print "add to basket"
        out = ws.run('updatebasket', testitems)
        self.assertEqual(out[0], True)
        print "done!"


        #list
        print "list basket"
        out = ws.run('listbasket')
        self.assertEqual(out[0], True)
        self.assertEqual(out[1], res2)
        #print out[1]
        print "done!"
            
        #delete 
        print "delete from basket"
        out= ws.run('deletefrombasket', testitems)
        self.assertEqual(out[0], True)
        print "done!"

if __name__ == '__main__':
    unittest.main()
