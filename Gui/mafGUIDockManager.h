/*=========================================================================

 Program: MAF2
 Module: mafGUIDockManager
 Authors: Benjamin I. Williams
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafGUIDockManager__H
#define __mafGUIDockManager__H

#include "mafDefines.h"
#include <wx/aui/aui.h>

/**
  class name: mafGUIDockManager
  Represent a frame manager for gui dock elements.
*/
class MAF_EXPORT mafGUIDockManager : public wxAuiManager 
{
public:
  /** add pane */
  virtual bool AddPane(wxWindow* window,const wxAuiPaneInfo& pane_info, const wxString &menu = _("&View"), const wxString &subMenu = wxEmptyString);
  /** add pane overload*/
  virtual bool AddPane(wxWindow* window,int direction = wxLEFT,const wxString& caption = wxEmptyString);	
  /** update */
  virtual void Update();
protected:
  /** add menu item */
	void AddMenuItem(wxWindow* window,const wxString& caption = wxEmptyString, const wxString &menu = _("&View"), const wxString &subMenu = wxEmptyString );
  /** update menu items */
  void UpdateMenuItems();
};  


#endif  //__mafGUIDockManager__
