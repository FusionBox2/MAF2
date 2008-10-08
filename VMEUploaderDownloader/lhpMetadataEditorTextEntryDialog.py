import  wx.lib.masked             as  masked
from sre_parse import isdigit
from tokenize import Number

import lhpMetadataEditor
import  wx
import lhpXMLDictionaryParser
#---------------------------------------------------------------------------
# Create and set a help provider.  Normally you would do this in
# the app's OnInit as it must be done before any SetHelpText calls.
provider = wx.SimpleHelpProvider()
wx.HelpProvider.Set(provider)

#---------------------------------------------------------------------------

VSPACE = 10

class Dialog(wx.Dialog):
    def __init__(
            self, parent, ID, title, size=wx.DefaultSize, pos=wx.DefaultPosition, 
            style=wx.DEFAULT_DIALOG_STYLE,
            useMetal=False, selectedTag = ""):
        
        self.TextControl = None
        self.SelectedTag = selectedTag
        
        mep = parent
        assert isinstance(mep , lhpMetadataEditor.MetadataEditorPanel)
        
        # get the xml dict node
        pi = mep.lhpXMLDictionaryParserInstance
        assert isinstance(pi , lhpXMLDictionaryParser.lhpXMLDictionaryParser)
        node = pi.GetNodeFromVMETagArrayTagName(selectedTag)
        assert(node)
        
        ad = pi.GetAttributesDictionary(node)
        
        vt = pi.GetValueType(node)
        print "ValueType: " + str(vt)
        
        pre = wx.PreDialog()
        pre.SetExtraStyle(wx.DIALOG_EX_CONTEXTHELP)
        pre.Create(parent, ID, title, pos, size, style)

        self.PostCreate(pre)

        # This extra style can be set after the UI object has been created.
        if 'wxMac' in wx.PlatformInfo and useMetal:
            self.SetExtraStyle(wx.DIALOG_EX_METAL)

        mainVSizer = wx.BoxSizer(wx.VERTICAL)
     
        box = wx.BoxSizer(wx.HORIZONTAL)
      
        btnsizer = wx.StdDialogButtonSizer()
        
        if wx.Platform != "__WXMSW__":
            btn = wx.ContextHelpButton(self)
            btnsizer.AddButton(btn)

        sep1 = wx.StaticText(self, -1, "")
        mainVSizer.Add(sep1)
 
        notesText = pi.GetNotes(node)
        if notesText:
            pass
        else:
            notesText = "Sorry, Notes not available for this entry..."
            
        notes = wx.StaticText(self, -1, notesText)
        mainVSizer.Add(notes, 0, wx.ALIGN_CENTRE|wx.ALL, 5)
        
        sep4 = wx.StaticText(self, -1, "")
        mainVSizer.Add(sep4)
 
        hsizer = wx.BoxSizer(wx.HORIZONTAL)
        mainVSizer.AddSizer(hsizer)

        if vt == "date":        
            
        
            control = [
           "",        "##/##/####",  'BCDEFGHIJKLMNOQRSTUVWXYZ','DF!',"",          '','', wx.DateTime_Now().Format("%m/%d/%Y"),   
           ]
         
            self.TextControl  = masked.TextCtrl( self, -1, "",
                                                mask         = control[1],
                                                excludeChars = control[2],
                                                formatcodes  = control[3],
                                                includeChars = "",
                                                validRegex   = control[4],
                                                validRange   = control[5],
                                                choices      = control[6],
                                                choiceRequired = True,
                                                defaultValue = control[7],
                                                demo         = True,
                                                name         = control[0])
        
                
        elif vt == "list":
             
             values = pi.GetValueList(node)
             print values
                          
             self.TextControl = wx.ComboBox(self, 500, values[0], (90, 50), 
                         (160, -1), values,
                         wx.CB_DROPDOWN
                         | wx.CB_READONLY
                         #| wx.CB_SORT
                         )

        elif vt == "bool":
             
             values = pi.GetValueList(node)
                          
             self.TextControl = wx.ComboBox(self, 500, "True", (90, 50), 
                         (160, -1), ["True","False"],
                         wx.CB_DROPDOWN
                         | wx.CB_READONLY
                         #| wx.CB_SORT
                         )

        elif vt == "string":
            print "string validation..."
            
            
            self.TextControl = wx.TextCtrl(self, -1, "", validator = TextObjectValidator(vt))
        elif vt == "URI":
        
            print "URI validation..."
            
            
            self.TextControl = wx.TextCtrl(self, -1, "", validator = TextObjectValidator(vt))
           
        elif vt == "integer":
            print "int validation..."
    
            self.TextControl = wx.TextCtrl(self, -1, "", validator = TextObjectValidator(vt))
    
        elif vt == "real":
            print "real validation..."
            self.TextControl = wx.TextCtrl(self, -1, "", validator = TextObjectValidator(vt))
        
        else:
            
            label = wx.StaticText(self, -1, "Cannot Create ")
            label.SetHelpText("This is the help text for the label")
            mainVSizer.Add(label, 0, wx.ALIGN_CENTRE|wx.ALL, 5)
            print "Cannot create this control!"

       
        mainVSizer.Add(self.TextControl,0, wx.ALIGN_CENTER)
        
        sep2 = wx.StaticText(self, -1, "")
        mainVSizer.Add(sep2)
        
        sep3 = wx.StaticText(self, -1, "")
        mainVSizer.Add(sep3)
 
        btn = wx.Button(self, wx.ID_OK)
        btn.SetHelpText("The OK button completes the dialog")
        btn.SetDefault()
        btnsizer.AddButton(btn)

        btn = wx.Button(self, wx.ID_CANCEL)
        btn.SetHelpText("The Cancel button cancels the dialog. (Cool, huh?)")
        btnsizer.AddButton(btn)
        btnsizer.Realize()

        mainVSizer.Add(btnsizer, 0, wx.ALIGN_CENTRE|wx.ALL, 5)

        self.SetSizer(mainVSizer)
        mainVSizer.Fit(self)
        
        

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        
        self.ShowDialog()            

    def ShowDialog(self):
        useMetal = False
        if 'wxMac' in wx.PlatformInfo:
            useMetal = self.cb.IsChecked()
            
        dlg = Dialog(self, -1, "Edit Tag", size=(350, 200),
                         #style=wx.CAPTION | wx.SYSTEM_MENU | wx.THICK_FRAME,
                         style=wx.DEFAULT_DIALOG_STYLE, # & ~wx.CLOSE_BOX,
                         useMetal=useMetal, 
                         )
        dlg.CenterOnScreen()

        # this does not return until the dialog is closed.
        val = dlg.ShowModal()
    
        if val == wx.ID_OK:
            self.log.WriteText("You pressed OK\n")
            if self.TextControl: 
                print "assigning: " + self.TextControl.GetValue()
        else:
            self.log.WriteText("You pressed Cancel\n")
            if self.TextControl: 
                print "discarding: " + self.TextControl.GetValue()
        dlg.Destroy()
        

#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#---------------------------------------------------------------------------


overview = """\
wxPython offers quite a few general purpose dialogs for useful data input from
the user; they are all based on the wx.Dialog class, which you can also subclass
to create custom dialogs to suit your needs.

The Dialog class, in addition to dialog-like behaviors, also supports the full
wxWindows layout featureset, which means that you can incorporate sizers or
layout constraints as needed to achieve the look and feel desired. It even supports
context-sensitive help, which is illustrated in this example.

The example is very simple; in real world situations, a dialog that had input
fields such as this would no doubt be required to deliver those values back to
the calling function. The Dialog class supports data retrieval in this manner.
<b>However, the data must be retrieved prior to the dialog being destroyed.</b>
The example shown here is <i>modal</i>; non-modal dialogs are possible as well.

See the documentation for the <code>Dialog</code> class for more details.

"""

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])


class TextObjectValidator(wx.PyValidator):
     """ This validator is used to ensure that the user has entered something
         into the text object editor dialog's text field.
     """
     def __init__(self, validationType):
         """ Standard constructor.
         """
         wx.PyValidator.__init__(self)
         self.ValidationType = ""
         self.ValidationType = validationType
         print validationType


     def Clone(self):
         """ Standard cloner.

             Note that every validator must implement the Clone() method.
         """
         return TextObjectValidator(self.ValidationType)


     def Validate(self, win):
         """ Validate the contents of the given text control.
         """
         textCtrl = self.GetWindow()
         text = textCtrl.GetValue()

         if len(text) == 0:
             wx.MessageBox("A text object must contain some text!", "Error")
             textCtrl.SetBackgroundColour("yellow")
             textCtrl.SetFocus()
             textCtrl.Refresh()
             return False
         
         elif self.ValidationType == "string":
            print "validating string"
            pass
        
         elif self.ValidationType == "URI":
            print "validating string"
            pass
        
         elif self.ValidationType == "integer":
            print "validating integer"
            for c in text:
                if isdigit(c):
                    continue
                else:
                     wx.MessageBox("You must write an integer value!", "Error")
                     textCtrl.SetBackgroundColour("yellow")
                     textCtrl.SetFocus()
                     textCtrl.Refresh()
                     return False                
                return True
            pass
         elif self.ValidationType == "real":
            print "validating real"
            
            try:
                val = float(text)
            except ValueError:     
                 wx.MessageBox("You must write a real value!", "Error")
                 textCtrl.SetBackgroundColour("yellow")
                 textCtrl.SetFocus()
                 textCtrl.Refresh()
                 return False                
    
            pass
         else:
             textCtrl.SetBackgroundColour(
                 wx.SystemSettings_GetColour(wx.SYS_COLOUR_WINDOW))
             textCtrl.Refresh()
             return True


     def TransferToWindow(self):
         """ Transfer data from validator to window.

             The default implementation returns False, indicating that an error
             occurred.  We simply return True, as we don't do any data transfer.
         """
         return True # Prevent wxDialog from complaining.


     def TransferFromWindow(self):
         """ Transfer data from window to validator.

             The default implementation returns False, indicating that an error
             occurred.  We simply return True, as we don't do any data transfer.
         """
         return True # Prevent wxDialog from complaining.
