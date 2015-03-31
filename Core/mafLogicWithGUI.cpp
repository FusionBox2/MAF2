/*=========================================================================

 Program: MAF2
 Module: mafLogicWithGUI
 Authors: Silvano Imboden, Paolo Quadrani
 
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


#include "mafLogicWithGUI.h"
#include <wx/utils.h>
#include <wx/datetime.h>
#include <wx/busyinfo.h>
#include <wx/splash.h>
#include <wx/tokenzr.h>
#include "mafDecl.h"
#include "mafView.h"
#include "mafGUIMDIFrame.h"
#include "mafGUIMDIChild.h"
#include "mafGUIFrame.h"
#include "mafGUIPicButton.h"
#include "mafGUISplittedPanel.h"
#include "mafGUINamedPanel.h"
#include "mafGUICrossSplitter.h"
#include "mafGUITimeBar.h"
#include "mafGUILocaleSettings.h"
#include "mafGUIMeasureUnitSettings.h"
#include "mafGUIApplicationSettings.h"
#include "mafGUISettingsStorage.h"
#include "mafGUISettingsTimeBar.h"

#include "mafWXLog.h"
#include "mafPics.h"
#ifdef MAF_USE_VTK
  #include "mafVTKLog.h"
  #include "vtkTimerLog.h"
#endif
//----------------------------------------------------------------------------
mafLogicWithGUI::mafLogicWithGUI(mafGUIMDIFrame *mdiFrame /*=NULL*/)
//----------------------------------------------------------------------------
{
  if (mdiFrame==NULL)
    m_Win = new mafGUIMDIFrame("maf", wxDefaultPosition, wxSize(800, 600));
  else
    m_Win = mdiFrame;

  m_Win->SetListener(this);

  m_ChildFrameStyle = wxCAPTION | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxRESIZE_BORDER; //wxTHICK_FRAME; // Default style
  m_LocaleSettings = new mafGUILocaleSettings(this);
  m_MeasureUnitSettings = new mafGUIMeasureUnitSettings(this);
  m_ApplicationSettings = new mafGUIApplicationSettings(this);
  m_StorageSettings = new mafGUISettingsStorage(this);
  m_TimeBarSettings = new mafGUISettingsTimeBar(this);

  m_ToolBar       = NULL;
  m_MenuBar       = NULL;

	m_LogToFile			= m_ApplicationSettings->GetLogToFileStatus();
	m_LogAllEvents	= m_ApplicationSettings->GetLogVerboseStatus();
	m_Logger				= NULL;
  m_VtkLog        = NULL;

	m_AppTitle      = "";

	m_PlugMenu		  = true;
	m_PlugToolbar	  = true;
	m_PlugSidebar	  = true;
  m_SidebarStyle  = mafSideBar::DOUBLE_NOTEBOOK;
	m_PlugTimebar	  = true;
	m_PlugLogbar	  = true;
}
//----------------------------------------------------------------------------
mafLogicWithGUI::~mafLogicWithGUI()
//----------------------------------------------------------------------------
{
  cppDEL(m_LocaleSettings);
  cppDEL(m_MeasureUnitSettings);
  cppDEL(m_ApplicationSettings);
  cppDEL(m_StorageSettings);
  cppDEL(m_TimeBarSettings);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::SetParentFrameStyle(long style)
//----------------------------------------------------------------------------
{
  m_Win->SetWindowStyleFlag(style);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::Configure()
//----------------------------------------------------------------------------
{
  if(m_PlugMenu)    this->AddMenu();
  if(m_PlugToolbar) this->AddToolbar();
  if(m_PlugTimebar) this->AddTimebar();
  if(m_PlugLogbar)  this->AddLogbar(); else this->CreateNullLog();
  EnableItem(CAMERA_RESET, false);
  EnableItem(CAMERA_FIT,   false);
  EnableItem(CAMERA_FLYTO, false);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::Show()
//----------------------------------------------------------------------------
{
  wxAcceleratorEntry *entries = new wxAcceleratorEntry[m_AccelTable.size()];
  for(int i = 0; i < m_AccelTable.size(); i++)
    entries[i] = m_AccelTable[i];
  wxAcceleratorTable atable(m_AccelTable.size(), entries);
  if (atable.Ok())
    mafGetFrame()->SetAcceleratorTable(atable);
  delete[] entries;
  m_AppTitle = m_Win->GetTitle().c_str();
	m_Win->Show(TRUE);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::ShowSplashScreen()
//----------------------------------------------------------------------------
{
  wxBitmap splashImage = mafPictureFactory::GetPictureFactory()->GetBmp("SPLASH_SCREEN");
  ShowSplashScreen(splashImage);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::ShowSplashScreen(wxBitmap &splashImage)
//----------------------------------------------------------------------------
{
  long splash_style = wxSIMPLE_BORDER | wxSTAY_ON_TOP;
  wxSplashScreen* splash = new wxSplashScreen(splashImage,
    wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
    2000, NULL, -1, wxDefaultPosition, wxDefaultSize,
    splash_style);
  mafYield();
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::AddMenu()
//----------------------------------------------------------------------------
{
  CreateMenu();
  m_Win->SetMenuBar(m_MenuBar);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateMenu()
//----------------------------------------------------------------------------
{
  m_MenuBar  = new wxMenuBar;
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(MENU_FILE_QUIT,  _("&Quit"));
  m_MenuBar->Append(file_menu, _("&File"));
  wxMenu *view_menu = new wxMenu;
  m_MenuBar->Append(view_menu, _("&View"));
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case MENU_FILE_QUIT:
      OnQuit();		
      break; 
    //resize view
	case TILE_WINDOW_CASCADE:
		m_Win->Cascade();
		break;
	case TILE_WINDOW_HORIZONTALLY:
      m_Win->Tile(wxHORIZONTAL);
	  break;
	case TILE_WINDOW_VERTICALLY:
	  m_Win->Tile(wxVERTICAL);
	  break;
      // ###############################################################
      // commands related to the Dockable Panes
    case MENU_VIEW_LOGBAR:
        m_Win->ShowDockPane("logbar",!m_Win->DockPaneIsShown("logbar") );
    break; 
    case MENU_VIEW_SIDEBAR:
      m_Win->ShowDockPane("sidebar",!m_Win->DockPaneIsShown("sidebar") );
    break; 
    case MENU_VIEW_TIMEBAR:
      m_Win->ShowDockPane("timebar",!m_Win->DockPaneIsShown("timebar") );
    break; 
    case MENU_VIEW_TOOLBAR:
      m_Win->ShowDockPane("toolbar",!m_Win->DockPaneIsShown("toolbar") );
    break; 
      // ###############################################################
      // commands related to the STATUSBAR
    case BIND_TO_PROGRESSBAR:
#ifdef MAF_USE_VTK
      m_Win->BindToProgressBar(e->GetVtkObj());
#endif
      break;
    case PROGRESSBAR_SHOW:
      m_Win->ProgressBarShow();
      break;
    case PROGRESSBAR_HIDE:
      m_Win->ProgressBarHide();
      break;
    case PROGRESSBAR_SET_VALUE:
      m_Win->ProgressBarSetVal(e->GetArg());
      break;
    case PROGRESSBAR_SET_TEXT:
      m_Win->ProgressBarSetText(&wxString(e->GetString()->GetCStr()));
      break;
      // ###############################################################
    case UPDATE_UI:
      break; 
    default:
      e->Log();
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::OnQuit()
//----------------------------------------------------------------------------
{
  // if OnQuit is redefined in a derived class,  mafLogicWithGUI::OnQuit() must be called last

  mafYield();
  if(m_PlugLogbar)
  {
    delete wxLog::SetActiveTarget(NULL);
  }
  m_Win->OnQuit();
  m_Win->Destroy();
#ifdef MAF_USE_VTK 
  vtkTimerLog::CleanupLog();
  vtkDEL(m_VtkLog);
#endif
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::AddLogbar()
//----------------------------------------------------------------------------
{
  CreateLogbar();
}

//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateLogbar()
//----------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  m_VtkLog = mafVTKLog::New();
  m_VtkLog->SetInstance(m_VtkLog);
#endif
  wxTextCtrl *log  = new wxTextCtrl( m_Win, MENU_VIEW_LOGBAR, "", wxPoint(0,0), wxSize(100,300), /*wxNO_BORDER |*/ wxTE_MULTILINE );
  m_Logger = new mafWXLog(log);
  m_Logger->LogToFile(m_LogToFile);
  if(m_LogToFile)
  {
    wxString s = m_ApplicationSettings->GetLogFolder().GetCStr();
    wxDateTime log_time = wxDateTime::Now();
    s += "\\";
    s += m_Win->GetTitle();
    s += wxString::Format("_%02d_%02d_%d_%02d_%2d",log_time.GetYear(),log_time.GetMonth() + 1,log_time.GetDay(),log_time.GetHour(),log_time.GetMinute());
    s += ".log";
    if (m_Logger->SetFileName(s) == MAF_ERROR)
    {
      mafLogMessage(wxString::Format("Unable to create log file %s!!",s),"Warning", wxOK|wxICON_WARNING);
    }
  }
  m_Logger->SetVerbose(m_LogAllEvents);

  wxLog *old_log = wxLog::SetActiveTarget( m_Logger );
  cppDEL(old_log);

  m_Win->AddDockPane(log, wxPaneInfo()
    .Name("logbar")
    .Caption(wxT("LogBar"))
    .Bottom()
    .Layer(0)
    .MinSize(100,10)
    .TopDockable(false) // prevent docking on top side - otherwise may dock also beside the toolbar -- and it's hugely
  );
  
  mafLogMessage(_("welcome"));
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateNullLog()
//----------------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  m_VtkLog = mafVTKLog::New();
  m_VtkLog->SetInstance(m_VtkLog);
#endif  
  wxTextCtrl *log  = new wxTextCtrl( m_Win, -1, "", wxPoint(0,0), wxSize(100,300), wxNO_BORDER | wxTE_MULTILINE );
  m_Logger = new mafWXLog(log);
  log->Show(false);
  wxLog *old_log = wxLog::SetActiveTarget( m_Logger );
  cppDEL(old_log);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::AddToolbar()
//----------------------------------------------------------------------------
{
  CreateToolbar();
  //m_Win->SetToolBar(m_ToolBar);
  m_Win->AddDockPane(m_ToolBar,  wxPaneInfo()
    .Name("toolbar")
    .Caption(wxT("ToolBar"))
    .Top()
    .Layer(2)
    .ToolbarPane()
    .LeftDockable(false)
    .RightDockable(false)
    .Floatable(false)
    .Movable(false)
    .Gripper(false)
    );
}

//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateToolbar()
//----------------------------------------------------------------------------
{
  //m_ToolBar = new wxToolBar(m_Win,-1,wxPoint(0,0),wxSize(-1,-1),wxHORIZONTAL|wxNO_BORDER|wxTB_FLAT  );
  m_ToolBar = new wxToolBar(m_Win,MENU_VIEW_TOOLBAR,wxPoint(0,0),wxSize(-1,-1),wxTB_FLAT | wxTB_NODIVIDER );
  m_ToolBar->SetMargins(0,0);
  m_ToolBar->SetToolSeparation(2);
  m_ToolBar->SetToolBitmapSize(wxSize(20,20));
  m_ToolBar->AddTool(MENU_FILE_NEW,mafPictureFactory::GetPictureFactory()->GetBmp("FILE_NEW"),    _("new msf storage file"));
  m_ToolBar->AddTool(MENU_FILE_OPEN,mafPictureFactory::GetPictureFactory()->GetBmp("FILE_OPEN"),  _("open msf storage file"));
  m_ToolBar->AddTool(MENU_FILE_SAVE,mafPictureFactory::GetPictureFactory()->GetBmp("FILE_SAVE"),  _("save current msf storage file"));
  m_ToolBar->Realize();
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::AddTimebar()
//----------------------------------------------------------------------------
{
  CreateTimebar();
  m_Win->AddDockPane(m_TimePanel, wxPaneInfo()
    .Name("timebar")
    .Caption(wxT("TimeBar"))
    .Bottom()
    .Row(1)
    .Layer(2)
    .ToolbarPane()
    .LeftDockable(false)
    .RightDockable(false)
    .MinSize(100,22)
    .Floatable(false)
    .Gripper(false)
    .Resizable(false)
    .Movable(false)
    );
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::CreateTimebar()
//----------------------------------------------------------------------------
{
  m_TimePanel = new mafGUITimeBar(m_Win,MENU_VIEW_TIMEBAR,true);
  m_TimePanel->SetListener(this);

  // Events coming from settings are forwarded to the time bar.
  m_TimePanel->SetTimeSettings(m_TimeBarSettings);
  m_TimeBarSettings->SetListener(m_TimePanel);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::EnableItem(int item, bool enable)
//----------------------------------------------------------------------------
{
  if(m_MenuBar) 
     // must always check if a menu item exist because
     // during application shutdown it is not guaranteed
     if(m_MenuBar->FindItem(item))	
        m_MenuBar->Enable(item,enable );
  if(m_ToolBar)
     m_ToolBar->EnableTool(item,enable );
}

//----------------------------------------------------------------------------
void mafLogicWithGUI::AddToMenu(const mafString& name, long id, wxMenu* path_menu, const mafString& menuPath)
//----------------------------------------------------------------------------
{
  if (menuPath != "")
  {
    wxString op_path = "";
    wxStringTokenizer path_tkz(menuPath.GetCStr(), "/");
    while ( path_tkz.HasMoreTokens() )
    {
      op_path = path_tkz.GetNextToken();
      int item = path_menu->FindItem(_(op_path));
      if (item != wxNOT_FOUND)
      {
        wxMenuItem *menu_item = path_menu->FindItem(item);
        if (menu_item)
          path_menu = menu_item->GetSubMenu();
      }
      else
      {
        wxMenu *sub_menu = new wxMenu;
        path_menu->Append(-1,_(op_path),sub_menu);
        path_menu = sub_menu;
      }
    }
  }
  path_menu->Append(id, _(name), _(name));
  SetAccelerator(name, id);
}
//----------------------------------------------------------------------------
void mafLogicWithGUI::SetAccelerator(const mafString& name, long id)
//----------------------------------------------------------------------------
{
  wxString accelerator, flag = "", extra_flag = "", key_code = "";
  int flag_num;
  accelerator = name.GetCStr();
  wxStringTokenizer tkz(accelerator, "\t");
  int token = tkz.CountTokens();

  if (token > 1)
  {
    accelerator  = tkz.GetNextToken();
    accelerator  = tkz.GetNextToken();
    wxStringTokenizer tkz2(accelerator, "+");
    token = tkz2.CountTokens();
    if (token == 2)
    {
      flag = tkz2.GetNextToken();
      key_code = tkz2.GetNextToken();
    }
    else
    {
      flag = tkz2.GetNextToken();
      extra_flag = tkz2.GetNextToken();
      key_code = tkz2.GetNextToken();
    }
    if (flag == "Ctrl")
      flag_num = wxACCEL_CTRL;
    else if(flag == "Alt")
      flag_num = wxACCEL_ALT;
    else if(flag == "Shift")
      flag_num = wxACCEL_SHIFT;

    if (extra_flag == "Ctrl")
      flag_num |= wxACCEL_CTRL;
    else if(extra_flag == "Alt")
      flag_num |= wxACCEL_ALT;
    else if(extra_flag == "Shift")
      flag_num |= wxACCEL_SHIFT;

    m_AccelTable.push_back(wxAcceleratorEntry(flag_num,  (int) *key_code.c_str(), id));
  }
}
