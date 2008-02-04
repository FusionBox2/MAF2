#simple illustration of threading module
# this is the server

import Server
import unittest

class ServerTest(unittest.TestCase):
      
    # def setUp(self):
            
    def testDecompose(self):
        s = Server.Server(clntsock = None, observer = None)
        valueList = []
        valueList.append("UPLOAD") #modality
        valueList.append(str(5)) #id
        valueList.append("C:\\space?proof\\") #directory with "?" instead of space
        valueList.append("user") #user
        valueList.append("password") #password
        valueList.append("http://devel.fec.cineca.it:12680/town/Members/portal_admin/test-lhp2") #server
        valueList.append("vme spazio") #vme name with space
        data = " ".join(valueList)
        newValueList = s.decompose(data)
        print valueList
        print newValueList
        
if __name__ == '__main__':
    unittest.main()