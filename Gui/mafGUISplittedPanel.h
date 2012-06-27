/*=========================================================================

 Program: MAF2
 Module: mafGUISplittedPanel
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUISplittedPanel_H__
#define __mafGUISplittedPanel_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/laywin.h>
#include "mafDefines.h"
#include "mafGUIPanel.h"

//----------------------------------------------------------------------------
// mafGUISplittedPanel :
/**
mafGUISplittedPanel is a wxPanel with a user-draggable splitter
that divide it in two sections vertically. Call SetTop and SetBottom
to place things on it.
*/
//----------------------------------------------------------------------------
class MAF_EXPORT mafGUISplittedPanel: public mafGUIPanel
{
public:
  mafGUISplittedPanel(wxWindow* parent,wxWindowID id = -1,int size = 100);
  virtual ~mafGUISplittedPanel();

  /** Put on top the window. */
	void PutOnTop(wxWindow *w);

  /** Put on bottom the window. */
  void PutOnBottom(wxWindow *w);

protected:
  wxSashLayoutWindow   *m_Bottom;
  wxWindow             *m_TopPanel;
  wxWindow             *m_BottomPanel;

  /** Call Dolayout. */
  void OnSize(wxSizeEvent& event);

  /** Readraw the Sash panel. */
  void OnSashDrag(wxSashEvent& event);

  /** Readistribute the interface according to the new size. */
  void DoLayout();
	
DECLARE_EVENT_TABLE()
};
#endif
