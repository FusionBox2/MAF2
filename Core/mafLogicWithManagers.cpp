/*=========================================================================

 Program: MAF2
 Module: mafLogicWithManagers
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
#include "mafDecl.h"

#include "mafLogicWithManagers.h"
#include <wx/config.h>

#include "mafView.h"
#include "mafViewCompound.h"

#include "mafViewManager.h"
#include "mafOp.h"
#include "mafOpManager.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafPrintSupport.h"

#ifdef MAF_USE_VTK
  #include "mafViewVTK.h"
  
  #include "mafOpImporterVTK.h"
  #include "mafOpImporterSTL.h"
  #include "mafInteractionManager.h"
  #include "mafInteractionFactory.h"
  #include "mafInteractor.h"
  #include "mafDeviceManager.h"
  #include "mafAction.h"
  #include "mafDeviceButtonsPadMouse.h"
  #include "mafDeviceClientMAF.h"
  #include "mafInteractorPER.h"
  #include "mafGUITreeContextualMenu.h"
  #include "mafGUIContextualMenu.h"
  #include "vtkCamera.h"
#endif
#include <wx/busyinfo.h>
#include "mafVME.h"

#include "mafSideBar.h"

#include "mafUser.h"
//#include "mafGUISRBBrowse.h"
#include "mafGUIDialogRemoteFile.h"
#include "mafGUIDialogFindVme.h"
#include "mafGUIMDIFrame.h"
#include "mafGUIMDIChild.h"
#include "mafGUICheckTree.h"
#include "mafGUITimeBar.h"
#include "mafGUIMaterialChooser.h"
#include "mafGUIViewFrame.h"
#include "mafGUILocaleSettings.h"
#include "mafGUIMeasureUnitSettings.h"
#include "mafGUIApplicationSettings.h"
#include "mafGUISettingsStorage.h"
#include "mafGUIApplicationLayoutSettings.h"
#include "mafGUISettingsTimeBar.h"
#include "mafRemoteLogic.h"
#include "mafGUISettingsDialog.h"
#include  "mafGUISettingsHelp.h"

#ifdef WIN32
  #include "mafDeviceClientMAF.h"
#endif

#include "mmdRemoteFileManager.h"

#include "mmaApplicationLayout.h"

#include "mafEventSender.h"
#include "mafDataVector.h"
#include "mafVMEStorage.h"
#include "mafRemoteStorage.h"
#include "mafOpSelect.h"

#include "mafEventIO.h"
#include "mafNodeIterator.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMERoot.h"

//----------------------------------------------------------------------------
bool mafLogicWithManagers::AskConfirmAndSave()
//----------------------------------------------------------------------------
{
  if (m_NodeManager&& m_NodeManager->MSFIsModified()) // check if the msf has been modified
  {
    int answer = wxMessageBox(_("your work is modified, would you like to save it?"),_("Confirm"),wxYES_NO|wxCANCEL|wxICON_QUESTION,mafGetFrame()); // ask user if will save msf before closing
    if(answer == wxCANCEL)
      return false;
    else if(answer == wxYES)
      return OnFileSave();
  }
  return true;
}

mafID mafLogicWithManagers::GetNewMenuId()
{
  return MENU_USER_START + m_UserCommandIndex++;
}

void mafLogicWithManagers::EnableOperations(bool enable)
{
  for(unsigned i = 0; i < m_MenuElems.size(); i++)
  {
    if(i == 0)
    {
      EnableItem(i + MENU_USER_START,enable && m_OpManager->UndoAvailable());
      continue;
    }
    if(i == 1)
    {
      EnableItem(i + MENU_USER_START,enable && m_OpManager->RedoAvailable()); 
      continue;
    }
    if(m_MenuElems[i].m_op)
    {
      bool enableOp = enable;
      mafNode *node = m_OpManager->GetSelectedVme();
      enableOp = enableOp && node && m_OpManager->GetOperationById(m_MenuElems[i].m_id)->Accept(node);
      EnableItem(i + MENU_USER_START, enableOp); 
    }
  }
}

//----------------------------------------------------------------------------
mafLogicWithManagers::mafLogicWithManagers(mafGUIMDIFrame *mdiFrame/*=NULL*/)
: mafLogicWithGUI(mdiFrame)
//----------------------------------------------------------------------------
{
  m_SideBar     = NULL;
  m_UseVMEManager  = true;
  m_UseViewManager = true;
  m_UseOpManager   = true;
  m_UseInteractionManager = true;
  
  m_ExternalViewFlag  = false;

  m_CameraLinkingObserverFlag = false;
  
  m_NodeManager  = NULL;
  m_ViewManager = NULL;
  m_OpManager   = NULL;
  m_InteractionManager = NULL;
  m_RemoteLogic = NULL;
  m_TestMode    = false;

  m_ImportMenu  = NULL; 
  m_ExportMenu  = NULL; 
  m_OpMenu      = NULL;
  m_ViewMenu    = NULL; 
  m_ViewListMenu= NULL; 
  m_EditMenu    = NULL;
  m_UserCommandIndex = 0;
  m_RecentFileMenu = NULL;

  m_MaterialChooser = NULL;

  // this is needed to manage events coming from the widget
  // when the user change the unit settings.
  m_MeasureUnitSettings->SetListener(this);

  m_PrintSupport = new mafPrintSupport();
  
  m_SettingsDialog = new mafGUISettingsDialog();

  m_ApplicationLayoutSettings = NULL;

  m_HelpSettings = NULL;

  m_Revision = _L("0.1");

  m_Extension = _R("msf");

  m_User = new mafUser();

  m_Config = wxConfigBase::Get();

  m_MakeBakFile = true;
  m_Storage     = NULL;
  mafString msfDir = mafGetApplicationDirectory();
  msfDir.ParsePathName();
  m_MSFDir   = msfDir;
  m_MSFFile  = _R("");
  m_ZipFile  = _R("");
  m_TmpDir   = _R("");

  m_SingleBinaryFile = false;

  m_FileHistoryIdx = -1;
}

//----------------------------------------------------------------------------
bool mafLogicWithManagers::SetAppTag(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(!vme->GetTagArray()->GetTag(_R("APP_STAMP")))
  {
    mafTagItem tag_appstamp;
    tag_appstamp.SetName(_R("APP_STAMP"));
    if(!m_AppStamp.empty())
      tag_appstamp.SetValue(m_AppStamp.at(0));
    else
      tag_appstamp.SetValue(_R(""));
    vme->GetTagArray()->SetTag(tag_appstamp);
    return true;
  }
  return false;
}


//----------------------------------------------------------------------------
void mafLogicWithManagers::AddCreationDate(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafString dateAndTime;
  wxDateTime time = wxDateTime::UNow(); // get time with millisecond precision
  dateAndTime  = mafString::Format(_R("%02d/%02d/%02d %02d:%02d:%02d"),time.GetDay(), time.GetMonth()+1, time.GetYear(), time.GetHour(), time.GetMinute(),time.GetSecond());

  mafTagItem tag_creationDate;
  tag_creationDate.SetName(_R("Creation_Date"));
  tag_creationDate.SetValue(dateAndTime);
  vme->GetTagArray()->SetTag(tag_creationDate); // set creation date tag for the specified vme
}

bool mafLogicWithManagers::CheckAppTag(mafNode *vme)
{
  mafString app_stamp;
  if(mafTagItem *ti = vme->GetTagArray()->GetTag(_R("APP_STAMP")))
    app_stamp = ti->GetValue();
  // First check for compatibility with all stored App stamps
  bool stamp_found = false;
  bool stamp_data_manager_found = false;
  bool stamp_open_all_found = false;
  for (int k=0; k<m_AppStamp.size(); k++)
  {
    // Check with the Application name
    if (app_stamp == m_AppStamp.at(k))
    {
      stamp_found = true;
    }
    // Check with the "Data Manager" tag
    if (m_AppStamp.at(k).Equals(_R("DataManager")))
    {
      stamp_data_manager_found = true;
    }
    // Check with the "OPEN_ALL_DATA" tag
    if (m_AppStamp.at(k).Equals(_R("OPEN_ALL_DATA")))
    {
      stamp_open_all_found = true;
    }
  }
  /*if(app_stamp.Equals("INVALID") || ((!stamp_found) && (!stamp_data_manager_found) && (!stamp_open_all_found)))
    return false;*/
  return true;
}

//----------------------------------------------------------------------------
void mafLogicWithManagers::SetSingleBinaryFile(bool singleFile)
//----------------------------------------------------------------------------
{
  m_SingleBinaryFile = singleFile;
}

//----------------------------------------------------------------------------
mafLogicWithManagers::~mafLogicWithManagers()
//----------------------------------------------------------------------------
{
  // Managers are destruct in the OnClose
  m_AppStamp.clear();
  cppDEL(m_User);
  cppDEL(m_ApplicationLayoutSettings);
  cppDEL(m_HelpSettings);
  cppDEL(m_PrintSupport);
  cppDEL(m_SettingsDialog); 
  cppDEL(m_Config);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Configure()
//----------------------------------------------------------------------------
{
  mafLogicWithGUI::Configure(); // create the GUI - and calls CreateMenu

  EnableItem(CAMERA_RESET, false);
  EnableItem(CAMERA_FIT,   false);
  EnableItem(CAMERA_FLYTO, false);
  EnableItem(MENU_FILE_PRINT, false);
  EnableItem(MENU_FILE_PRINT_PREVIEW, false);
  EnableItem(MENU_FILE_PRINT_SETUP, false);
  EnableItem(MENU_FILE_PRINT_PAGE_SETUP, false);

  if(this->m_PlugSidebar)
  {
    m_SideBar = new mafSideBar(m_Win,MENU_VIEW_SIDEBAR,this,m_SidebarStyle);
    m_Win->AddDockPane(m_SideBar->m_Notebook, wxAuiPaneInfo()
      .Name("sidebar")
      .Caption(wxT("ControlBar"))
      .Right()
      .Layer(2)
      .MinSize(240,450)
      .TopDockable(false)
      .BottomDockable(false)
      );
  }

  if(m_UseVMEManager)
  {
    m_NodeManager = new mafNodeManager();
    m_NodeManager->SetListener(this); 
  }

// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if (m_UseInteractionManager)
  {
    m_InteractionManager = new mafInteractionManager();
    m_InteractionManager->SetListener(this);
    mafPlugDevice<mmdRemoteFileManager>("mmdRemoteFileManager");

    SetGlobalMouse(m_InteractionManager->GetMouseDevice());
    //SIL m_InteractionManager->GetClientDevice()->AddObserver(this, MCH_INPUT);
  }
#endif

  if(m_UseViewManager)
  {
    m_ViewManager = new mafViewManager();
    m_ViewManager->SetListener(this);
  }

  if(m_UseOpManager)
  {
    m_OpManager = new mafOpManager();
    m_OpManager->SetListener(this);
    m_OpManager->WarningIfCantUndo(m_ApplicationSettings->GetWarnUserFlag());
  }
  
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if (m_UseInteractionManager && m_UseViewManager && m_UseOpManager)
  {
#ifdef __WIN32__
    m_RemoteLogic = new mafRemoteLogic(this, m_ViewManager, m_OpManager);

    m_RemoteLogic->SetClientUnit(m_InteractionManager->GetClientDevice());
#endif

  }
#endif

  // Fill the SettingsDialog
  m_SettingsDialog->AddPage( m_ApplicationSettings->GetGui(), m_ApplicationSettings->GetLabel());
  m_SettingsDialog->AddPage( m_StorageSettings->GetGui(), m_StorageSettings->GetLabel());

  if (m_ViewManager)
  {
    m_ApplicationLayoutSettings = new mafGUIApplicationLayoutSettings(this);
    m_ApplicationLayoutSettings->SetViewManager(m_ViewManager);
    m_ApplicationLayoutSettings->SetApplicationFrame(m_Win);
    m_SettingsDialog->AddPage( m_ApplicationLayoutSettings->GetGui(), m_ApplicationLayoutSettings->GetLabel());
  }

  m_SettingsDialog->AddPage( m_Win->GetDockSettingGui(), _("User Interface Preferences"));

  m_HelpSettings = new mafGUISettingsHelp(this);
  m_SettingsDialog->AddPage(m_HelpSettings->GetGui(), m_HelpSettings->GetLabel());

// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if(m_InteractionManager)
    m_SettingsDialog->AddPage(m_InteractionManager->GetGui(), _("Interaction Manager"));
#endif    
  if(m_LocaleSettings)
    m_SettingsDialog->AddPage(m_LocaleSettings->GetGui(), m_LocaleSettings->GetLabel());

  if (m_MeasureUnitSettings)
    m_SettingsDialog->AddPage(m_MeasureUnitSettings->GetGui(), m_MeasureUnitSettings->GetLabel());

  if (m_TimeBarSettings)
    m_SettingsDialog->AddPage(m_TimeBarSettings->GetGui(), m_TimeBarSettings->GetLabel());
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Plug(mafView* view, bool visibleInMenu)
//----------------------------------------------------------------------------
{
  if(m_ViewManager) 
  {
    long id = m_ViewManager->ViewAdd(view);
    if(visibleInMenu)
    {
      if(!m_ViewListMenu)
      {
        m_ViewListMenu = new wxMenu;
        m_ViewMenu->AppendSeparator();
        m_ViewMenu->Append(0,_("Add View"),m_ViewListMenu);
      }
      wxString s = view->GetLabel().toWx();
      mafID command = GetNewMenuId();
      m_ViewListMenu->Append(command, s, (wxMenu *)NULL, s );
      m_MenuElems.push_back(mafMenuElems(false, id, command));
    }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Plug(mafOp *op, const mafString& menuPath, bool canUndo, mafGUISettings *setting)
//----------------------------------------------------------------------------
{
  if(m_OpManager) 
  {
    mafString fullLabel = op->GetLabel();
    op->SetLabel(mafStripMenuCodes(fullLabel));
    long id = m_OpManager->OpAdd(op/*, canUndo/*, setting*/);
    wxMenu *path_menu = m_OpMenu;
    if(op->GetType() == OPTYPE_IMPORTER)
      path_menu = m_ImportMenu;
    else if(op->GetType() == OPTYPE_EXPORTER)
      path_menu = m_ExportMenu;
    else if(op->GetType() == OPTYPE_EDIT)
      path_menu = m_EditMenu;
    mafID command = GetNewMenuId();
    AddToMenu(fullLabel, command, path_menu, menuPath);
    m_MenuElems.push_back(mafMenuElems(true, id, command));

    
// currently mafInteraction is strictly dependent on VTK
#ifdef MAF_USE_VTK    
    if (m_InteractionManager)
    {
      if (const char **actions = op->GetActions())
      {
        const char *action;
        for (int i=0;action=actions[i];i++)
        {
          m_InteractionManager->AddAction(action);
        }
      }
    }
#endif
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Show()
//----------------------------------------------------------------------------
{
  if(m_NodeManager && m_RecentFileMenu)
  {
    m_FileHistory.UseMenu(m_RecentFileMenu);
    m_FileHistory.Load(*m_Config); // Loads file history from registry
  }

  mafLogicWithGUI::Show();
  EnableOperations(true);

  // must be after the mafLogicWithGUI::Show(); because in that method is set the m_AppTitle var
  SetApplicationStamp(m_AppTitle);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::SetApplicationStamp(const mafString &app_stamp)
//----------------------------------------------------------------------------
{
  // Add a single application stamp; this is done automatically while creating the application with the application name
  m_AppStamp.push_back(app_stamp);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::SetApplicationStamp(const std::vector<mafString>& app_stamp)
//----------------------------------------------------------------------------
{
  // Add a vector of time stamps; this can be done manually for adding compatibility with other applications. 
  // The application name itself must not be included since it was already added with the other call (see function above).
  m_AppStamp.insert(m_AppStamp.end(), app_stamp.begin(), app_stamp.end());
}
//----------------------------------------------------------------------------
mafUser *mafLogicWithManagers::GetUser()
//----------------------------------------------------------------------------
{
  return m_User;
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Init(int argc, char **argv)
//----------------------------------------------------------------------------
{
  if(m_NodeManager)
  {
    if(argc > 1 )
	  {
		  mafString file = _R(argv[1]);
		  if(mafFileExists(file))
		  {
			  OnFileOpen(file);
			  UpdateFrameTitle();
		  }
      else
      {
        OnFileNew();
      }
	  }
    else
    {
      OnFileNew();
    }
  }
  if (m_OpManager)
  {
    m_OpManager->FillSettingDialog(m_SettingsDialog);

    if(argc > 1 )
    {
      mafString op_type = _R(argv[1]);
      mafString op_param = _R(argv[2]);
      for (int p = 3; p < argc; p++)
      {
        op_param += _R(" ");
        op_param += _R(argv[p]);
      }
      m_OpManager->OpRun(op_type, (void *)op_param.toStd().c_str());
    }
  }

  m_ApplicationLayoutSettings->LoadLayout(true);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::CreateMenu()
//----------------------------------------------------------------------------
{
  m_MenuBar  = new wxMenuBar;
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(MENU_FILE_NEW,   _("&New  \tCtrl+N"));
  file_menu->Append(MENU_FILE_OPEN,  _("&Open   \tCtrl+O"));
  file_menu->Append(MENU_FILE_SAVE,  _("&Save  \tCtrl+S"));
  file_menu->Append(MENU_FILE_SAVEAS,_("Save &As  \tCtrl+Shift+S"));
  if (m_StorageSettings->UseRemoteStorage())
  {
    //file_menu->Append(MENU_FILE_UPLOAD, _("&Upload"));
  }
  m_ImportMenu = new wxMenu;
  file_menu->AppendSeparator();
  file_menu->Append(0,_("Import"),m_ImportMenu );

  m_ExportMenu = new wxMenu;
  file_menu->Append(0,_("Export"),m_ExportMenu);

  // Print menu item
  file_menu->AppendSeparator();
  file_menu->Append(MENU_FILE_PRINT, _("&Print  \tCtrl+P"));
  file_menu->Append(MENU_FILE_PRINT_PREVIEW, _("Print Preview"));
  file_menu->Append(MENU_FILE_PRINT_SETUP, _("Printer Setup"));
  file_menu->Append(MENU_FILE_PRINT_PAGE_SETUP, _("Page Setup"));

  m_RecentFileMenu = new wxMenu;
  file_menu->AppendSeparator();
  file_menu->Append(0,_("Recent Files"),m_RecentFileMenu);

  file_menu->AppendSeparator();
  file_menu->Append(MENU_FILE_QUIT,  _("&Quit  \tCtrl+Q"));

  m_MenuBar->Append(file_menu, _("&File"));

  m_EditMenu = new wxMenu;
  mafID undoCommand = GetNewMenuId();
  mafID redoCommand = GetNewMenuId();
  AddToMenu(_L("Undo  \tCtrl+Z"),      MENU_USER_START + 0,m_EditMenu);
  AddToMenu(_L("Redo  \tCtrl+Shift+Z"),MENU_USER_START + 1,m_EditMenu);
  m_EditMenu->AppendSeparator();
  m_MenuElems.push_back(mafMenuElems(true, 0, undoCommand));
  m_MenuElems.push_back(mafMenuElems(true, 0, redoCommand));
  m_EditMenu->Append(MENU_EDIT_FIND_VME, _("Find VME \tCtrl+F"));
  m_MenuBar->Append(m_EditMenu, _("&Edit"));

  m_ViewMenu = new wxMenu;
  m_MenuBar->Append(m_ViewMenu, _("&View"));

  m_OpMenu = new wxMenu;
  m_MenuBar->Append(m_OpMenu, _("&Operations"));

  wxMenu    *option_menu = new wxMenu;
  option_menu->Append(ID_APP_SETTINGS, _("Options..."));
  m_MenuBar->Append(option_menu, _("Tools"));

	wxMenu    *help_menu = new wxMenu;
	help_menu->Append(ABOUT_APPLICATION,_("About"));
	help_menu->Append(HELP_HOME, _("Help"));

	m_MenuBar->Append(help_menu, _("&Help"));
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::CreateToolbar()
//----------------------------------------------------------------------------
{
  
  //m_ToolBar = new wxToolBar(m_Win,-1,wxPoint(0,0),wxSize(-1,-1),wxHORIZONTAL|wxNO_BORDER|wxTB_FLAT  );
  m_ToolBar = new wxToolBar(m_Win,MENU_VIEW_TOOLBAR,wxPoint(0,0),wxSize(-1,-1),wxTB_FLAT | wxTB_NODIVIDER );
  m_ToolBar->SetMargins(0,0);
  m_ToolBar->SetToolSeparation(2);
  m_ToolBar->SetToolBitmapSize(wxSize(20,20));
  m_ToolBar->AddTool(MENU_FILE_NEW, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("FILE_NEW")),    (_L("new ") + m_Extension + _L(" storage file")).toWx());
  m_ToolBar->AddTool(MENU_FILE_OPEN, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("FILE_OPEN")),  (_L("open ") + m_Extension + _L(" storage file")).toWx());
  m_ToolBar->AddTool(MENU_FILE_SAVE, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("FILE_SAVE")),  (_L("save current ") + m_Extension + _L(" storage file")).toWx());
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(MENU_FILE_PRINT, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("PRINT")),  _("print the selected view"));
  m_ToolBar->AddTool(MENU_FILE_PRINT_PREVIEW, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("PRINT_PREVIEW")),  _("show the print preview for the selected view"));
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(MENU_USER_START + 0, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("OP_UNDO")),  _("undo (ctrl+z)"));
  m_ToolBar->AddTool(MENU_USER_START + 1, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("OP_REDO")),  _("redo (ctrl+shift+z)"));
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(MENU_USER_START + 2, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("OP_DELETE")),  _("delete selected vme (ctrl+shift+d)"));
  m_ToolBar->AddTool(MENU_USER_START + 3, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("OP_CUT")),  _("cut selected vme (ctrl+x)"));
  m_ToolBar->AddTool(MENU_USER_START + 4, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("OP_COPY")), _("copy selected vme (ctrl+c)"));
  m_ToolBar->AddTool(MENU_USER_START + 5, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("OP_PASTE")),_("paste vme (ctrl+v)"));
  m_ToolBar->AddSeparator();
  m_ToolBar->AddTool(CAMERA_RESET, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("ZOOM_ALL")),_("reset camera to fit all (ctrl+f)"));
  m_ToolBar->AddTool(CAMERA_FIT, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("ZOOM_SEL")),_("reset camera to fit selected object (ctrl+shift+f)"));
  m_ToolBar->AddTool(CAMERA_FLYTO, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("FLYTO")),_("fly to object under mouse"));
  m_ToolBar->Realize();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::UpdateFrameTitle()
//----------------------------------------------------------------------------
{
  mafString title(m_AppTitle);
  if(!m_MSFFile.IsEmpty())
    title += _R("   ") + m_MSFFile;
  m_Win->SetTitle(title.toWx());
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  mafEvent *e = mafEvent::SafeDownCast(maf_event);
  if(!e)
  {
    mafLogicWithGUI::OnEvent(maf_event);
    return;
  }
  mafID eventId = e->GetId();
  if (mafDataVector::GetSingleFileDataId() == eventId)
  {
    e->SetBool(m_SingleBinaryFile);
    return;
  }
  for(int i = 0; i < m_MenuElems.size(); i++)
  {
    if(eventId != m_MenuElems[i].m_command)
      continue;
    if(!m_MenuElems[i].m_op)
    {
      if(m_ViewManager)
        m_ViewManager->ViewCreate(m_MenuElems[i].m_id + VIEW_START);
      return;
    }
    if(!m_OpManager) 
      break;
    EnableOperations(false);
    if(i == 0)
    {
      m_OpManager->OpUndo();
      EnableOperations(true);
      return;
    }
    if(i == 1)
    {
      m_OpManager->OpRedo();
      EnableOperations(true);
      return;
    }
    m_OpManager->OpRun(m_MenuElems[i].m_id, m_OpManager->GetSelectedVme());
    EnableOperations(true);
    return;
  }
  // ###############################################################
  // commands related to FILE MENU  
  if(MENU_FILE_NEW == eventId)
  {
    OnFileNew();
    return; 
  }
  if(MENU_FILE_OPEN == eventId)
  {
    mafString *filename = e->GetString();
    if(filename)
      OnFileOpen(*filename);
    else
      OnFileOpen();
    UpdateFrameTitle();
    return; 
  }
  if(IMPORT_FILE == eventId)
  {
    mafString *filename = e->GetString();
    if(filename)
    {
      ImportExternalFile(*filename);
    }
    return;
  }
  if(wxID_FILE1 <= eventId && eventId <= wxID_FILE9)
  {
    OnFileHistory(e->GetId() - wxID_FILE1);
    return;
  }
  if(MENU_FILE_SAVE == eventId)
  {
    OnFileSave();
    return; 
  }
  if(MENU_FILE_SAVEAS == eventId)
  {
    OnFileSaveAs();
    return;
  }
  if(MENU_FILE_UPLOAD == eventId)
  {
    /*  Re-think about it!!
    wxString remote_path = "";
    wxString remote_file = "";
    bool valid_dir = false;
    wxString msg = _("Insert remote directory on remote host: ") + m_StorageSettings->GetRemoteHostName();
    do 
    {
      remote_path = wxGetTextFromUser(msg,_("Remote directory choose"),"/mafstorage/pub/");
      if (!remote_path.IsEmpty())
      {
        remote_file = m_StorageSettings->GetRemoteHostName();
        remote_file << remote_path;
      }
      else
        return;
      wxString dir_check = remote_file[remote_file.Length()-1];
      valid_dir = dir_check.IsSameAs("/") || dir_check.IsSameAs("\\");
      if (!valid_dir)
      {
        wxMessageBox(_("Not valid path!! It should ends with '/'"), _("Warning"));
      }
    } while(!valid_dir);

    // for now upload the entire msf
    remote_file += wxFileNameFromPath(m_VMEManager->GetFileName());
    OnFileUpload(remote_file.c_str());
    */
    return;
  }

  if(MENU_FILE_PRINT == eventId)
  {
    if (m_ViewManager && m_PrintSupport)
      m_PrintSupport->OnPrint(m_ViewManager->GetSelectedView());
    return;
  }
  if(MENU_FILE_PRINT_PREVIEW == eventId)
  {
    if (m_ViewManager && m_PrintSupport)
      m_PrintSupport->OnPrintPreview(m_ViewManager->GetSelectedView());
    return;
  }
  if(MENU_FILE_PRINT_SETUP == eventId)
  {
    if (m_PrintSupport)
      m_PrintSupport->OnPrintSetup();
    return;
  }
  if(MENU_FILE_PRINT_PAGE_SETUP == eventId)
  {
    if (m_PrintSupport)
      m_PrintSupport->OnPageSetup();
    return;
  }
  if(MENU_FILE_QUIT == eventId)
  {
    OnQuit();		
    return; 
  }
  // ###############################################################
  // commands related to VME
  if(MENU_EDIT_FIND_VME == eventId)
  {
    FindVME();
    return;
  }
  if(VME_SELECT == eventId)
  {
    VmeSelect(*e);		
    return;
  }
  if(VME_SELECTED == eventId)
  {
    VmeSelected(e->GetVme(), false);
    return;
  }
  if(VME_DCLICKED == eventId)
  {
    VmeDoubleClicked(*e);
    return;
  }
  if(VME_SHOW == eventId)
  {
    VmeShow(e->GetVme(), e->GetBool());
    if(m_RemoteLogic && (e->GetSender() != m_RemoteLogic) && m_RemoteLogic->IsSocketConnected())
    {
      m_RemoteLogic->VmeShow(e->GetVme(), e->GetBool());
    }
    return;
  }
  if(VME_MODIFIED == eventId)
  {
    VmeModified(e->GetVme());
    if(!m_PlugTimebar && ((mafVME*)e->GetVme())->IsAnimated())
      m_Win->ShowDockPane("timebar",!m_Win->DockPaneIsShown("timebar") );
    return; 
  }
  if(VME_EXPAND == eventId)
  {
    VmeExpand(e->GetVme());
    return; 
  }
  if(VME_COLLAPSE == eventId)
  {
    VmeCollapse(e->GetVme());
    return; 
  }
  if(VME_EXPANDSUBTREE == eventId)
  {
    VmeExpandSubTree(e->GetVme());
    return; 
  }
  if(VME_COLLAPSESUBTREE == eventId)
  {
    VmeCollapseSubTree(e->GetVme());
    return; 
  }
  if(VME_EXPANDVISIBLE == eventId)
  {
    VmeExpandVisible(e->GetVme());
    return; 
  }
  if(VME_ADD == eventId)
  {
    VmeAdd(e->GetVme());
    return; 
  }
  if(VME_ADDED == eventId)
  {
    VmeAdded(e->GetVme());
    return; 
  }
  if(VME_REMOVE == eventId)
  {
    VmeRemove(e->GetVme());
    return; 
  }
  if(VME_REMOVING == eventId)
  {
    VmeRemoving(e->GetVme());
    return; 
  }
  if(VME_CHOOSE == eventId)
  {
    mafString *s = e->GetString();
    if(s != NULL)
    {
      std::vector<mafNode*> nodeVector = VmeChoose(e->GetArg(), REPRESENTATION_AS_TREE, *s, e->GetBool());
      if (!e->GetBool())
      {
        if (nodeVector.size() != 0)
        {
          e->SetVme(nodeVector[0]);
        }
        else
        {
          e->SetVme(NULL);
        }
      }
      else
      {
        e->SetVmeVector(nodeVector);
      }
    }
    else
    {
      std::vector<mafNode*> nodeVector = VmeChoose(e->GetArg(), REPRESENTATION_AS_TREE, _R("Choose Node"), e->GetBool());
      if (!e->GetBool())
      {
        if (nodeVector.size() != 0)
        {
          e->SetVme(nodeVector[0]);
        }
        else
        {
          e->SetVme(NULL);
        }
      }
      else
      {
        e->SetVmeVector(nodeVector);
      }
    }
    return;
  }
  if(VME_CHOOSE_MATERIAL == eventId)
  {
    VmeChooseMaterial((mafVME *)e->GetVme(), e->GetBool());
    return;
  }
  if(VME_VISUAL_MODE_CHANGED == eventId)
  {
    mafVME *vme = (mafVME *)e->GetVme();
    VmeShow(vme, false);
    VmeShow(vme, true);
    return;
  }
  if(UPDATE_PROPERTY == eventId)
  {
    VmeUpdateProperties((mafVME *)e->GetVme(), e->GetBool());
    return;
  }
  if(SHOW_CONTEXTUAL_MENU == eventId)
  {
    if (e->GetSender() == m_SideBar->GetTree())
      TreeContextualMenu(*e);
    else
      ViewContextualMenu(e->GetBool());
    return;
  }
  // ###############################################################
  // commands related to OP
  if(MENU_OP == eventId)
  {
    if(m_OpManager) 
    {
      m_OpManager->OpRun(e->GetArg());
      if(/*m_OpManager->GetRunningOperation() && */m_RemoteLogic && m_RemoteLogic->IsSocketConnected() && !m_OpManager->m_FromRemote)
      {
        mafEvent re(this, mafOpManager::RUN_OPERATION_EVENT, e->GetArg());
        re.SetChannel(REMOTE_COMMAND_CHANNEL);
        m_RemoteLogic->OnEvent(&re);
      }
    }
    return;
  }
  if(PARSE_STRING == eventId)
  {
    if(this->m_OpManager->Running())
    {
      wxMessageBox("There is an other operation running!!");
      return;
    }
    int menuId, opId;
    mafString *s = e->GetString();
    menuId = m_MenuBar->FindMenu(_("Operations"));
    opId = m_MenuBar->GetMenu(menuId)->FindItem(s->toWx());
    m_OpManager->OpRun(opId);
    return;
  }
  if(MENU_OPTION_APPLICATION_SETTINGS == eventId)
  {
    if (m_OpManager)
    {
      m_OpManager->WarningIfCantUndo(m_ApplicationSettings->GetWarnUserFlag());
      return;
    }
  }
  if(CLEAR_UNDO_STACK == eventId)
  {
    if (!m_OpManager->Running())
    {
      m_OpManager->ClearUndoStack();
      EnableOperations(true);
    }
    return;
  }
  if(OP_RUN_STARTING == eventId)
  {
    mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
    if (c != NULL)
      c->SetAllowCloseWindow(false);
    OpRunStarting();
    return; 
  }
  if(OP_RUN_TERMINATED == eventId)
  {
    mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
    if (c != NULL)
      c->SetAllowCloseWindow(true);
    OpRunTerminated();
    return; 
  }
  if(OP_SHOW_GUI == eventId)
  {
    OpShowGui(!e->GetBool(), (mafGUIPanel*)e->GetWin());
    return; 
  }
  if(OP_HIDE_GUI == eventId)
  {
    OpHideGui(e->GetBool());
    return;
  }
  if(OP_FORCE_STOP == eventId)
  {
    m_OpManager->StopCurrentOperation();
    return;
  }
  // ###############################################################
  // commands related to VIEWS
  if(VIEW_CREATE == eventId)
  {
    ViewCreate(e->GetArg());
    return;
  }
  if(VIEW_CREATED == eventId)
  {
    ViewCreated(e->GetView());
    return;
  }
  if(VIEW_RESIZE == eventId)
  {
    mafView *view = NULL;
    const char *viewStr=e->GetString()->GetCStr();
    view=m_ViewManager->GetFromList(viewStr);
    if(view) 
    {
      view->GetFrame()->SetSize(e->GetWidth(),e->GetHeight());
      view->GetFrame()->SetPosition(wxPoint(e->GetX(),e->GetY()));
    }
  }
  if(VIEW_DELETE == eventId)
  {

    if(m_PlugSidebar)
      this->m_SideBar->ViewDeleted(e->GetView());

#ifdef MAF_USE_VTK
    // currently mafInteraction is strictly dependent on VTK (marco)
    if(m_InteractionManager)
      m_InteractionManager->ViewSelected(NULL);
#endif

    if (m_ViewManager)
    {
      EnableItem(CAMERA_RESET, false);
      EnableItem(CAMERA_FIT,   false);
      EnableItem(CAMERA_FLYTO, false);

      EnableItem(MENU_FILE_PRINT, false);
      EnableItem(MENU_FILE_PRINT_PREVIEW, false);
      EnableItem(MENU_FILE_PRINT_SETUP, false);
      EnableItem(MENU_FILE_PRINT_PAGE_SETUP, false);
    }
    if (m_OpManager)
    {
      EnableOperations(!m_OpManager->Running());
    }
    return;
  }
  if(VIEW_SELECT == eventId)
  {
    ViewSelect();
    if (m_OpManager)
    {
      mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
      if (c != NULL)
        c->SetAllowCloseWindow(!m_OpManager->Running());
    }
    return;
  }
  if(VIEW_MAXIMIZE == eventId)
  {
    if (m_RemoteLogic && m_RemoteLogic->IsSocketConnected() && !m_ViewManager->m_FromRemote)
    {
      m_RemoteLogic->RemoteMessage(*e->GetString());
    }
    return;
  }
  if(VIEW_SELECTED == eventId)
  {
    e->SetBool(m_ViewManager->GetSelectedView() != NULL);
    e->SetView(m_ViewManager->GetSelectedView());
    return;
  }
  if(VIEW_SAVE_IMAGE == eventId)
  {
    mafViewCompound *v = mafViewCompound::SafeDownCast(m_ViewManager->GetSelectedView());
    if (v && e->GetBool())
    {
      v->GetRWI()->SaveAllImages(v->GetLabel(),v, m_ApplicationSettings->GetImageTypeId());
    }
    else
    {
      mafView *v = m_ViewManager->GetSelectedView();
      if (v)
      {
        v->GetRWI()->SaveImage(v->GetLabel());
      }
    }
    return;
  }
  if(LAYOUT_LOAD == eventId)
  {
    RestoreLayout();
    return;
  }
  if(CAMERA_RESET == eventId)
  {
    if(m_ViewManager) m_ViewManager->CameraReset();
    return; 
  }
  if(CAMERA_FIT == eventId)
  {
    if(m_ViewManager) m_ViewManager->CameraReset(true);
    return;
  }
  if(CAMERA_FLYTO == eventId)
  {
    if(m_ViewManager) m_ViewManager->CameraFlyToMode();
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if(m_InteractionManager) m_InteractionManager->CameraFlyToMode();  //modified by Marco. 15-9-2004 fly to with devices.
#endif
    return;
  }
  if(LINK_CAMERA_TO_INTERACTOR == eventId)
  {
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if (m_InteractionManager == NULL || m_InteractionManager->GetPER() == NULL)
      return;

    vtkCamera *cam = vtkCamera::SafeDownCast(e->GetVtkObj());
    bool link_camera = e->GetBool();
    if (!link_camera) 
    {
      if (cam) 
        m_InteractionManager->GetPER()->LinkCameraRemove(cam);
      else
        m_InteractionManager->GetPER()->LinkCameraRemoveAll();

      if (m_CameraLinkingObserverFlag) 
      {
        m_InteractionManager->GetPER()->GetCameraMouseInteractor()->RemoveObserver(this);
        m_CameraLinkingObserverFlag = false;
      }
    }
    else if (cam) 
    {
      if (!m_CameraLinkingObserverFlag) 
      {
        m_InteractionManager->GetPER()->GetCameraMouseInteractor()->AddObserver(this);
        m_CameraLinkingObserverFlag = true;
      }
      m_InteractionManager->GetPER()->LinkCameraAdd(cam);
    }
#endif
    return;
  }
  if(TIME_SET == eventId)
  {
    TimeSet(e->GetDouble());
    return;
  }
  // ###############################################################
  // commands related to interaction manager
  if(ID_APP_SETTINGS == eventId)
  {
    m_SettingsDialog->ShowModal();
    return;
  }
  if(mafGUIMeasureUnitSettings::MEASURE_UNIT_UPDATED == eventId)
  {
    UpdateMeasureUnit();
    return;
  }
  if(CAMERA_PRE_RESET == eventId)
  {
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if(m_InteractionManager) 
    {
      vtkRenderer *ren = (vtkRenderer*)e->GetVtkObj();
      //assert(ren);
      m_InteractionManager->PreResetCamera(ren);
      //mafLogMessage("CAMERA_PRE_RESET");
    }
#endif
    return;
  }
  if(CAMERA_POST_RESET == eventId)
  {
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if(m_InteractionManager) 
    {
      vtkRenderer *ren = (vtkRenderer*)e->GetVtkObj();
      //assert(ren); //modified by Marco. 2-11-2004 Commented out to allow reset camera of all cameras.
      m_InteractionManager->PostResetCamera(ren);
      //mafLogMessage("CAMERA_POST_RESET");
    }
#endif
    return;
  }
  if(CAMERA_UPDATE == eventId)
  {
    if(m_ViewManager) m_ViewManager->CameraUpdate();
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
		//PERFORMANCE WARNING : if view manager exist this will cause another camera update!!
		// An else could be added to reduce CAMERA_UPDATE by a 2 factor. This has been done for the HipOp vertical App showing
		// big performance improvement in composite views creation.
    if(m_InteractionManager) m_InteractionManager->CameraUpdate(e->GetView());
#endif
    return;
  }
  if(CAMERA_SYNCHRONOUS_UPDATE == eventId)     
  {
    m_ViewManager->CameraUpdate();
    return;
  }
  // ###############################################################
  // commands related to interaction manager
  if(INTERACTOR_ADD == eventId)
  {
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if(m_InteractionManager)
    {
      mafInteractor *interactor = mafInteractor::SafeDownCast(e->GetMafObject());
      assert(interactor);
      mafString *action_name = e->GetString();
      m_InteractionManager->BindAction(action_name->GetCStr(),interactor);
    }
#endif
    return;
  }
  if(INTERACTOR_REMOVE == eventId)
  {
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if(m_InteractionManager) 
    {
      mafInteractor *interactor = mafInteractor::SafeDownCast(e->GetMafObject());
      assert(interactor);
      mafString *action_name = e->GetString();
      m_InteractionManager->UnBindAction(action_name->GetCStr(),interactor);
    }
#endif
    return;
  }
  if(PER_PUSH == eventId)
  {
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if(m_InteractionManager)
    {
      mafInteractorPER *per = mafInteractorPER::SafeDownCast(e->GetMafObject());
      assert(per);
      m_InteractionManager->PushPER(per);
    }
#endif
    return; 
  }
  if(PER_POP == eventId)
  {
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if(m_InteractionManager) m_InteractionManager->PopPER();
#endif
    return;
  }
  if(DEVICE_ADD == eventId)
  {
    m_InteractionManager->AddDeviceToTree((mafDevice *)e->GetMafObject());
    return;
  }
  if(DEVICE_REMOVE == eventId)
  {
    m_InteractionManager->RemoveDeviceFromTree((mafDevice *)e->GetMafObject());
    return;
  }
  if(DEVICE_GET == eventId)
  {
    return;
  }
  if(COLLABORATE_ENABLE == eventId)
  {
    bool collaborate = e->GetBool();
    if (collaborate)
    {
      m_RemoteLogic->SetRemoteMouse(m_InteractionManager->GetRemoteMouseDevice());
      GetGlobalMouse()->AddObserver(m_RemoteLogic, REMOTE_COMMAND_CHANNEL);
      if(m_RemoteLogic->IsSocketConnected())  //check again, because if no server is present
      {                                       //no synchronization is necessary
        m_RemoteLogic->SynchronizeApplication();
      }
    }
    else
    {
      if (m_RemoteLogic != NULL)
      {
        m_RemoteLogic->SetRemoteMouse(NULL);
        m_RemoteLogic->Disconnect();
        GetGlobalMouse()->RemoveObserver(m_RemoteLogic);
      }
    }
    m_ViewManager->Collaborate(collaborate);
    m_OpManager->Collaborate(collaborate);
    GetGlobalMouse()->Collaborate(collaborate);
    return;
  }
  if(ABOUT_APPLICATION == eventId)
  {
    mafString message = m_AppTitle;
    message += _L(" Application ");
    message += m_Revision;
    wxMessageBox(message.toWx(), "About Application");
    return;
  }
  if(HELP_HOME == eventId)
  {
    if (m_HelpSettings)
    {
      m_HelpSettings->OpenHelpPage(_R("HELP_HOME"));
    }
  }
  if(GET_BUILD_HELP_GUI == eventId)
  {
    int buildGui = -1;
    if (m_HelpSettings == NULL)
    {
      buildGui = false;
      e->SetArg(buildGui);
    }
    else
    {
      buildGui = m_HelpSettings->GetBuildHelpGui();
    }
    e->SetArg(buildGui);
  }
  if(OPEN_HELP_PAGE == eventId)
  {
  // open help for entity
    m_HelpSettings->OpenHelpPage(*e->GetString());
  }
  mafLogicWithGUI::OnEvent(maf_event);
}
//----------------------------------------------------------------------------
bool mafLogicWithManagers::OnFileClose(bool force)
//----------------------------------------------------------------------------
{
  if(!m_NodeManager)
    return true;
  if(!force && !AskConfirmAndSave())
    return false;
  OnEvent(&mafEvent(this,CLEAR_UNDO_STACK)); // ask logic to clear the undo stack
  if(m_Storage && !m_TmpDir.IsEmpty())
  {
    mafRemoveDirectory(m_TmpDir); // remove the temporary directory
    m_TmpDir.Clear();
  }
  m_NodeManager->SetRoot(NULL);
  m_NodeManager->MSFModified(false);
  mafDEL(m_Storage);
  m_NodeManager->SetListener(this);
  VmeSelected(NULL);
  m_MSFFile.Clear();
  m_ZipFile.Clear(); 
  UpdateFrameTitle();
  return true;
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileNew()
//----------------------------------------------------------------------------
{
  if(!m_NodeManager)
    return;
  if(!OnFileClose())
    return;
  mafVMERoot *root;
  mafNEW(root);
  root->SetName(_R("root"));
  root->Initialize();
  //Add the application stamps
  SetAppTag(root);
  AddCreationDate(root);
  m_NodeManager->SetRoot(root);
  VmeSelected(root);
  root->SetTreeTime(0.0); // set the tree time
  UpdateFrameTitle();
  mafDEL(root);
  m_NodeManager->MSFModified(false);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileUpload(const char *remote_file, unsigned int upload_flag)
//----------------------------------------------------------------------------
{
  if (remote_file == NULL)
  {
    wxMessageBox(_("remote filename not valid!!"), _("Warning"));
    return;
  }

  wxMessageBox(_("Not implemented: think about it!!"), _("Warning"));
}
//----------------------------------------------------------------------------
bool mafLogicWithManagers::OnFileOpen(const mafString& file_to_open)
//----------------------------------------------------------------------------
{
  //!!remember upgrade of data to current VERSION!!!!!!!!!!!!!!!!!!!!!!!
  if(!m_NodeManager)
    return false;

  if(!AskConfirmAndSave())
    return false;

  mafString file = file_to_open;
  if(file.IsEmpty())
  {
    if (m_StorageSettings->GetStorageType() == mafGUISettingsStorage::HTTP)
    {
      mafGUIDialogRemoteFile remoteFile;
      remoteFile.ShowModal();
      file = remoteFile.GetFile();
    }
    else
    {
		  mafString wildc = _R("MAF Storage Format file (*.");
		  wildc += m_Extension;
		  wildc += _R(")|*.");
		  wildc += m_Extension;
			wildc += _R("|Compressed file (*.z") + m_Extension + _R(")|*.z") + m_Extension;
		  //mafString wildc    = _("MAF Storage Format file (*.msf)|*.msf|Compressed file (*.zmsf)|*.zmsf");
      file = mafGetOpenFile(_R(""), wildc);
    }
    if(file.IsEmpty())
      return false;
  }

  if(!OnFileClose(true))
    return false;

  mafString protocol;
  bool remote_file = false;
  if (IsRemote(file,protocol))
  {
    remote_file = true;
    const mafString& cache_folder = m_StorageSettings->GetCacheFolder();
    if (!mafDirExists(cache_folder))
      mafDirMake(cache_folder);
    mafRemoteStorage *rs = mafRemoteStorage::New();
    if(!rs)
      return false;
    rs->SetTmpFolder(cache_folder);
    rs->SetHostName(m_StorageSettings->GetRemoteHostName());
    rs->SetRemotePort(m_StorageSettings->GetRemotePort());
    rs->SetUsername(m_StorageSettings->GetUserName());
    rs->SetPassword(m_StorageSettings->GetPassword());
    rs->Initialize();
    m_Storage = rs;
  }
  else
  {
    if(!mafFileExists(file))
    {
      mafString msg;
      msg = _L("File ");
      msg += file;
      msg += _L(" not found!");
      mafWarningMessage(_M(msg));

      if(m_FileHistoryIdx != -1)
      {
        m_FileHistory.RemoveFileFromHistory(m_FileHistoryIdx); // remove filename to history
        m_FileHistory.Save(*m_Config); // Save file history to registry
        m_FileHistoryIdx = -1;
      }
      return false;
    }
    m_Storage = mafVMEStorage::New();
  }

  if(!m_Storage)
    return false;
  m_Storage->SetListener(this);
  m_Storage->SetManager(m_NodeManager);

  wxWindowDisabler *disableAll;
  wxBusyCursor *wait_cursor;
  if(!m_TestMode) // Losi 02/16/2010 for test class
  {
    disableAll = new wxWindowDisabler();
    wait_cursor = new wxBusyCursor();
  }
  mafString unixname = file;
  mafString path, name, ext;
  mafSplitPath(file,&path,&name,&ext);
  if(ext == _R("zmsf"))
  {
    if (remote_file) // download remote zmsf
    {
      // Download the file if it is not present into the cache
      // we are using the remote storage!!
      mafString local_filename, remote_filename;
      remote_filename = file;
      local_filename = m_Storage->GetTmpFolder();
      local_filename += _R("/");
      local_filename += name;
      local_filename += _R(".zmsf");
      ((mafRemoteStorage *)m_Storage)->GetRemoteFileManager()->DownloadRemoteFile(remote_filename, local_filename); // download the remote file in the download cache
      file = local_filename;
    }
    m_ZipFile = file;
    unixname = mafOpenZIP(file, m_Storage->GetTmpFolder(), m_TmpDir); // open the zmsf archive and extract it to the temporary directory
    if(unixname.IsEmpty())
    {
      mafMessage(_M(mafString(_L("Bad or corrupted zmsf file!"))));
      m_NodeManager->SetListener(this);
      mafDEL(m_Storage);
      if(!m_TestMode) // Losi 02/16/2010 for test class
      {
        cppDEL(disableAll);
        cppDEL(wait_cursor);
      }
      return false;
    }
    wxSetWorkingDirectory(m_TmpDir.toWx());
  }

  unixname.ParsePathName();

  m_MSFFile = unixname; 
  m_Storage->SetURL(m_MSFFile);


  int res = m_Storage->Restore();
  if(res != mafStorage::IO_OK && res != mafStorage::IO_WRONG_OBJECT_TYPE)
  {
    mafDEL(m_Storage);
    m_NodeManager->SetListener(this);
    if(!m_TestMode) // Losi 02/16/2010 for test class
    {
      cppDEL(disableAll);
      cppDEL(wait_cursor);
    }
    return false;
  }
  if(res == mafStorage::IO_WRONG_OBJECT_TYPE)
  {
    mafErrorMessage(_M(mafString(_L("Errors during file parsing! Look the log area for error messages."))));
  }
  mafVMERoot *root = mafVMERoot::SafeDownCast(m_NodeManager->GetRoot());
  SetAppTag(root);
  if(!CheckAppTag(root))
  {
    //Application stamp not valid
    mafMessage(_M(mafString(_L("File not valid for this application!"))));
    mafDEL(m_Storage);
    m_NodeManager->SetListener(this);
    m_NodeManager->SetRoot(NULL);
    if(!m_TestMode) // Losi 02/16/2010 for test class
    {
      cppDEL(disableAll);
      cppDEL(wait_cursor);
    }
    return false;
  }
  //root->Initialize();
  //m_NodeManager->SetRoot(root);
  VmeSelected(root);
  mafTimeStamp b[2] = {0.0, 0.0};
  root->GetOutput()->GetTimeBounds(b);
  root->SetTreeTime(b[0]); // Set tree time to the starting time
  mafNodeIterator *iter = root->NewIterator();
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
  {
    mafVMEGenericAbstract *vmeWithDataVector = mafVMEGenericAbstract::SafeDownCast(node);
    if (vmeWithDataVector)
    {
      mafDataVector *dv = vmeWithDataVector->GetDataVector();
      if (dv != NULL)
      {
        SetSingleBinaryFile(dv->GetSingleFileMode());
        break;
      }
    }
  }
  iter->Delete();

  RestoreLayout();
  if(!m_TestMode) // Losi 02/16/2010 for test class
  {
    cppDEL(disableAll);
    cppDEL(wait_cursor);
  }

  if (!m_TmpDir.IsEmpty())
  {
    m_FileHistory.AddFileToHistory(m_ZipFile.toWx()); // add the zmsf file to the history
  }
  else if(/*!remote_file && */res == MAF_OK)
  {
    m_FileHistory.AddFileToHistory(m_MSFFile.toWx()); // add the msf file to the history
  }
  else if(res != MAF_OK && m_FileHistoryIdx != -1)
  {
    m_FileHistory.RemoveFileFromHistory(m_FileHistoryIdx); // if something get wrong retoring the file remove it from istory
  }
  m_FileHistory.Save(*m_Config); // save file history to registry
  return true;
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileHistory(int fileId)
//----------------------------------------------------------------------------
{
  m_FileHistoryIdx = fileId;
  OnFileOpen(mafWxToString(m_FileHistory.GetHistoryFile(fileId)));
  m_FileHistoryIdx = -1;
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Save()
//----------------------------------------------------------------------------
{
  if(!m_NodeManager)
    return;
	mafString save_default_folder = m_StorageSettings->GetDefaultSaveFolder();
	save_default_folder.ParsePathName();
	m_MSFDir = save_default_folder;
  mafVMERoot *root = mafVMERoot::SafeDownCast(m_NodeManager->GetRoot());
  if(!root)
    return;
  if(m_MSFFile.IsEmpty())
  {
    assert(false);
    return;
  }
  if(!m_Storage)
    return;
  if(mafFileExists(m_MSFFile) && m_MakeBakFile) // an msf with the same name exists
  {
    mafString bak_filename = m_MSFFile + _R(".bak");                // create the backup for the saved msf
    mafFileRename(m_MSFFile, bak_filename);  // renaming the founded one
  }
  wxBusyInfo *bi = NULL;
  if(!m_TestMode) // Losi 02/16/2010 for test class 
  {
    bi = new wxBusyInfo(_("Saving MSF: Please wait"));
	Sleep(1500);
  }
  if (m_Storage->Store() != MAF_OK) // store the tree
  {
    mafLogMessage(_M(mafString(_L("Error during MSF saving"))));
    if(!m_TestMode) // Losi 02/16/2010 for test class 
    {
      cppDEL(bi);
    }
    return;
  }
  // add the msf (or zmsf) to the history

  m_MakeBakFile = true;
  UpdateFrameTitle();
  m_NodeManager->MSFModified(false);
  m_FileHistory.Save(*m_Config);
  if(!m_TestMode) // Losi 02/16/2010 for test class 
  {
    cppDEL(bi);
  }
}
//----------------------------------------------------------------------------
bool mafLogicWithManagers::OnFileSave()
//----------------------------------------------------------------------------
{
  if(!m_NodeManager)
    return true;
	mafString save_default_folder = m_StorageSettings->GetDefaultSaveFolder();
	save_default_folder.ParsePathName();
	m_MSFDir = save_default_folder;
  mafVMERoot *root = mafVMERoot::SafeDownCast(m_NodeManager->GetRoot());
  if(!root)
    return true;;

  if(m_Storage && m_Storage->GetURL() != m_MSFFile)
  {
    assert(false);
  }
  if(m_MSFFile.IsEmpty()) 
    return OnFileSaveAs();
  Save();
  return true;
}
//----------------------------------------------------------------------------
bool mafLogicWithManagers::OnFileSaveAs()
//----------------------------------------------------------------------------
{
  if(!m_NodeManager)
    return true;
  mafVMERoot *root = mafVMERoot::SafeDownCast(m_NodeManager->GetRoot());
  if(!root)
    return true;

  m_MSFFile.Clear(); // set filenames to empty so the MSFSave method will ask for them
  m_ZipFile.Clear();
  m_MakeBakFile = false;

  // new file to save: ask to the application which is the default
  // modality to save binary files.
  /*mafEvent e(this,mafDataVector::SINGLE_FILE_DATA);
  OnEvent(&e);
  SetSingleBinaryFile(e.GetBool()); // set the save modality for time-varying vme*/

  // ask for the new file name.
  mafString wildc = _L("MAF Storage Format file (*.msf)|*.msf|Compressed file (*.zmsf)|*.zmsf");
  mafString file = mafGetSaveFile(m_MSFDir, wildc);
  if(file.IsEmpty())
    return false;

  if(!mafFileExists(file))
  {
    mafString path, name, ext, file_dir;
    mafSplitPath(file,&path,&name,&ext);
    size_t length = path.Length();
    if(length > 0 && path[length - 1] != '\\' && path[length - 1] != '/')
      file_dir = path + _R("/");
    file_dir = path + _R("/") + name;
    if(!mafDirExists(file_dir))
      mafDirMake(file_dir);
    if (ext == _R("zmsf"))
    {
      m_ZipFile = file;
      m_TmpDir = file_dir;
      ext = _R("msf");
    }
    file = file_dir + _R("/") + name + _R(".") + ext;
  }

  file.ParsePathName();

  m_MSFFile = file;

  if(m_Storage && m_MSFFile != m_Storage->GetURL())
  {
    mafNodeIterator *iter = root->NewIterator();
    for(mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      if(mafVMEGenericAbstract *vga = mafVMEGenericAbstract::SafeDownCast(node))
      {
        if (mafDataVector *dv = vga->GetDataVector())
        {
          dv->Modified();
        }
      }
    }
    iter->Delete();
  }

  if(!m_Storage)
  {
    m_Storage = mafVMEStorage::New();
    if(!m_Storage)
      return false;
    m_Storage->SetListener(this);
    m_Storage->SetManager(m_NodeManager);
  }
  m_Storage->SetURL(m_MSFFile);
  Save();
  // add the msf (or zmsf) to the history
  if (!m_ZipFile.IsEmpty())
  {
    mafZIPSave(m_ZipFile, m_TmpDir);
    m_FileHistory.AddFileToHistory(m_ZipFile.toWx()); // add the zmsf to the file history
  }
  else
  {
    m_FileHistory.AddFileToHistory(m_MSFFile.toWx()); // add the msf to the file history
  }
  return true;
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnQuit()
//----------------------------------------------------------------------------
{
  if (m_OpManager && m_OpManager->Running())
  {
    return;
  }

  if(m_ApplicationLayoutSettings->GetModifiedLayouts())
  {
    int answer = wxMessageBox(_("would you like to save your layout list ?"),_("Confirm"),wxYES_NO|wxCANCEL|wxICON_QUESTION , m_Win);
    if(answer == wxCANCEL) 
      return;
    else if(answer == wxYES) 
      m_ApplicationLayoutSettings->SaveApplicationLayout();
  }

  if(!OnFileClose())
    return;
  /*int answer = wxMessageBox(_("quit program ?"), _("Confirm"), wxYES_NO | wxICON_QUESTION , m_Win);
  if(answer != wxYES) 
  return;*/

  cppDEL(m_RemoteLogic);
  cppDEL(m_NodeManager);
  cppDEL(m_MaterialChooser);
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  SetGlobalMouse(NULL);
  cppDEL(m_InteractionManager);
#endif
  cppDEL(m_ViewManager);
  cppDEL(m_OpManager);

  // must be deleted after m_NodeManager
  cppDEL(m_SideBar);

  mafLogicWithGUI::OnQuit();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeDoubleClicked(mafEvent &e)
//----------------------------------------------------------------------------
{
  mafNode *node = e.GetVme();
  if (node)
  {
    mafLogMessage(_M(_R("Double click on ") + node->GetName()));
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeSelect(mafEvent& e)	//modified by Paolo 10-9-2003
//----------------------------------------------------------------------------
{
  mafNode *node = NULL;

	if(m_PlugSidebar && (e.GetSender() == this->m_SideBar->GetTree()))
    node = (mafNode*)e.GetArg();//sender == tree => the node is in e.arg
  else
    node = e.GetVme();          //sender == PER  => the node is in e.node  

  if(node == NULL)
  {
    //node can be selected by its ID
    if(m_NodeManager)
    {
		  long vme_id = e.GetArg();
		  mafVMERoot *root = mafVMERoot::SafeDownCast(m_NodeManager->GetRoot());
		  if (root)
		  {
			  node = root->FindInTreeById(vme_id);
			  e.SetVme(node);
      }
    }
  }

  if(node && m_OpManager && node != m_OpManager->GetSelectedVme()) 
  {
    mafOpSelect opsel;
    opsel.SetNewSel(node);
    m_OpManager->OpExec(&opsel);

    //OnEvent(&mafEvent(this,VME_SELECTED,node));
    mafLogMessage(_M(_R("node selected: ") + node->GetName()));
  }

  if(m_RemoteLogic && (e.GetSender() != m_RemoteLogic) && m_RemoteLogic->IsSocketConnected())
  {
    m_RemoteLogic->VmeSelected(node);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeSelected(mafNode *vme, bool remote)
//----------------------------------------------------------------------------
{
  if(m_ViewManager) m_ViewManager->VmeSelect(vme);
  if(m_OpManager)   {m_OpManager->VmeSelected(vme);    EnableOperations(true);}
	if(m_SideBar)     m_SideBar->VmeSelected(vme);
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if (m_InteractionManager)
    m_InteractionManager->VmeSelected(vme);
#endif

  if(remote && m_RemoteLogic && m_RemoteLogic->IsSocketConnected())
  {
    m_RemoteLogic->VmeSelected(vme);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeShow(mafNode *vme, bool visibility)
//----------------------------------------------------------------------------
{
	if(m_ViewManager) m_ViewManager->VmeShow(vme, visibility);
  bool vme_in_tree = vme->IsVisible(); //check VisibleToTraverse flag.
  if(m_SideBar && vme_in_tree)
    m_SideBar->VmeShow(vme,visibility);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeModified(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_PlugTimebar) UpdateTimeBounds();
  bool vme_in_tree = vme->IsVisible();
  if(m_SideBar && vme_in_tree)
    m_SideBar->VmeModified(vme);
	if(m_NodeManager) m_NodeManager->MSFModified(true);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_NodeManager)
    m_NodeManager->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeAdded(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_NodeManager)
  {
    if (mafVMERoot *root = mafVMERoot::SafeDownCast(m_NodeManager->GetRoot()))
    {
      if (mafVME *vmenode = mafVME::SafeDownCast(vme))
      {
        // Update the new VME added to the tree with the current time-stamp
        // present in the tree.
        vmenode->SetTimeStamp(root->GetTimeStamp());
      }
    }
  }
  if(m_ViewManager)
    m_ViewManager->VmeAdd(vme);
  bool vme_in_tree = true;
  vme_in_tree = !vme->GetTagArray()->GetTag(_R("VISIBLE_IN_THE_TREE")) || 
    (vme->GetTagArray()->GetTag(_R("VISIBLE_IN_THE_TREE")) && vme->GetTagArray()->GetTag(_R("VISIBLE_IN_THE_TREE"))->GetValueAsDouble() != 0);
  if(m_SideBar && vme_in_tree)
    m_SideBar->VmeAdd(vme);
  if(m_PlugTimebar)
    UpdateTimeBounds();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::RestoreLayout()
//----------------------------------------------------------------------------
{
  // Retrieve the saved layout.
  mafNode *vme = m_NodeManager->GetRoot();
  mmaApplicationLayout *app_layout = mmaApplicationLayout::SafeDownCast(vme->GetAttribute(_R("ApplicationLayout")));
  if (app_layout)
  {
    int answer = wxMessageBox(_("Do you want to load the layout?"), _("Warning"), wxYES_NO);
    if (answer == wxNO)
    {
      return;
    }
    
    m_ApplicationLayoutSettings->SetVisibilityVME(true);
    m_ApplicationLayoutSettings->ApplyTreeLayout();
    m_ApplicationLayoutSettings->SetVisibilityVME(false);
  }
}


void mafLogicWithManagers::VmeExpand(mafNode *vme)
{
  m_SideBar->VmeExpand(vme);
}
void mafLogicWithManagers::VmeCollapse(mafNode *vme)
{
  m_SideBar->VmeCollapse(vme);
}
void mafLogicWithManagers::VmeExpandSubTree(mafNode *vme)
{
  m_SideBar->VmeExpandSubTree(vme);
}
void mafLogicWithManagers::VmeCollapseSubTree(mafNode *vme)
{
  m_SideBar->VmeCollapseSubTree(vme);
}
void mafLogicWithManagers::VmeExpandVisible(mafNode *vme)
{
  m_SideBar->VmeExpandVisible(vme);
}


//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_NodeManager)
    m_NodeManager->VmeRemove(vme);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeRemoving(mafNode *vme)
//----------------------------------------------------------------------------
{
  bool vme_in_tree = true;
  vme_in_tree = !vme->GetTagArray()->GetTag(_R("VISIBLE_IN_THE_TREE")) || 
    (vme->GetTagArray()->GetTag(_R("VISIBLE_IN_THE_TREE")) && vme->GetTagArray()->GetTag(_R("VISIBLE_IN_THE_TREE"))->GetValueAsDouble() != 0);
  if(m_SideBar && vme_in_tree)
    m_SideBar->VmeRemove(vme);
	if(m_ViewManager)
    m_ViewManager->VmeRemove(vme);
  if(m_PlugTimebar)
    UpdateTimeBounds();
  if (m_ViewManager)
    m_ViewManager->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpRunStarting()
//----------------------------------------------------------------------------
{
  EnableMenuAndToolbar(false);
  EnableOperations(false);
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if(m_InteractionManager) m_InteractionManager->EnableSelect(false);
#endif
  if(m_SideBar)    m_SideBar->EnableSelect(false);
  EnableItem(MENU_EDIT_FIND_VME, false);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpRunTerminated()
//----------------------------------------------------------------------------
{
  EnableMenuAndToolbar(true);
  EnableOperations(true);
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if(m_InteractionManager) 
    m_InteractionManager->EnableSelect(true);
#endif
  if(m_SideBar)
    m_SideBar->EnableSelect(true);
  EnableItem(MENU_EDIT_FIND_VME, true);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::EnableMenuAndToolbar(bool enable)
//----------------------------------------------------------------------------
{
  EnableItem(MENU_FILE_NEW,enable);
  EnableItem(MENU_FILE_OPEN,enable);
  EnableItem(MENU_FILE_SAVE,enable);
  EnableItem(MENU_FILE_SAVEAS,enable);
  EnableItem(MENU_FILE_MERGE,enable);
  EnableItem(MENU_FILE_QUIT,enable);
  EnableItem(wxID_FILE1,enable);
  EnableItem(wxID_FILE2,enable);
  EnableItem(wxID_FILE3,enable);
  EnableItem(wxID_FILE4,enable);
  EnableItem(wxID_FILE5,enable);
  EnableItem(wxID_FILE6,enable);
  EnableItem(wxID_FILE7,enable);
  EnableItem(wxID_FILE8,enable);
  EnableItem(wxID_FILE9,enable);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpShowGui(bool push_gui, mafGUIPanel *panel)
//----------------------------------------------------------------------------
{
	if(m_SideBar) m_SideBar->OpShowGui(push_gui, panel);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OpHideGui(bool view_closed)
//----------------------------------------------------------------------------
{
	if(m_SideBar) m_SideBar->OpHideGui(view_closed);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewCreate(int viewId)
//----------------------------------------------------------------------------
{
	if(m_ViewManager)
  {
    mafView* v = m_ViewManager->ViewCreate(viewId);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewSelect()
//----------------------------------------------------------------------------
{
  if(m_ViewManager) 
  {
    mafView *view = m_ViewManager->GetSelectedView();
    if(m_SideBar)	m_SideBar->ViewSelect(view);

    EnableItem(CAMERA_RESET, view!=NULL);
    EnableItem(CAMERA_FIT,   view!=NULL);
    EnableItem(CAMERA_FLYTO, view!=NULL);

    EnableItem(MENU_FILE_PRINT, view != NULL);
    EnableItem(MENU_FILE_PRINT_PREVIEW, view != NULL);
    EnableItem(MENU_FILE_PRINT_SETUP, view != NULL);
    EnableItem(MENU_FILE_PRINT_PAGE_SETUP, view != NULL);

// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if (m_InteractionManager)
    {
      m_InteractionManager->ViewSelected(view);
    }
#endif

    if(m_OpManager && !m_OpManager->Running()) 
    {
      // needed to update all the operations that will be enabled on View Creation
      m_OpManager->VmeSelected(m_OpManager->GetSelectedVme());
    }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewCreated(mafView *v)
//----------------------------------------------------------------------------
{
  // removed temporarily support for external Views
  if(v) 
  {
    if(m_RemoteLogic && m_RemoteLogic->IsSocketConnected() && !m_ViewManager->m_FromRemote)
    {
      mafEvent ev(this,VIEW_CREATE,v);
      ev.SetChannel(REMOTE_COMMAND_CHANNEL);
      m_RemoteLogic->OnEvent(&ev);
    }

    if (GetExternalViewFlag())
    {
      // external views
      mafGUIViewFrame *extern_view = new mafGUIViewFrame(m_Win, -1, v->GetLabel(), wxPoint(10,10),wxSize(800,600)/*, wxSIMPLE_BORDER|wxMAXIMIZE*/);
      extern_view->SetView(v);
      extern_view->SetListener(m_ViewManager);
      v->GetFrame()->SetWindowStyleFlag(m_ChildFrameStyle);
      v->SetListener(extern_view);
      v->SetFrame(extern_view);
      extern_view->Refresh();
    }
    else
    {
      // child views
      mafGUIMDIChild *c = new mafGUIMDIChild(m_Win,v);
      c->SetWindowStyleFlag(m_ChildFrameStyle);
      c->SetListener(m_ViewManager);
      v->SetFrame(c);
    }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::TimeSet(double t)
//----------------------------------------------------------------------------
{
  if(m_NodeManager)
  {
    if(mafVMERoot *root = mafVMERoot::SafeDownCast(m_NodeManager->GetRoot()))
      root->SetTreeTime(t);
  }
  if(m_ViewManager)
  {
    m_ViewManager->CameraUpdate(m_TimeBarSettings->GetPlayingInActiveViewport() != 0);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::UpdateTimeBounds()
//----------------------------------------------------------------------------
{
  double min, max; 
  if(m_NodeManager)
  {
    mafTimeStamp b[2] = {0, 0};
    if(mafVMERoot *root = mafVMERoot::SafeDownCast(m_NodeManager->GetRoot()))
      root->GetOutput()->GetTimeBounds(b);
    min = b[0];
    max = b[1];
  }
  if(m_TimePanel)
  {
    m_TimePanel->SetBounds(min,max);
    m_Win->ShowDockPane("timebar", min<max);
  }
}
//----------------------------------------------------------------------------
std::vector<mafNode*> mafLogicWithManagers::VmeChoose(long vme_accept_function, long style, mafString title, bool multiSelect)
//----------------------------------------------------------------------------
{
  mafGUIVMEChooser vc(m_SideBar->GetTree(),title, vme_accept_function, style, multiSelect);
  return vc.ShowChooserDialog();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeChooseMaterial(mafVME *vme, bool updateProperty)
//----------------------------------------------------------------------------
{
  if (m_MaterialChooser == NULL)
  {
    m_MaterialChooser = new mafGUIMaterialChooser();
  }
  if(m_MaterialChooser->ShowChooserDialog(vme))
  {
    this->m_ViewManager->PropertyUpdate(updateProperty);
    this->m_ViewManager->CameraUpdate();
    this->m_NodeManager->MSFModified(true);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeUpdateProperties(mafVME *vme, bool updatePropertyFromTag)
//----------------------------------------------------------------------------
{
  this->m_ViewManager->PropertyUpdate(updatePropertyFromTag);
  this->m_ViewManager->CameraUpdate();
  this->m_NodeManager->MSFModified(true);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::FindVME()
//----------------------------------------------------------------------------
{
  mafGUICheckTree *tree = m_SideBar->GetTree();
  mafGUIDialogFindVme fd(_L("Find VME"));
  fd.SetTree(tree);
  fd.ShowModal();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ViewContextualMenu(bool vme_menu)
//----------------------------------------------------------------------------
{
  mafGUIContextualMenu *contextMenu = new mafGUIContextualMenu();
  contextMenu->SetListener(this);
  mafView *v = m_ViewManager->GetSelectedView();
  mafGUIMDIChild *c = (mafGUIMDIChild *)m_Win->GetActiveChild();
  if(c != NULL)
    contextMenu->ShowContextualMenu(c,v,vme_menu);
  cppDEL(contextMenu);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::TreeContextualMenu(mafEvent &e)
//----------------------------------------------------------------------------
{
  mafGUITreeContextualMenu *contextMenu = new mafGUITreeContextualMenu();
  contextMenu->SetListener(m_ApplicationLayoutSettings);
  mafView *v = m_ViewManager->GetSelectedView();
  mafVME  *vme = (mafVME *)e.GetVme();
  bool vme_menu = e.GetBool();
  bool autosort = e.GetArg() != 0;
  contextMenu->CreateContextualMenu((mafGUICheckTree *)e.GetSender(),v,vme,vme_menu);
  contextMenu->ShowContextualMenu();
  cppDEL(contextMenu);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::HandleException()
//----------------------------------------------------------------------------
{
  int answare = wxMessageBox(_("Do you want to try to save the unsaved work ?"), _("Fatal Exception!!"), wxYES_NO|wxCENTER);
  if(answare == wxYES)
  {
    OnFileSaveAs();
    m_OpManager->StopCurrentOperation();
  }
  OnQuit();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::SetExternalViewFlag(bool external)
//----------------------------------------------------------------------------
{
  m_ExternalViewFlag = external;
  wxConfig *config = new wxConfig(wxEmptyString);
  config->Write("ExternalViewFlag",m_ExternalViewFlag);
  cppDEL(config);
}
//----------------------------------------------------------------------------
bool mafLogicWithManagers::GetExternalViewFlag()
//----------------------------------------------------------------------------
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->Read("ExternalViewFlag", &m_ExternalViewFlag, false);
  cppDEL(config);
  return m_ExternalViewFlag;
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::UpdateMeasureUnit()
//----------------------------------------------------------------------------
{
  const std::list<mafView *>& v = m_ViewManager->GetList();
  for(std::list<mafView*>::const_iterator it = v.begin(); it != v.end(); ++it)
    (*it)->OptionsUpdate();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ImportExternalFile(mafString &filename)
//----------------------------------------------------------------------------
{
  mafString path, name, ext;
  mafSplitPath(filename,&path,&name,&ext);
  ext.MakeLower();
  if (ext == _R("vtk"))
  {
    mafOpImporterVTK *vtkImporter = new mafOpImporterVTK(_R("importer"));
    vtkImporter->SetInput(m_NodeManager->GetRoot());
    vtkImporter->SetListener(m_OpManager);
    vtkImporter->SetFileName(filename);
    vtkImporter->ImportVTK();
    vtkImporter->OpDo();
    cppDEL(vtkImporter);
  }
  else if (ext == _R("stl"))
  {
    mafOpImporterSTL *stlImporter = new mafOpImporterSTL(_R("importer"));
    stlImporter->SetInput(m_NodeManager->GetRoot());
    stlImporter->SetListener(m_OpManager);
    stlImporter->SetFileName(filename.GetCStr());
    stlImporter->ImportSTL();
    stlImporter->OpDo();
    cppDEL(stlImporter);
  }
  else
    mafWarningMessage(_M("Can not import this type of file!"));
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::SetRevision(mafString revision)
//----------------------------------------------------------------------------
{
	m_Revision=revision;
}
