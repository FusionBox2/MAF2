#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import sys
sys.path.append('./webServicesClient')

import lhpCSVDictionaryDownloader
import Debug

import Debug
import unittest

import os


import urllib, urllib2, base64, re, os, cookielib, sys

class lhpCSVDictionaryDownloaderTest(unittest.TestCase):
      
    def setUp(self):
        
        print "Beware:  In order to work run this est must be launched from VMEUploaderDownloader dir!"
        
        curDir = os.getcwd()        
        
        print " current directory is: " + curDir
            
    def testBiomedtownLogin(self):
        """login to biomedtown"""
    
        orig = "http://www.biomedtown.org"
        
        cj = cookielib.CookieJar()
        opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
        urllib2.install_opener(opener)
        opener.addheaders = [('User-Agent','Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.0.7) Gecko/20060909 Firefox/1.5.0.7')]
        
        username = 'lhpparabuild'
        password = '2bf5ZM'
    
        url1 = '%s/login_form' % orig
    
        data = {
          'vlti-login_name':username,
          'vlti-login_password':password,
        }
    
        req1 = urllib2.Request(url1)
    
        postdata = urllib.urlencode(data)
    
        download_url = ''
        appRes = {}
    
        # req = lhpCSVDictionaryDownloader.getAuthData(url1, username, password, True)
        # print "--> URL1:", url1, "<-->", req, "<--"
    
        #LOGIN PAGE - 
        try:
            f1 = opener.open(req1)
        except Exception, e:
            raise 'Error: %s' % str(e)
    
        first_index = f1.readlines()
        f1.close()
    
        cid = ''
        
        # X Francesco che ci metto qui???????
        if Debug: print first_index
        for s in first_index:
            s = s.replace('\r\n','')
            if re.match('.*action="/login\.htm\?cid=.*',s):
                cid = re.search('action="/login\.htm\?cid=(.*)"',s).group(1)
    
        if Debug: print "*******", cid, "*******"
    
        req2 = urllib2.Request(url1+'?cid='+cid)
        
        f2 = opener.open(req2, postdata)
    
        afterLoginPage = f2.readlines()
        f2.close()

    
        if Debug: print "-->", afterLoginPage, "<--"

        
    def estRun(self):
        """This is what I have to do...Not working with the following log
        
======================================================================
FAIL: testRun (__main__.lhpCSVDictionaryDownloaderTest)
----------------------------------------------------------------------
Traceback (most recent call last):
  File "d:\vapps\LHPBuilder_Parabuild\VMEUploaderDownloader\lhpCSVDictionaryDownloaderTest.py", line 37, in testRun
    self.assertEqual(out, True)
AssertionError: "<?xml version='1.0'?>\n<methodResponse>\n<fault>\n<value><struct>\n<member>\n<name>faultCode</name>\n<value><int>-1</int></value>\n</member>\n<member>\n<name>faultString</name>\n<value><string>Unexpected Zope exception: zExceptions.NotFound -  \n \n\n \n \n \n\n \n   Site Error \n   An error was encountered while publishing this resource.\n   \n    Debugging Notice  \n\n  Zope has encountered a problem publishing your object. \nThe object at http://www.biomedtown.org/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries/XMLDownload has an empty or missing docstring. Objects must have a docstring to be published.\n   \n\n   Troubleshooting Suggestions \n\n   \n   The URL may be incorrect. \n   The parameters passed to this resource may be incorrect. \n   A resource that this resource relies on may be\n      encountering an error. \n   \n\n   For more detailed information about the error, please\n  refer to error log.\n   \n\n   If the error p
ersists please contact the site maintainer.\n  Thank you for your patience.\n   \n  \n </string></value>\n</member>\n</struct></value>\n</fault>\n</methodResponse>\n" != True

----------------------------------------------------------------------

        """
        ws = lhpCSVDictionaryDownloader.lhpCSVDictionaryDownloader()

        testFile = "LHDL_dictionary.dic"
        
        #download
        print "download remote " + testFile + "..."
        out = ws.run('xmldownload', testFile)
        self.assertEqual(out, True)
        print "done!"
        
        #md5
        #print "md5 checking..."
        #f1 = open(testFile, 'rb')
        #f2 = open(testFileOLD, 'rb')
        #self.assertEqual(md5.new(f1.read()).digest(), md5.new(f2.read()).digest())
        #f1.close()
        #f2.close()
        #print "done!"

    
    def estRun2(self):
        """This is just for comparison and it`s working... """
        ws = lhpCSVDictionaryDownloader.lhpCSVDictionaryDownloader()
        
        testFile = "testXml.msf"
        
        #download
        print "download remote " + testFile + "..."
        out = ws.run2('xmldownload', testFile)
        self.assertEqual(out, True)
        print "done!"
        
        #md5
        #print "md5 checking..."
        #f1 = open(testFile, 'rb')
        #f2 = open(testFileOLD, 'rb')
        #self.assertEqual(md5.new(f1.read()).digest(), md5.new(f2.read()).digest())
        #f1.close()
        #f2.close()
        #print "done!"

if __name__ == '__main__':
    unittest.main()
    
