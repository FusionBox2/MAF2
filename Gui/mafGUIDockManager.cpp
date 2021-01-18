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

#include <wx/wx.h>
#include <wx/minifram.h>
#include <wx/image.h>
#include "mafGUIDockManager.h"
#include "mafGUIMDIFrame.h"

//--------------------------------------------------------------------------
bool mafGUIDockManager::AddPane(wxWindow* window,const wxAuiPaneInfo& pane_info, const wxString &menu, const wxString &subMenu)
//--------------------------------------------------------------------------
{
  AddMenuItem(window,pane_info.caption, menu, subMenu);
  return wxAuiManager::AddPane(window,pane_info);
}
//--------------------------------------------------------------------------
bool mafGUIDockManager::AddPane(wxWindow* window,int direction,const wxString& caption)
//--------------------------------------------------------------------------
{
  AddMenuItem(window,caption);
  return wxAuiManager::AddPane(window,direction,caption);
}
//--------------------------------------------------------------------------
void mafGUIDockManager::Update()
//--------------------------------------------------------------------------
{
  wxAuiManager::Update();
  UpdateMenuItems();
}
//--------------------------------------------------------------------------
void mafGUIDockManager::AddMenuItem(wxWindow* window,const wxString& caption , const wxString &menu, const wxString &subMenu)
//--------------------------------------------------------------------------
{
  if (!caption.IsEmpty())
  {
    wxMenuBar* menubar = ((wxFrame*)m_frame)->GetMenuBar();
    if ( menubar )
    {
      int idx = menubar->FindMenu(menu);
      if( idx != wxNOT_FOUND)
      {
        wxMenu *m = menubar->GetMenu(idx);
				if(subMenu != wxEmptyString)
				{
					int ids = m->FindItem(subMenu);
					wxMenuItem *sm = m->FindItem(ids);
					if( ids != wxNOT_FOUND && sm->GetSubMenu())
					{		
						sm->GetSubMenu()->Append(window->GetId(), caption, "", wxITEM_CHECK);
					}
				}
				else
				{
          m->Append(window->GetId(), caption, "", wxITEM_CHECK);
				}
        //m_MenuBar->Check(id,);
      }
    }
  }
}
//--------------------------------------------------------------------------
void mafGUIDockManager::UpdateMenuItems()
//--------------------------------------------------------------------------
{
  wxMenuBar* menubar = ((wxFrame*)m_frame)->GetMenuBar();
  if ( !menubar ) return;

  int i, pane_count = m_panes.GetCount();
  for (i = 0; i < pane_count; ++i)
  {
    wxAuiPaneInfo& p = m_panes.Item(i);
    if( menubar->FindItem(p.window->GetId() ) )
      menubar->Check(p.window->GetId(), p.IsShown());
  }
}
