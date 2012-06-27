/*=========================================================================

 Program: MAF2
 Module: testTabLogic
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


#include "testTabLogic.h" 
#include "mafGUIMDIFrame.h"
#include "mafGUINamedPanel.h"
#include "mafGUISashPanel.h"
#include "mafWXLog.h"
#include "testTabGui.h" 

//----------------------------------------------------------------------------
testTabLogic::testTabLogic()
//----------------------------------------------------------------------------
{
  m_gui = NULL;
  m_win = new mafGUIMDIFrame("testTab", wxDefaultPosition, wxSize(800, 600));
  m_win->SetListener(this);

  CreateMenu();
  CreateSideBar();
  CreateLogBar();

}
//----------------------------------------------------------------------------
void testTabLogic::Show()
//----------------------------------------------------------------------------
{
	m_win->Show(TRUE);
}
//----------------------------------------------------------------------------
void testTabLogic::OnQuit()
//----------------------------------------------------------------------------
{
  cppDEL(m_gui);
  m_win->Destroy();
}
//----------------------------------------------------------------------------
wxWindow* testTabLogic::GetTopWin()
//----------------------------------------------------------------------------
{
  return m_win;
}
//----------------------------------------------------------------------------
void testTabLogic::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event)) 
  {
    switch(e->GetId())
    {
    case MENU_FILE_QUIT:
      OnQuit();		
      break; 
    case MENU_VIEW_LOGBAR:
      if(m_log_bar) m_log_bar->Show(!m_log_bar->IsShown());
      break; 
    case MENU_VIEW_SIDEBAR:
      if(m_side_bar) m_side_bar->Show(!m_side_bar->IsShown());
      break; 
    case UPDATE_UI:
      break; 
    default:
      e->Log();
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void testTabLogic::CreateMenu()
//----------------------------------------------------------------------------
{
  m_menu_bar  = new wxMenuBar;
  wxMenu    *file_menu = new wxMenu;
  file_menu->Append(MENU_FILE_QUIT,  "&Quit");
  m_menu_bar->Append(file_menu, "&File");
  wxMenu    *view_menu = new wxMenu;
  m_menu_bar->Append(view_menu, "&View");
  m_win->SetMenuBar(m_menu_bar);
}
//----------------------------------------------------------------------------
void testTabLogic::CreateLogBar()
//----------------------------------------------------------------------------
{
  /*
  wxTextCtrl *log  = new wxTextCtrl( m_win, -1, "", wxPoint(0,0), wxSize(100,300), wxNO_BORDER | wxTE_MULTILINE );
  mafWXLog *m_logger = new mafWXLog(log);
  wxLog *old_log = wxLog::SetActiveTarget( m_logger );
  cppDEL(old_log);

  mafGUINamedPanel *log_panel = new mafGUINamedPanel(m_win,-1,true);
  log_panel->SetTitle(" Log Area:");
  log_panel->Add(log,1,wxEXPAND);

  m_log_bar = new mafGUISashPanel(m_win, MENU_VIEW_LOGBAR, wxBOTTOM,80,"Log Bar \tCtrl+L");
  m_log_bar->Put(log_panel);
  //m_log_bar->Show(false);
  wxLogMessage("buongiorno");
  */

  m_log_bar = new mafGUISashPanel(m_win, MENU_VIEW_LOGBAR, wxBOTTOM,80,"Log Bar \tCtrl+L");

  wxNotebook *notebook = new wxNotebook(m_log_bar,-1);
  m_log_bar->Put(notebook);

  wxTextCtrl *log  = new wxTextCtrl( notebook, -1, "", wxPoint(0,0), wxSize(100,300), wxNO_BORDER | wxTE_MULTILINE );
  notebook->AddPage(log,"log",true);

  mafWXLog *m_logger = new mafWXLog(log);
  wxLog *old_log = wxLog::SetActiveTarget( m_logger );
  cppDEL(old_log);

  mafGUINamedPanel *p1 = new mafGUINamedPanel(notebook,-1,true);
  p1->SetTitle(" p1:");
  notebook->AddPage(p1,"console",true);

  mafGUINamedPanel *p2 = new mafGUINamedPanel(notebook,-1,true);
  p2->SetTitle(" p2:");
  notebook->AddPage(p2,"time bar",true);

  wxLogMessage("1buongiorno");
  wxLogMessage("2buongiorno");
  wxLogMessage("3buongiorno");
  wxLogMessage("4buongiorno");
  wxLogMessage("5buongiorno");
  wxLogMessage("6buongiorno");
  wxLogMessage("7buongiorno");
  wxLogMessage("8buongiorno");
  wxLogMessage("9buongiorno");
  wxLogMessage("10buongiorno");
  wxLogMessage("11buongiorno");
}
//----------------------------------------------------------------------------
void testTabLogic::CreateSideBar()
//----------------------------------------------------------------------------
{
  m_side_bar = new mafGUISashPanel(m_win, MENU_VIEW_SIDEBAR, wxRIGHT,330,"Side Bar \tCtrl+S");
  m_notebook = new wxNotebook(m_side_bar,-1);
  m_side_bar->Put(m_notebook);

  mafGUINamedPanel *p1 = new mafGUINamedPanel(m_notebook,-1,true);
  p1->SetTitle(" p1:");
  m_notebook->AddPage(p1,"page 1",true);

  mafGUINamedPanel *p2 = new mafGUINamedPanel(m_notebook,-1,true);
  p2->SetTitle(" p2:");
  m_notebook->AddPage(p2,"page 2",true);

  //mafGUINamedPanel *p3 = new mafGUINamedPanel(m_notebook,-1,true);
  //p3->SetTitle(" p3:");
  m_gui = new testTabGui(m_notebook);
  //p3->Add(m_gui->GetGui());

  //m_notebook->AddPage(p3,"page 3",true);
  m_notebook->AddPage(m_gui->GetGui(),"gui",true);
  
  //p3->SetTitleColor();


}




