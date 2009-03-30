#!/usr/bin/env python

import threading
from time import time, ctime, sleep
import random
from Debug import Debug

class CustomThread(threading.Thread):
   
    def __init__(self, func, args, name='', observer=None):
        threading.Thread.__init__(self)
        self.name = name
        self.func = func
        self.args = args
        self.observer = observer;
        self.progress = 0

    def getResult(self):
        return self.res
        #pass
        
    def run(self):
        if Debug:
            print 'starting', self.name, 'at:', ctime(time())      
        self.res = apply(self.func, self.args)
             
        if Debug:
            print self.name, 'finished at:', ctime(time())
        #print self.name, 'result :' , self.getResult()
