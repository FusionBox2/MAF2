/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGUIMDIChild.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 06:53:38 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafGUIMDIChild_H__
#define __mafGUIMDIChild_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include <wx/mdi.h>

//#define MAX_CUSTOM_ID 20  //todo: SIL. 7-4-2005: ---- what is this ?

#include "mafEvent.h"
//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class mafView;

//----------------------------------------------------------------------------
// mafGUIMDIChild :
//----------------------------------------------------------------------------
class mafGUIMDIChild: public wxMDIChildFrame
{
public:
  mafGUIMDIChild(wxMDIParentFrame* parent, mafView *view); 
  virtual ~mafGUIMDIChild();
  void SetListener(mafObserver *Listener) {m_Listener = Listener;};
  
	/** Set the quitting flag. */
	static void OnQuit () {m_Quitting=true;}; 

  /** 
  Allow to set the flag to avoid closing the view. 
  For instance when an operation is running or something else the vertical application don't want that the view must stay open. */
  void SetAllowCloseWindow(bool allow_close) {m_AllowCloseFlag = allow_close;};

  /** Enable/disable resizing of the view.*/
  void EnableResize(bool enable = true);

protected:
	/** Adjust the child size. */
  void OnSize(wxSizeEvent &event); 

	/** Send the event to select the owned view. */
  void OnSelect(wxCommandEvent &event); 

	/** Send the event to destroy the owned view. */
  void OnCloseWindow  (wxCloseEvent& event);

	/** Send the event to select the owned view. */
  void OnActivate (wxActivateEvent& event);

  /** Used to maximize view in remote application.*/
  void OnMaximize(wxMaximizeEvent &event);

  wxWindow		*m_Win;
  mafView     *m_View;
  mafObserver *m_Listener;
  static bool  m_Quitting;
  bool         m_AllowCloseFlag;
  bool         m_EnableResize;

  DECLARE_EVENT_TABLE()
};
#endif