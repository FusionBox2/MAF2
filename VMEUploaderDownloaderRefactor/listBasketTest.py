import os
import listBasket
import unittest

class listBasketTest(unittest.TestCase):
    
    def setUp(self):  
        print "Beware:  In order to work run this test must be launched from VMEUploaderDownloader dir!"  
        
        self.curDir = os.getcwd()
        self.user = 'testuser' #substitute
        self.pwd  = '6w8DHF' #substitute
        
        self.lBasket = listBasket.listBasket()
        self.lBasket.SetCredentials(self.user, self.pwd)
        
        print " current directory is: " + self.curDir
       
    def testListing(self):
        self.lBasket.getListFromBasket()
        for test in self.lBasket.IdList:
            print test
        pass
    
    def testWritingOnFile(self):
        self.lBasket.getListFromBasket()
        self.lBasket.IdListSelected = self.lBasket.IdList
        self.lBasket.writeIdListSelectedOnFile()
         
        file = open(self.lBasket.fileName, 'r')
        
        stream = file.read()
        
        print "***FILE***"
        print stream     
        file.close()
        print "***END FILE***"
    
if __name__ == '__main__':
    unittest.main()
