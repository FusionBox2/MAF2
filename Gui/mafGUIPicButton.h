/*=========================================================================

 Program: MAF2
 Module: mafGUIPicButton
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUIPicButton_H__
#define __mafGUIPicButton_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafEventSender.h"
#include "mafBaseEventHandler.h"

//----------------------------------------------------------------------------
// mafGUIPicButton :
/**
mafGUIPicButton inherit from wxBitmapButton.
It override SetDefault and SetFocus to prevent the button to
display the frame typical of the selected buttons.


To create a mafGUIPicButton you must specify
- parent
- Bitmap_ID : which should be a valid pic-index in the pic-factory (see mafGUIPics)
- ID: the ID of the Event that will be sent to the mafListener --- must be in the range MINID-MAXID or PIC_START-PIC-STOP (see mafDecl.h)
- obviously you must also specify a Listener */
//----------------------------------------------------------------------------
class MAF_EXPORT mafGUIPicButton : public wxBitmapButton, public mafEventSender
{
DECLARE_DYNAMIC_CLASS(mafGUIPicButton)

public:
  mafGUIPicButton() {};
  mafGUIPicButton(wxWindow *parent, wxString BitmapId, wxWindowID id = 0, mafBaseEventHandler *listener = NULL, int offset = 0);
  mafGUIPicButton(wxWindow *parent, wxBitmap *b, wxWindowID id = 0, mafBaseEventHandler *listener = NULL, int offset = 0);

  /** allow to change the Event-Id at run time */
  void SetEventId(long EventId); 

  /**  Used to change Bitmap and Event-ID at run time.
  Example: the Time Bar Play button once pressed become a Stop Button 
  which send a Stop-Event-Id, when pressed again is reverted to Play. */
  void SetBitmap(wxString BitmapId, wxWindowID id = 0); 

protected:  
  virtual void SetDefault() {};

  /** Overriden function member to intercept event generation and forward it to the Listener. */
  virtual void Command(wxCommandEvent& event);

  void OnSetFocus(wxFocusEvent& event) {}; 

  int m_Id;

	DECLARE_EVENT_TABLE()
};
#endif // __mafGUIPicButton_H__
