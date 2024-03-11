/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: psLoaderGUIContextualMenu.h,v $
  Language:  C++
  Date:      $Date: 2008-12-23 11:17:30 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Daniele Giunchi    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medGUIContextualMenu_H__
#define __medGUIContextualMenu_H__

#include "mafEventSender.h"
//----------------------------------------------------------------------------
// forward references;
//----------------------------------------------------------------------------
class mafView;
class mafEvent;
class mafBaseEventHandler;

//----------------------------------------------------------------------------
// medGUIContextualMenu :
//----------------------------------------------------------------------------
class psLoaderGUIContextualMenu : public wxMenu, public mafEventSender
{
public:
  psLoaderGUIContextualMenu();
  virtual ~psLoaderGUIContextualMenu();

	/** 
  Visualize contextual menù for the MDI child and selected view. */
  void ShowContextualMenu(wxFrame *child, mafView *view, bool vme_menu);		

protected:
  wxFrame     *m_ChildViewActive;
  mafView     *m_ViewActive;

	/** 
  Answer contextual menù's selection. */
	void OnContextualViewMenu(wxCommandEvent& event);
  DECLARE_EVENT_TABLE()
};
#endif
