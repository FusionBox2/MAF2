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


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafGUIPicButton.h"
#include "mafDecl.h"
#include "mafPics.h"

//#define BN_CLICKED 0  //SIL. 23-3-2005:  Hack to be Removed
//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIPicButton,wxBitmapButton)
     EVT_SET_FOCUS(mafGUIPicButton::OnSetFocus) 
     EVT_COMMAND_RANGE(MINID,MAXID,wxEVT_COMMAND_BUTTON_CLICKED,   mafGUIPicButton::Command)
     EVT_COMMAND_RANGE(PIC_START,PIC_END,wxEVT_COMMAND_BUTTON_CLICKED,   mafGUIPicButton::Command)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(mafGUIPicButton,wxBitmapButton)
//----------------------------------------------------------------------------
mafGUIPicButton::mafGUIPicButton(wxWindow *parent, wxString BitmapId, wxWindowID id, mafBaseEventHandler *listener, int offset)
//----------------------------------------------------------------------------
{
  SetListener(listener);
  m_Id = id;

  wxBitmap b = mafPictureFactory::GetPictureFactory()->GetBmp(BitmapId);
  wxSize size(b.GetWidth()+offset,b.GetHeight()+offset);

  //Create(parent, m_Id, b, wxDefaultPosition, size,0);
  Create(parent, m_Id, b, wxDefaultPosition, size);
  SetBitmap(BitmapId,id);
}
//----------------------------------------------------------------------------
mafGUIPicButton::mafGUIPicButton(wxWindow *parent, wxBitmap *b, wxWindowID id, mafBaseEventHandler *listener, int offset)
//----------------------------------------------------------------------------
{
  SetListener(listener);
  m_Id = id;

  wxSize size(b->GetWidth()+offset,b->GetHeight()+offset);
  //Create(parent, m_Id, *b, wxDefaultPosition, size,0);
  Create(parent, m_Id, *b, wxDefaultPosition, size);

  SetBitmapLabel(*b);
  //SetBitmapDisabled(mafGrayScale(*b));
  Refresh();
}
//----------------------------------------------------------------------------
void mafGUIPicButton::SetEventId(long EventId)
//----------------------------------------------------------------------------
{
   m_Id = EventId;
}
//----------------------------------------------------------------------------
void mafGUIPicButton::Command(wxCommandEvent& event)
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this, m_Id));
}
//----------------------------------------------------------------------------
void mafGUIPicButton::SetBitmap(wxString BitmapId, wxWindowID id )
//----------------------------------------------------------------------------
{
  wxBitmap b = mafPictureFactory::GetPictureFactory()->GetBmp(BitmapId);
  SetBitmapLabel(b);
  //SetBitmapDisabled(mafGrayScale(b));
  m_Id = id;
  Refresh();
}
