import unittest
import xmlrpcDemoWS
import os, commands, md5

class ListSandboxTest(unittest.TestCase):
    """"""
    def test(self):

        ws = xmlrpcDemoWS.xmlrpc_demoWS()
        ws.setServer("http://devel.fec.cineca.it:12680/town/biomed_town/LHDL/users/repository/lhprepository2/")
        ws.setCredentials("lhpparabuild","2bf5ZM")

        #res = "<?xml version='1.0'?>\n<methodResponse>\n<params>\n<param>\n<value><array><data>\n<value><string>Data_426.xml</string></value>\n<value><string>prova_benincasa</string></value>\n</data></array></value>\n</param>\n</params>\n</methodResponse>\n"

        print "list sandbox service"
        out = ws.run('listsandbox')
        self.assertEqual(out[0], True)
        #self.assertEqual(out[1], res)
        #print out[1]
        print "done!"


if __name__ == '__main__':
    unittest.main()
