#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Daniele Giunchi<s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import webbrowser

def openWxPythonDownloadPage():
    downloadPage = r"http://www.wxpython.org/download.php#binaries"
    webbrowser.open(downloadPage)

def openPythonDownloadPage():
    downloadPage = r"http://www.python.org/download/releases/2.5.1/"
    webbrowser.open(downloadPage)

if __name__ == '__main__':    
    openPythonDownloadPage()
 
