/*=========================================================================

 Program: MAF2
 Module: mafGUIViewFrame
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



#include "mafGUIViewFrame.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafView.h"
//#include "mafSceneGraph.h"
//#include "mafSceneNode.h"
//#include "vtkVersion.h"

//----------------------------------------------------------------------------
// mafGUIViewFrame
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(mafGUIViewFrame, wxFrame)
    EVT_CLOSE(mafGUIViewFrame::OnCloseWindow)
    EVT_SIZE(mafGUIViewFrame::OnSize)
    EVT_BUTTON(VIEW_CLICKED, mafGUIViewFrame::OnSelect)
	  EVT_ACTIVATE  (mafGUIViewFrame::OnActivate)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
mafGUIViewFrame::mafGUIViewFrame( wxFrame* parent, 
                            wxWindowID id, 
                            const mafString& title,
                            const wxPoint& pos, 
                            const wxSize& size, 
                            long style 
                            )
//----------------------------------------------------------------------------
: wxFrame(parent, id, title.toWx(), pos, size, style)
{
  m_Win = NULL;
}
//----------------------------------------------------------------------------
mafGUIViewFrame::~mafGUIViewFrame( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIViewFrame::OnCloseWindow(wxCloseEvent &event)
//----------------------------------------------------------------------------
{ 
	{mafEvent evUnq(this,VIEW_DELETE,m_View); mafEventMacro(evUnq);}
	Destroy();
}
//----------------------------------------------------------------------------
void mafGUIViewFrame::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{ 
	Refresh();
  if (m_Win)
  {
    wxLayoutAlgorithm layout;
    layout.LayoutWindow(this,m_Win);
  }

#ifndef WIN32
  int w,h;
  //don't initialize w & h using the event - use GetClientSize instead
  this->GetClientSize(&w,&h); 
  
  if(m_View)
  {
    m_View->SetWindowSize(w,h);
  }
#endif

}
//----------------------------------------------------------------------------
void mafGUIViewFrame::OnSelect(wxCommandEvent &event) 
//----------------------------------------------------------------------------
{
  wxWindow *rwi = (wxWindow*)event.GetEventObject();
  {mafEvent evUnq(this,VIEW_SELECT,m_View,rwi); mafEventMacro(evUnq);}
}
//----------------------------------------------------------------------------
void mafGUIViewFrame::OnActivate(wxActivateEvent& event)
//----------------------------------------------------------------------------
{ 
	if( event.GetActive())
  {
		{mafEvent evUnq(this,VIEW_SELECT,m_View,(wxWindow*)NULL); mafEventMacro(evUnq);}
	  Layout();
	}
}
//----------------------------------------------------------------------------
void mafGUIViewFrame::SetView(mafView *view)
//----------------------------------------------------------------------------
{
   m_View = view;
   m_Win = m_View->GetWindow();
   m_Win->Reparent(this);
   m_Win->Show(true);

   SetTitle(wxStripMenuCodes(m_View->GetLabel().toWx()));
}
//----------------------------------------------------------------------------
void mafGUIViewFrame::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case VIEW_QUIT:
      {mafEvent evUnq(this,VIEW_DELETE,m_View); mafEventMacro(evUnq);}
      Destroy();
      break;
    default:
      // forward to ViewManager
      mafEventMacro(*e);
      break;
    }
  }
}
