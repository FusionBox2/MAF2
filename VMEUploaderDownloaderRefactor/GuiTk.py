import Tkinter
from ProgressBarViewTk import *

class GuiPart(Frame):
    def __init__(self, master, queue, endCommand):
        Frame.__init__(self, master)
        self.master = master
        self.queue = queue
        # Set up the GUI
        self.console = Tkinter.Button(master, text='Waiting...', command=endCommand)
        self.console.pack()
        # Add more GUI stuff here
        self.bars = []
        self.labels = []
        self.quitApplication = endCommand
        
    def createBar(self):
            self.bars.append(ProgressBarViewTk(self.master, value=0))
            self.master.geometry("300x"+ str((len(self.bars)+1) * 50 + 10))
            self.bars[len(self.bars)-1].pack(fill=X)
                 
    def createLabel(self , title):
        self.labels.append(Label(self.master, text="Up ->" + title, justify=LEFT, width=100))
        self.labels[len(self.labels)-1].pack()
 
    def destroy(self):
        self.quitApplication()
        Frame.destroy(self)

    def processIncoming(self):
        """
        Handle all the messages currently in the queue (if any).
        """
        while self.queue.qsize():
            try:
                lista = self.queue.get(0)
                # Check contents of message and do what it says
                # As a test, we simply print it
                #self.console["text"] = lista[0]
                self.console["text"] = "Running..."
                lista[0].updateProgress(lista[1])
                #print lista
            except Queue.Empty:
                pass
        else:
            finished = True
            for i in self.bars:
                if(i.value != 100):
                    finished = False
                    break
			
            if (finished): self.console["text"] = "Waiting..."

def test():
    def endFunction(): print "ending"
    import Queue
    root = Tkinter.Tk()
    gui = GuiPart(root , Queue.Queue(), endFunction)
    root.mainloop()

if __name__ == '__main__':
  test()