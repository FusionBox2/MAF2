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
#ifndef __mafGUIViewFrame_H__
#define __mafGUIViewFrame_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafEventSender.h"
#include "mafBaseEventHandler.h"
#include <wx/laywin.h>
//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mafView;

//----------------------------------------------------------------------------
// mafGUIViewFrame :
//----------------------------------------------------------------------------
class mafGUIViewFrame: public wxFrame , public mafBaseEventHandler, public mafEventSender
{
	public:
	mafGUIViewFrame(wxFrame* parent, 
							wxWindowID id, 
							const mafString& title,
							const wxPoint& pos = wxDefaultPosition, 
							const wxSize& size = wxDefaultSize, 
							long style = wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN 
							);
       
	~mafGUIViewFrame(); 
	
	/** Set which is the external view. */
	void SetView(mafView *view);

  /** Answer to the messages coming from bottom classes. */
	virtual void OnEvent(mafEventBase *maf_event);

protected:
	/** Send the event to destroy the owned view. */
	void OnCloseWindow  (wxCloseEvent &event);

	/** Adjust the child size. */
	void OnSize         (wxSizeEvent &event);

	/** Send the event to select the owned view. */
	void OnSelect				(wxCommandEvent &event); 

	/** Send the event to select the owned view. */
  void OnActivate     (wxActivateEvent &event);

	wxWindow    *m_ClientWin;
	mafView     *m_View;

	DECLARE_EVENT_TABLE()
};
#endif
