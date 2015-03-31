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
  
  m_VMEManager  = NULL;
  m_ViewManager = NULL;
  m_OpManager   = NULL;
  m_InteractionManager = NULL;
  m_RemoteLogic = NULL;

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

  m_Revision = _("0.1");

  m_Extension = "msf";

  m_User = new mafUser();

}
//----------------------------------------------------------------------------
mafLogicWithManagers::~mafLogicWithManagers()
//----------------------------------------------------------------------------
{
  // Managers are destruct in the OnClose
  cppDEL(m_User);
  cppDEL(m_ApplicationLayoutSettings);
  cppDEL(m_HelpSettings);
  cppDEL(m_PrintSupport);
  cppDEL(m_SettingsDialog); 
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
    m_Win->AddDockPane(m_SideBar->m_Notebook , wxPaneInfo()
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
    m_VMEManager = new mafVMEManager();
    m_VMEManager->SetHost(m_StorageSettings->GetRemoteHostName());
    m_VMEManager->SetRemotePort(m_StorageSettings->GetRemotePort());
    m_VMEManager->SetUser(m_StorageSettings->GetUserName());
    m_VMEManager->SetPassword(m_StorageSettings->GetPassword());
    m_VMEManager->SetLocalCacheFolder(m_StorageSettings->GetCacheFolder());
    m_VMEManager->SetListener(this); 
	m_VMEManager->SetFileExtension(m_Extension);
    //m_VMEManager->SetSingleBinaryFile(m_StorageSettings->GetSingleFileStatus()!= 0);
  }

// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if (m_UseInteractionManager)
  {
    m_InteractionManager = new mafInteractionManager();
    m_InteractionManager->SetListener(this);
    mafPlugDevice<mmdRemoteFileManager>("mmdRemoteFileManager");

    m_Mouse = m_InteractionManager->GetMouseDevice();
    //SIL m_InteractionManager->GetClientDevice()->AddObserver(this, MCH_INPUT);
  }
#endif

  if(m_UseViewManager)
  {
    m_ViewManager = new mafViewManager();
    m_ViewManager->SetListener(this);
    m_ViewManager->SetMouse(m_Mouse);
  }

  if(m_UseOpManager)
  {
    m_OpManager = new mafOpManager();
    m_OpManager->SetListener(this);
    m_OpManager->SetMouse(m_Mouse);
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
      wxString s = wxString::Format("%s",view->GetLabel().c_str());
      mafID command = GetNewMenuId();
      m_ViewListMenu->Append(command, s, (wxMenu *)NULL, s );
      m_MenuElems.push_back(mafMenuElems(false, id, command));
    }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::Plug(mafOp *op, wxString menuPath, bool canUndo, mafGUISettings *setting)
//----------------------------------------------------------------------------
{
  if(m_OpManager) 
  {
    wxString fullLabel = op->m_Label;
    wxString st = op->m_Label.c_str();
    wxString menu_codes=wxStripMenuCodes(st);
    op->m_Label = menu_codes.c_str();
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
  if(m_VMEManager && m_RecentFileMenu)
  {
    m_VMEManager->SetFileHistoryMenu(m_RecentFileMenu);
  }

  mafLogicWithGUI::Show();
  EnableOperations(true);

  // must be after the mafLogicWithGUI::Show(); because in that method is set the m_AppTitle var
  SetApplicationStamp(m_AppTitle);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::SetApplicationStamp(mafString &app_stamp)
//----------------------------------------------------------------------------
{
  if (m_VMEManager)
  {
    m_VMEManager->SetApplicationStamp(app_stamp);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::SetApplicationStamp(std::vector<mafString> app_stamp)
//----------------------------------------------------------------------------
{
  if (m_VMEManager)
  {
    m_VMEManager->SetApplicationStamp(app_stamp);
  }
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
  if(m_VMEManager)
  {
    if(argc > 1 )
	  {
		  wxString file = argv[1];
		  if(wxFileExists(file))
		  {
			  m_VMEManager->MSFOpen(file);
			  UpdateFrameTitle();
		  }
      else
      {
        m_VMEManager->MSFNew();
      }
	  }
    else
    {
      m_VMEManager->MSFNew();
    }
  }
  if (m_OpManager)
  {
    m_OpManager->FillSettingDialog(m_SettingsDialog);

    if(argc > 1 )
    {
      mafString op_type = argv[1];
      mafString op_param = argv[2];
      for (int p = 3; p < argc; p++)
      {
        op_param += " ";
        op_param += argv[p];
      }
      m_OpManager->OpRun(op_type, (void *)op_param.GetCStr());
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
  AddToMenu(_("Undo  \tCtrl+Z"),      MENU_USER_START + 0,m_EditMenu);
  AddToMenu(_("Redo  \tCtrl+Shift+Z"),MENU_USER_START + 1,m_EditMenu);
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
  m_ToolBar->AddTool(MENU_FILE_NEW,mafPictureFactory::GetPictureFactory()->GetBmp("FILE_NEW"),    _("new " + m_Extension + " storage file"));
  m_ToolBar->AddTool(MENU_FILE_OPEN,mafPictureFactory::GetPictureFactory()->GetBmp("FILE_OPEN"),  _("open " + m_Extension + " storage file"));
  m_ToolBar->AddTool(MENU_FILE_SAVE,mafPictureFactory::GetPictureFactory()->GetBmp("FILE_SAVE"),  _("save current " + m_Extension + " storage file"));
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(MENU_FILE_PRINT,mafPictureFactory::GetPictureFactory()->GetBmp("PRINT"),  _("print the selected view"));
  m_ToolBar->AddTool(MENU_FILE_PRINT_PREVIEW,mafPictureFactory::GetPictureFactory()->GetBmp("PRINT_PREVIEW"),  _("show the print preview for the selected view"));
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(MENU_USER_START + 0, mafPictureFactory::GetPictureFactory()->GetBmp("OP_UNDO"),  _("undo (ctrl+z)"));
  m_ToolBar->AddTool(MENU_USER_START + 1, mafPictureFactory::GetPictureFactory()->GetBmp("OP_REDO"),  _("redo (ctrl+shift+z)"));
  m_ToolBar->AddSeparator();

  m_ToolBar->AddTool(MENU_USER_START + 3, mafPictureFactory::GetPictureFactory()->GetBmp("OP_CUT"),  _("cut selected vme (ctrl+x)"));
  m_ToolBar->AddTool(MENU_USER_START + 4, mafPictureFactory::GetPictureFactory()->GetBmp("OP_COPY"), _("copy selected vme (ctrl+c)"));
  m_ToolBar->AddTool(MENU_USER_START + 5, mafPictureFactory::GetPictureFactory()->GetBmp("OP_PASTE"),_("paste vme (ctrl+v)"));
  m_ToolBar->AddSeparator();
  m_ToolBar->AddTool(CAMERA_RESET,mafPictureFactory::GetPictureFactory()->GetBmp("ZOOM_ALL"),_("reset camera to fit all (ctrl+f)"));
  m_ToolBar->AddTool(CAMERA_FIT,  mafPictureFactory::GetPictureFactory()->GetBmp("ZOOM_SEL"),_("reset camera to fit selected object (ctrl+shift+f)"));
  m_ToolBar->AddTool(CAMERA_FLYTO,mafPictureFactory::GetPictureFactory()->GetBmp("FLYTO"),_("fly to object under mouse"));
  m_ToolBar->Realize();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::UpdateFrameTitle()
//----------------------------------------------------------------------------
{
  wxString title(m_AppTitle);
  title += "   " + m_VMEManager->GetFileName();
  m_Win->SetTitle(title);
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
  /*if (mafDataVector::GetSingleFileDataId() == eventId)
  {
    e->SetBool(m_StorageSettings->GetSingleFileStatus()!= 0);
    return;
  }*/
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
      OnFileOpen((*filename).GetCStr());
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
    OnFileHistory(e->GetId());
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
    VmeSelected(e->GetVme());
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
      std::vector<mafNode*> nodeVector = VmeChoose(e->GetArg(), REPRESENTATION_AS_TREE, "Choose Node", e->GetBool());
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
    opId = m_MenuBar->GetMenu(menuId)->FindItem(s->GetCStr());
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
      m_InteractionManager->BindAction(*action_name,interactor);
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
      m_InteractionManager->UnBindAction(*action_name,interactor);
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
  if(CREATE_STORAGE == eventId)
  {
    CreateStorage(e);
    return;
  }
  if(COLLABORATE_ENABLE == eventId)
  {
    bool collaborate = e->GetBool();
    if (collaborate)
    {
      m_RemoteLogic->SetRemoteMouse(m_InteractionManager->GetRemoteMouseDevice());
      m_Mouse->AddObserver(m_RemoteLogic, REMOTE_COMMAND_CHANNEL);
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
        m_Mouse->RemoveObserver(m_RemoteLogic);
      }
    }
    m_ViewManager->Collaborate(collaborate);
    m_OpManager->Collaborate(collaborate);
    m_Mouse->Collaborate(collaborate);
    return;
  }
  if(ABOUT_APPLICATION == eventId)
  {
    wxString message = m_AppTitle.GetCStr();
    message += _(" Application ");
    message += m_Revision;
    wxMessageBox(message, "About Application");
    return;
  }
  if(HELP_HOME == eventId)
  {
    if (m_HelpSettings)
    {
      m_HelpSettings->OpenHelpPage("HELP_HOME");
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
    wxString entity = e->GetString()->GetCStr();
    m_HelpSettings->OpenHelpPage(entity);
  }
  mafLogicWithGUI::OnEvent(maf_event);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileNew()
//----------------------------------------------------------------------------
{
	if(m_VMEManager)
  {
    if(m_VMEManager->AskConfirmAndSave())
	  {
		  m_VMEManager->MSFNew();
	  }
  }
	m_Win->SetTitle(wxString(m_AppTitle.GetCStr()));
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
void mafLogicWithManagers::OnFileOpen(const char *file_to_open)
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
  {
	  if(m_VMEManager->AskConfirmAndSave())
	  {
      mafString file;
      if (m_StorageSettings->GetStorageType() == mafGUISettingsStorage::HTTP)
      {
        if (file_to_open != NULL)
        {
          file = file_to_open;
        }
        else
        {
          mafGUIDialogRemoteFile remoteFile;
          remoteFile.ShowModal();
          file = remoteFile.GetFile().GetCStr();
          mafString protocol;
          if (IsRemote(file,protocol))
          {
            m_VMEManager->SetHost(remoteFile.GetHost());
            m_VMEManager->SetRemotePort(remoteFile.GetPort());//
            m_VMEManager->SetUser(remoteFile.GetUser());
            m_VMEManager->SetPassword(remoteFile.GetPassword());
          }
        }
      }
      else      
      {
		    mafString wildc = "MAF Storage Format file (*.";
		    wildc += m_Extension;
		    wildc += ")|*.";
		    wildc += m_Extension;
			wildc += "|Compressed file (*.z" + m_Extension + ")|*.z" + m_Extension + "";
		    //mafString wildc    = _("MAF Storage Format file (*.msf)|*.msf|Compressed file (*.zmsf)|*.zmsf");
        if (file_to_open != NULL)
        {
          file = file_to_open;
        }
        else
        {
          file = mafGetOpenFile("", wildc);
        }
      }

      if(file.IsEmpty())
        return;

		  m_VMEManager->MSFOpen(file);
	  }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileHistory(int menuId)
//----------------------------------------------------------------------------
{
	if(m_VMEManager) 
  {
    if(m_VMEManager->AskConfirmAndSave())
    {
      m_VMEManager->MSFOpen(menuId);
      UpdateFrameTitle();
    }
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileSave()
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
  {
	mafString save_default_folder = m_StorageSettings->GetDefaultSaveFolder();
	save_default_folder.ParsePathName();
	m_VMEManager->SetDirName(save_default_folder);
    m_VMEManager->MSFSave();
    UpdateFrameTitle();
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::OnFileSaveAs()
//----------------------------------------------------------------------------
{
  if(m_VMEManager) 
  {
	mafString save_default_folder = m_StorageSettings->GetDefaultSaveFolder();
	save_default_folder.ParsePathName();
	m_VMEManager->SetDirName(save_default_folder);
    m_VMEManager->MSFSaveAs();
    UpdateFrameTitle();
  }
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
		int answer = wxMessageBox
			(
			_("would you like to save your layout list ?"),
			_("Confirm"), 
			wxYES_NO|wxCANCEL|wxICON_QUESTION , m_Win
			);
		if(answer == wxYES) 
			m_ApplicationLayoutSettings->SaveApplicationLayout();
	}

  if(m_VMEManager)
  {
    bool quitting = false;
    if (m_VMEManager->MSFIsModified())
    {
      int answer = wxMessageBox
          (
          _("would you like to save your work before quitting ?"),
          _("Confirm"), 
          wxYES_NO|wxCANCEL|wxICON_QUESTION , m_Win
          );
      if(answer == wxYES) 
        m_VMEManager->MSFSave();
      quitting = answer != wxCANCEL;
    }
    else 
    {
      int answer = wxMessageBox(_("quit program ?"), _("Confirm"), wxYES_NO | wxICON_QUESTION , m_Win);
      quitting = answer == wxYES;
    }
    if(!quitting) 
      return;
  }


  cppDEL(m_RemoteLogic);
  cppDEL(m_VMEManager);
  cppDEL(m_MaterialChooser);
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  cppDEL(m_InteractionManager);
#endif
  cppDEL(m_ViewManager);
  cppDEL(m_OpManager);

  // must be deleted after m_VMEManager
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
    mafLogMessage("Double click on %s", node->GetName());
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
    if(m_VMEManager)
    {
		  long vme_id = e.GetArg();
		  mafVMERoot *root = this->m_VMEManager->GetRoot();
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
    mafLogMessage("node selected: %s", node->GetName());
  }


// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if (m_InteractionManager)
    m_InteractionManager->VmeSelected(node);
#endif

  if(m_RemoteLogic && (e.GetSender() != m_RemoteLogic) && m_RemoteLogic->IsSocketConnected())
  {
    m_RemoteLogic->VmeSelected(node);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeSelected(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_ViewManager) m_ViewManager->VmeSelect(vme);
  if(m_OpManager)   {m_OpManager->VmeSelected(vme);    EnableOperations(true);}
	if(m_SideBar)     m_SideBar->VmeSelected(vme);
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
	if(m_VMEManager) m_VMEManager->MSFModified(true);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
	if(m_VMEManager) 
    m_VMEManager->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeAdded(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_ViewManager)
    m_ViewManager->VmeAdd(vme);
  bool vme_in_tree = true;
  vme_in_tree = !vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") || 
    (vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") && vme->GetTagArray()->GetTag("VISIBLE_IN_THE_TREE")->GetValueAsDouble() != 0);
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
  mafNode *vme = m_VMEManager->GetRoot();
  mmaApplicationLayout *app_layout = mmaApplicationLayout::SafeDownCast(vme->GetAttribute("ApplicationLayout"));
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
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_VMEManager)
    m_VMEManager->VmeRemove(vme);
  if(m_PlugTimebar)
    UpdateTimeBounds();
  if (m_ViewManager)
    m_ViewManager->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeRemoving(mafNode *vme)
//----------------------------------------------------------------------------
{
  bool vme_in_tree = true;
  vme_in_tree = !vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") || 
    (vme->GetTagArray()->IsTagPresent("VISIBLE_IN_THE_TREE") && vme->GetTagArray()->GetTag("VISIBLE_IN_THE_TREE")->GetValueAsDouble() != 0);
  if(m_SideBar && vme_in_tree)
    m_SideBar->VmeRemove(vme);
	if(m_ViewManager)
    m_ViewManager->VmeRemove(vme);
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
  if(m_VMEManager)
  {
    m_VMEManager->TimeSet(t);
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
  if(m_VMEManager)
  {
    m_VMEManager->TimeGetBounds(&min, &max);
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
  mafGUIVMEChooser vc(m_SideBar->GetTree(),title.GetCStr(), vme_accept_function, style, multiSelect);
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
    this->m_VMEManager->MSFModified(true);
  }
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::VmeUpdateProperties(mafVME *vme, bool updatePropertyFromTag)
//----------------------------------------------------------------------------
{
  this->m_ViewManager->PropertyUpdate(updatePropertyFromTag);
  this->m_ViewManager->CameraUpdate();
  this->m_VMEManager->MSFModified(true);
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::FindVME()
//----------------------------------------------------------------------------
{
  mafGUICheckTree *tree = m_SideBar->GetTree();
  mafGUIDialogFindVme fd(_("Find VME"));
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
void mafLogicWithManagers::CreateStorage(mafEvent *e)
//----------------------------------------------------------------------------
{
  if (m_StorageSettings->UseRemoteStorage())
  {
    mafString cache_folder = m_StorageSettings->GetCacheFolder();
    if (!wxDirExists(cache_folder.GetCStr()))
    {
      wxMkdir(cache_folder.GetCStr());
    }
    mafRemoteStorage *storage;
    storage = (mafRemoteStorage *)e->GetMafObject();
    if (storage)
    {
      m_VMEManager->NotifyRemove(storage->GetRoot());
      storage->Delete();
    }
    storage = mafRemoteStorage::New();
    storage->SetTmpFolder(cache_folder.GetCStr());
    
    //set default values for remote connection
    storage->SetHostName(m_StorageSettings->GetRemoteHostName());
    storage->SetRemotePort(m_StorageSettings->GetRemotePort());
    storage->SetUsername(m_StorageSettings->GetUserName());
    storage->SetPassword(m_StorageSettings->GetPassword());
    
    storage->GetRoot()->SetName("root");
    storage->SetListener(m_VMEManager);
    storage->Initialize();
    storage->GetRoot()->Initialize();
    e->SetMafObject(storage);
  }
  else
  {
    mafVMEStorage *storage;
    storage = (mafVMEStorage *)e->GetMafObject();
    if (storage)
    {
      m_VMEManager->NotifyRemove(storage->GetRoot());
      storage->Delete();
    }
    storage = mafVMEStorage::New();
    storage->GetRoot()->SetName("root");
    storage->SetListener(m_VMEManager);
    storage->GetRoot()->Initialize();
    e->SetMafObject(storage);
  }
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
  for(mafView* v = m_ViewManager->GetList(); v; v=v->m_Next) 
    v->OptionsUpdate();
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::ImportExternalFile(mafString &filename)
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
  wxSplitPath(filename.GetCStr(),&path,&name,&ext);
  ext.MakeLower();
  if (ext == "vtk")
  {
    mafOpImporterVTK *vtkImporter = new mafOpImporterVTK("importer");
    vtkImporter->SetInput(m_VMEManager->GetRoot());
    vtkImporter->SetListener(m_OpManager);
    vtkImporter->SetFileName(filename.GetCStr());
    vtkImporter->ImportVTK();
    vtkImporter->OpDo();
    cppDEL(vtkImporter);
  }
  else if (ext == "stl")
  {
    mafOpImporterSTL *stlImporter = new mafOpImporterSTL("importer");
    stlImporter->SetInput(m_VMEManager->GetRoot());
    stlImporter->SetListener(m_OpManager);
    stlImporter->SetFileName(filename.GetCStr());
    stlImporter->ImportSTL();
    stlImporter->OpDo();
    cppDEL(stlImporter);
  }
  else
    wxMessageBox(_("Can not import this type of file!"), _("Warning"));
}
//----------------------------------------------------------------------------
void mafLogicWithManagers::SetRevision(mafString revision)
//----------------------------------------------------------------------------
{
	m_Revision=revision;
}
