#include "mafLogicWithManagers.h"

#include <wx/config.h>
#include <wx/tokenzr.h>

#include "mafView.h"
#include "mafViewCompound.h"
#include "mafWXLog.h"

#include "mafViewManager.h"
#include "mafOp.h"
#include "mafOpManager.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafPrintSupport.h"

#ifdef MAF_USE_VTK
#include "mafVTKLog.h"
#include "vtkTimerLog.h"
#endif
#ifdef MAF_USE_VTK
  #include "mafViewVTK.h"
  
  #include "mafOpImporterVTK.h"
  #include "mafOpImporterSTL.h"
  #include "mafInteractionManager.h"
  #include "ftk/Interaction/InteractionFactory.h"
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

#include "mafGUIDialogFindVme.h"
#include "ftk/Gui/MainFrame.h"
#include "ftk/Gui/ViewFrame.h"
#include "mafGUICheckTree.h"
#include "mafGUITimeBar.h"
#include "mafGUIMaterialChooser.h"
#include "mafGUILocaleSettings.h"
#include "mafGUIMeasureUnitSettings.h"
#include "mafGUIApplicationSettings.h"
#include "mafGUISettingsStorage.h"
#include "mafGUIApplicationLayoutSettings.h"
#include "mafGUISettingsTimeBar.h"
#if MAF_USE_CURL
#include "mafRemoteLogic.h"
#include "mafGUIDialogRemoteFile.h"
#include "mmdRemoteFileManager.h"
#endif
#include "mafGUISettingsDialog.h"
#include  "mafGUISettingsHelp.h"

#ifdef WIN32
  #include "mafDeviceClientMAF.h"
#endif

#include "mmaApplicationLayout.h"

#include "mafEventSender.h"
#include "mafDataVector.h"
#include "mafStorage.h"
#include "mafRemoteStorage.h"
#include "mafOpSelect.h"

#include "mafEventIO.h"
#include "mafNodeIterator.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMERoot.h"
#include <wx/aboutdlg.h>
#include <wx/splash.h>


InnerLogic::InnerLogic(mafBaseEventHandler* listener)
{
  m_LocaleSettings = std::make_unique<mafGUILocaleSettings>(listener);
  m_MeasureUnitSettings = std::make_unique<mafGUIMeasureUnitSettings>(listener);
  m_ApplicationSettings = std::make_unique<mafGUIApplicationSettings>(listener);
  m_StorageSettings = std::make_unique<mafGUISettingsStorage>(listener);
  m_TimeBarSettings = std::make_unique<mafGUISettingsTimeBar>(listener);

  // this is needed to manage events coming from the widget
  // when the user change the unit settings.
  m_MeasureUnitSettings->SetListener(listener);

  m_LogToFile = m_ApplicationSettings->GetLogToFileStatus();
  m_LogAllEvents = m_ApplicationSettings->GetLogVerboseStatus();

  m_PrintSupport = std::make_unique<mafPrintSupport>();

  m_SettingsDialog = std::make_unique<mafGUISettingsDialog>();

  m_Config = wxConfigBase::Get();
}
InnerLogic::~InnerLogic() = default;


mafLogicWithManagers::mafLogicWithManagers() = default;

mafLogicWithManagers::~mafLogicWithManagers() = default;

bool mafLogicWithManagers::Configure()
{
  auto frame = new mafGUIMDIFrame("maf", wxDefaultPosition, wxWindow::FromDIP(wxSize(800, 600), nullptr));

  //m_Win->SetListener(this);
  frame->Bind(wxEVT_CLOSE_WINDOW, [this](const wxCloseEvent& event) {mafEvent evUnq(this, MENU_FILE_QUIT); OnEvent(&evUnq); });
  frame->Bind(wxEVT_MENU, [this](const wxCommandEvent& event) {mafEvent evUnq(this, event.GetId());	OnEvent(&evUnq); }, MENU_START, MENU_END);
  frame->Bind(wxEVT_MENU, [this](const wxCommandEvent& event) {mafEvent evUnq(this, event.GetId());	OnEvent(&evUnq); }, wxID_FILE1, wxID_FILE9);
  frame->Bind(wxEVT_IDLE,
    [frame](const wxIdleEvent& event)
    {
#ifdef __WIN32__
      MEMORYSTATUS ms;
      GlobalMemoryStatus(&ms);
      wxString s;
      s << "free mem " << (ms.dwAvailPhys >> 20) << " MB";
      if (frame->GetStatusBar())
        frame->SetStatusText(s, 5);
      //if (current_free_memory < m_MemoryLimitAlert && !m_UserAlerted)
      {
        //m_UserAlerted = true;
        //int answer = wxMessageBox(_("Program is running with few free memory!! \nFree memory used by UnDo stack?."), _("Warning"), wxYES_NO);
        //if (answer == wxYES)
        {
          // Clear UnDo stack to gain memory.
          //{mafEvent evUnq(this, CLEAR_UNDO_STACK); InvokeEvent(evUnq);}
        }
      }
#endif
    }
  );
  frame->Bind(wxEVT_UPDATE_UI,
    [this](wxUpdateUIEvent& event)
    {
      mafEvent evUnq(this, UPDATE_UI);
      evUnq.SetUIEvent(&event);
      OnEvent(&evUnq);
    }, MENU_START, MENU_END);

	frame->Bind(wxEVT_DROP_FILES,
    [this](const wxDropFilesEvent& event)
    {
      for (int i = 0; i < event.GetNumberOfFiles(); i++)
      {
        mafString file_to_open = mafWxToString(event.GetFiles()[i]);
        mafString path, name, ext;
        mafSplitPath(file_to_open, &path, &name, &ext);
        if (ext == _R("msf") || ext == _R("zmsf"))
        {
          OnFileOpen(file_to_open);
          UpdateFrameTitle();
          return;
        }
        else
        {
          ImportExternalFile(file_to_open);
        }
      }
    }
  );
  //m_Win->Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& event) {mafEvent evUnq(this, MENU_FILE_QUIT); OnEvent(&evUnq); });

  m_logic = std::make_unique<InnerLogic>(this);
  m_logic->m_frame = frame;

  mafString msfDir = mafGetApplicationDirectory();
  ParsePathName(msfDir);
  m_logic->m_StorageData = std::make_unique<mafStorageData>(_R("msf"), true, msfDir);

  //////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////

  m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {OnFileNew(); }, wxID_NEW);
  m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {OnFileOpen(); UpdateFrameTitle(); }, wxID_OPEN);
  m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {OnFileSave(); }, wxID_SAVE);
  m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {OnFileSaveAs(); }, wxID_SAVEAS);
  m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {OnFileHistory(event.GetId() - wxID_FILE1); }, wxID_FILE1, wxID_FILE9);

  m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {if (m_logic->m_ViewManager && m_logic->m_PrintSupport) m_logic->m_PrintSupport->OnPrint(m_logic->m_ViewManager->GetSelectedView()); }, wxID_PRINT);
  m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {if (m_logic->m_ViewManager && m_logic->m_PrintSupport) m_logic->m_PrintSupport->OnPrintPreview(m_logic->m_ViewManager->GetSelectedView()); }, wxID_PREVIEW);
  m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {if (m_logic->m_PrintSupport) m_logic->m_PrintSupport->OnPrintSetup(); }, wxID_PRINT_SETUP);
  m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {if (m_logic->m_PrintSupport) m_logic->m_PrintSupport->OnPageSetup(); }, wxID_PAGE_SETUP);

  //m_logic->m_frame->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {auto sb = m_logic->m_frame->GetStatusBar();  event.Check(sb && sb->IsShown()); }, MENU_VIEW_STATUSBAR_);
  m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {EnableOperations(false); m_logic->m_OpManager->OpUndo(); EnableOperations(true); }, wxID_UNDO);
  m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {EnableOperations(false); m_logic->m_OpManager->OpRedo(); EnableOperations(true); }, wxID_REDO);

	//m_logic->m_frame->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {event.Enable(true); }, wxID_UNDO);
  //m_logic->m_frame->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {event.Enable(true); }, wxID_REDO);

  m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent&) { m_logic->m_SettingsDialog->ShowModal(); }, wxID_PREFERENCES);

	if (m_logic->m_PlugMenu)
	{
    m_logic->m_MenuBar = new wxMenuBar;
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_NEW);
    fileMenu->Append(wxID_OPEN);
    fileMenu->Append(wxID_SAVE);
    fileMenu->Append(wxID_SAVEAS);
    fileMenu->Append(wxID_OPEN, _("&Open   \tCtrl+O"));

		if (m_logic->m_StorageSettings->UseRemoteStorage())
    {
      //file_menu->Append(MENU_FILE_UPLOAD, _("&Upload"));
    }

    fileMenu->AppendSeparator();

		m_logic->m_ImportMenu = new wxMenu;
    fileMenu->Append(0, _("Import"), m_logic->m_ImportMenu);

    m_logic->m_ExportMenu = new wxMenu;
    fileMenu->Append(0, _("Export"), m_logic->m_ExportMenu);

    // Print menu item
    fileMenu->AppendSeparator();

    fileMenu->Append(wxID_PRINT);
    fileMenu->Append(wxID_PREVIEW);
    fileMenu->Append(wxID_PRINT_SETUP, _("Printer Setup"));
    fileMenu->Append(wxID_PAGE_SETUP, _("Page Setup"));
    
    m_logic->m_RecentFileMenu = new wxMenu;
    fileMenu->AppendSeparator();
    fileMenu->Append(0, _("Recent Files"), m_logic->m_RecentFileMenu);

    fileMenu->AppendSeparator();
    fileMenu->Append(MENU_FILE_QUIT, _("&Quit  \tCtrl+Q"));

    m_logic->m_MenuBar->Append(fileMenu, _("&File"));

    m_logic->m_EditMenu = new wxMenu;
    m_logic->m_EditMenu->Append(wxID_UNDO);
    m_logic->m_EditMenu->Append(wxID_REDO);
    m_logic->m_EditMenu->AppendSeparator();
    m_logic->m_EditMenu->Append(MENU_EDIT_FIND_VME, _("Find VME \tCtrl+F"));
    m_logic->m_MenuBar->Append(m_logic->m_EditMenu, _("&Edit"));

    m_logic->m_ViewMenu = new wxMenu;
    m_logic->m_MenuBar->Append(m_logic->m_ViewMenu, _("&View"));

    m_logic->m_OpMenu = new wxMenu;
    m_logic->m_MenuBar->Append(m_logic->m_OpMenu, _("&Operations"));

    wxMenu* option_menu = new wxMenu;
    option_menu->Append(wxID_PREFERENCES);
    m_logic->m_MenuBar->Append(option_menu, _("Tools"));

    wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT);
    help_menu->Append(HELP_HOME, _("Help"));
    m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {wxAboutDialogInfo info; info.SetVersion("0.1"); wxAboutBox(info); }, wxID_ABOUT);

    m_logic->m_MenuBar->Append(help_menu, _("&Help"));
    m_logic->m_frame->SetMenuBar(m_logic->m_MenuBar);
	}

  if (m_logic->m_PlugToolbar)
  {
    //m_ToolBar = new wxToolBar(m_Win,-1,wxPoint(0,0),wxSize(-1,-1),wxHORIZONTAL|wxNO_BORDER|wxTB_FLAT  );
    m_logic->m_ToolBar = new wxToolBar(m_logic->m_frame, MENU_VIEW_TOOLBAR_, wxPoint(0, 0), wxSize(-1, -1), wxTB_FLAT | wxTB_NODIVIDER);
    m_logic->m_ToolBar->SetMargins(0, 0);
    m_logic->m_ToolBar->SetToolSeparation(2);
    m_logic->m_ToolBar->SetToolBitmapSize(wxSize(20, 20));
    m_logic->m_ToolBar->AddTool(wxID_NEW, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("FILE_NEW")), (_L("new ") + m_logic->m_StorageData->m_Extension + _L(" storage file")).toWx());
    m_logic->m_ToolBar->AddTool(wxID_OPEN, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("FILE_OPEN")), (_L("open ") + m_logic->m_StorageData->m_Extension + _L(" storage file")).toWx());
    m_logic->m_ToolBar->AddTool(wxID_SAVE, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("FILE_SAVE")), (_L("save current ") + m_logic->m_StorageData->m_Extension + _L(" storage file")).toWx());
    m_logic->m_ToolBar->AddSeparator();

    m_logic->m_ToolBar->AddTool(wxID_PRINT, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("PRINT")), _("print the selected view"));
    m_logic->m_ToolBar->AddTool(wxID_PREVIEW, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("PRINT_PREVIEW")), _("show the print preview for the selected view"));
    m_logic->m_ToolBar->AddSeparator();

    m_logic->m_ToolBar->AddTool(wxID_UNDO, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("OP_UNDO")), _("undo (ctrl+z)"));
    m_logic->m_ToolBar->AddTool(wxID_REDO, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("OP_REDO")), _("redo (ctrl+shift+z)"));
    m_logic->m_ToolBar->AddSeparator();

    m_logic->m_ToolBar->AddTool(MENU_USER_START + 0, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("OP_DELETE")), _("delete selected vme (ctrl+shift+d)"));
    m_logic->m_ToolBar->AddTool(MENU_USER_START + 1, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("OP_CUT")), _("cut selected vme (ctrl+x)"));
    m_logic->m_ToolBar->AddTool(MENU_USER_START + 2, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("OP_COPY")), _("copy selected vme (ctrl+c)"));
    m_logic->m_ToolBar->AddTool(MENU_USER_START + 3, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("OP_PASTE")), _("paste vme (ctrl+v)"));
    m_logic->m_ToolBar->AddSeparator();
    m_logic->m_ToolBar->AddTool(CAMERA_RESET, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("ZOOM_ALL")), _("reset camera to fit all (ctrl+f)"));
    m_logic->m_ToolBar->AddTool(CAMERA_FIT, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("ZOOM_SEL")), _("reset camera to fit selected object (ctrl+shift+f)"));
    m_logic->m_ToolBar->AddTool(CAMERA_FLYTO, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("FLYTO")), _("fly to object under mouse"));
    m_logic->m_ToolBar->Realize();

  	//m_Win->SetToolBar(m_ToolBar);
    m_logic->m_frame->AddPane(m_logic->m_ToolBar, wxAuiPaneInfo()
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
  if (m_logic->m_PlugTimebar)
  {
    m_logic->m_TimePanel = new mafGUITimeBar(m_logic->m_frame, MENU_VIEW_TIMEBAR_, true);
    m_logic->m_TimePanel->SetListener(this);

    // Events coming from settings are forwarded to the time bar.
    m_logic->m_TimePanel->SetTimeSettings(m_logic->m_TimeBarSettings.get());
    m_logic->m_TimeBarSettings->SetListener(m_logic->m_TimePanel);
    m_logic->m_frame->AddPane(m_logic->m_TimePanel, wxAuiPaneInfo()
      .Name("timebar")
      .Caption(wxT("TimeBar"))
      .Bottom()
      .Row(1)
      .Layer(2)
      .ToolbarPane()
      .LeftDockable(false)
      .RightDockable(false)
      .MinSize(100, 22)
      .Floatable(false)
      .Gripper(false)
      .Resizable(false)
      .Movable(false)
    );
  }
  if (m_logic->m_PlugLogbar)
  {
#ifdef MAF_USE_VTK
    m_logic->m_VtkLog = mafVTKLog::New();
    m_logic->m_VtkLog->SetInstance(m_logic->m_VtkLog);
#endif
    wxTextCtrl* log = new wxTextCtrl(m_logic->m_frame, MENU_VIEW_LOGBAR_, "", wxPoint(0, 0), wxSize(100, 300), /*wxNO_BORDER |*/ wxTE_MULTILINE);
    m_logic->m_Logger = new mafWXLog(log);
    m_logic->m_Logger->LogToFile(m_logic->m_LogToFile);
    if (m_logic->m_LogToFile)
    {
      mafString s = m_logic->m_ApplicationSettings->GetLogFolder();
      wxDateTime log_time = wxDateTime::Now();
      s += _R("\\");
      s += mafWxToString(m_logic->m_frame->GetTitle());
      s += mafString::Format(_R("_%02d_%02d_%d_%02d_%2d"), log_time.GetYear(), log_time.GetMonth() + 1, log_time.GetDay(), log_time.GetHour(), log_time.GetMinute());
      s += _R(".log");
      if (m_logic->m_Logger->SetFileName(s.toWx()) == MAF_ERROR)
      {
        mafLogMessage(_M(_R("Unable to create log file ") + s));
      }
    }
    m_logic->m_Logger->SetVerbose(m_logic->m_LogAllEvents);

    wxLog* old_log = wxLog::SetActiveTarget(m_logic->m_Logger);
    cppDEL(old_log);

    m_logic->m_frame->AddPane(log, wxAuiPaneInfo()
      .Name("logbar")
      .Caption(wxT("LogBar"))
      .Bottom()
      .Layer(0)
      .MinSize(100, 10)
      .TopDockable(false) // prevent docking on top side - otherwise may dock also beside the toolbar -- and it's hugely
    );

    mafLogMessage(_M(mafString(_L("welcome"))));
  }
	else
	{
#ifdef MAF_USE_VTK
    m_logic->m_VtkLog = mafVTKLog::New();
    m_logic->m_VtkLog->SetInstance(m_logic->m_VtkLog);
#endif  
    wxTextCtrl* log = new wxTextCtrl(m_logic->m_frame, -1, "", wxPoint(0, 0), wxSize(100, 300), wxNO_BORDER | wxTE_MULTILINE);
    m_logic->m_Logger = new mafWXLog(log);
    log->Show(false);
    wxLog* old_log = wxLog::SetActiveTarget(m_logic->m_Logger);
    cppDEL(old_log);
  }

  EnableItem(CAMERA_RESET, false);
  EnableItem(CAMERA_FIT, false);
  EnableItem(CAMERA_FLYTO, false);
  EnableItem(wxID_PRINT, false);
  EnableItem(wxID_PREVIEW, false);
  EnableItem(wxID_PRINT_SETUP, false);
  EnableItem(wxID_PAGE_SETUP, false);

  if (m_logic->m_PlugSidebar)
  {
    m_logic->m_SideBar = std::make_unique<mafSideBar>(m_logic->m_frame, MENU_VIEW_SIDEBAR_, this, m_logic->m_SidebarStyle);
    m_logic->m_frame->AddPane(m_logic->m_SideBar->m_Notebook, wxAuiPaneInfo()
      .Name("sidebar")
      .Caption(wxT("ControlBar"))
      .Right()
      .Layer(2)
      .MinSize(240, 450)
      .TopDockable(false)
      .BottomDockable(false)
    );
  }

  {
    m_logic->m_NodeManager = std::make_unique<mafNodeManager>();
    m_logic->m_NodeManager->SetListener(this);
  }

  // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  {
    m_logic->m_InteractionManager = std::make_unique<mafInteractionManager>();
    m_logic->m_InteractionManager->SetListener(this);
#ifdef MAF_USE_CURL
    mafPlugDevice<mmdRemoteFileManager>("mmdRemoteFileManager");
#endif

    SetGlobalMouse(m_logic->m_InteractionManager->GetMouseDevice());
    //SIL m_InteractionManager->GetClientDevice()->AddObserver(this, MCH_INPUT);
  }
#endif

  {
    m_logic->m_ViewManager = std::make_unique<mafViewManager>();
    m_logic->m_ViewManager->SetListener(this);
  }

  {
    m_logic->m_OpManager = std::make_unique<mafOpManager>();
    m_logic->m_OpManager->SetListener(this);
    m_logic->m_OpManager->WarningIfCantUndo(m_logic->m_ApplicationSettings->GetWarnUserFlag());
  }

  // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  {
#ifdef __WIN32__
#ifdef MAF_USE_CURL
    m_RemoteLogic = std::make_unique<mafRemoteLogic>(this, m_ViewManager.get(), m_OpManager.get());

    m_RemoteLogic->SetClientUnit(m_InteractionManager->GetClientDevice());
#endif
#endif

  }
#endif

  // Fill the SettingsDialog
  m_logic->m_SettingsDialog->AddPage(m_logic->m_ApplicationSettings->GetGui(), m_logic->m_ApplicationSettings->GetLabel());
  m_logic->m_SettingsDialog->AddPage(m_logic->m_StorageSettings->GetGui(), m_logic->m_StorageSettings->GetLabel());

  if (m_logic->m_ViewManager)
  {
    m_logic->m_ApplicationLayoutSettings = std::make_unique<mafGUIApplicationLayoutSettings>(this);
    m_logic->m_ApplicationLayoutSettings->SetViewManager(m_logic->m_ViewManager.get());
    m_logic->m_ApplicationLayoutSettings->SetApplicationFrame(m_logic->m_frame);
    m_logic->m_SettingsDialog->AddPage(m_logic->m_ApplicationLayoutSettings->GetGui(), m_logic->m_ApplicationLayoutSettings->GetLabel());
  }

  m_logic->m_SettingsDialog->AddPage(m_logic->m_frame->GetDockSettingGui(), _("User Interface Preferences"));

  m_logic->m_HelpSettings = std::make_unique<mafGUISettingsHelp>(this);
  m_logic->m_SettingsDialog->AddPage(m_logic->m_HelpSettings->GetGui(), m_logic->m_HelpSettings->GetLabel());

  // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if (m_logic->m_InteractionManager)
    m_logic->m_SettingsDialog->AddPage(m_logic->m_InteractionManager->GetGui(), _("Interaction Manager"));
#endif    
  if (m_logic->m_LocaleSettings)
    m_logic->m_SettingsDialog->AddPage(m_logic->m_LocaleSettings->GetGui(), m_logic->m_LocaleSettings->GetLabel());

  if (m_logic->m_MeasureUnitSettings)
    m_logic->m_SettingsDialog->AddPage(m_logic->m_MeasureUnitSettings->GetGui(), m_logic->m_MeasureUnitSettings->GetLabel());

  if (m_logic->m_TimeBarSettings)
    m_logic->m_SettingsDialog->AddPage(m_logic->m_TimeBarSettings->GetGui(), m_logic->m_TimeBarSettings->GetLabel());

  return true;
}
void mafLogicWithManagers::Plug(mafView* view, bool visibleInMenu)
{
  if (m_logic->m_ViewManager)
  {
    long id = m_logic->m_ViewManager->ViewAdd(view);
    if (visibleInMenu)
    {
      if (!m_logic->m_ViewListMenu)
      {
        m_logic->m_ViewListMenu = new wxMenu;
        m_logic->m_ViewMenu->AppendSeparator();
        m_logic->m_ViewMenu->Append(0, _("Add View"), m_logic->m_ViewListMenu);
      }
      wxString s = view->GetLabel().toWx();
      mafID command = GetNewMenuId();
      m_logic->m_ViewListMenu->Append(command, s, (wxMenu*)NULL, s);
      m_logic->m_MenuElems.push_back(mafMenuElems(false, id, command));
    }
  }
}
void mafLogicWithManagers::Plug(mafOp* op, const mafString& menuPath, bool canUndo, mafGUISettings* setting)
{
  if (m_logic->m_OpManager)
  {
    mafString fullLabel = op->GetLabel();
    op->SetLabel(mafStripMenuCodes(fullLabel));
    long id = m_logic->m_OpManager->OpAdd(op/*, canUndo/*, setting*/);
    wxMenu* path_menu = m_logic->m_OpMenu;
    if (op->GetType() == OPTYPE_IMPORTER)
      path_menu = m_logic->m_ImportMenu;
    else if (op->GetType() == OPTYPE_EXPORTER)
      path_menu = m_logic->m_ExportMenu;
    else if (op->GetType() == OPTYPE_EDIT)
      path_menu = m_logic->m_EditMenu;
    mafID command = GetNewMenuId();
    AddToMenu(fullLabel, command, path_menu, menuPath);
    m_logic->m_MenuElems.push_back(mafMenuElems(true, id, command));


    // currently mafInteraction is strictly dependent on VTK
#ifdef MAF_USE_VTK    
    if (m_logic->m_InteractionManager)
    {
      if (const char** actions = op->GetActions())
      {
        const char* action;
        for (int i = 0; action = actions[i]; i++)
        {
          m_logic->m_InteractionManager->AddAction(action);
        }
      }
    }
#endif
  }
}

bool mafLogicWithManagers::AskConfirmAndSave()
{
  if (m_logic->m_NodeManager&& m_logic->m_NodeManager->MSFIsModified()) // check if the msf has been modified
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
  return MENU_USER_START + m_logic->m_UserCommandIndex++;
}

void mafLogicWithManagers::EnableOperations(bool enable)
{
  EnableItem(wxID_UNDO, enable && m_logic->m_OpManager->UndoAvailable());
  EnableItem(wxID_REDO, enable && m_logic->m_OpManager->RedoAvailable());
  for(unsigned i = 0; i < m_logic->m_MenuElems.size(); i++)
  {
    if(m_logic->m_MenuElems[i].m_op)
    {
      bool enableOp = enable;
      auto node = m_logic->m_OpManager->GetSelectedVme();
      enableOp = enableOp && node && m_logic->m_OpManager->GetOperationById(m_logic->m_MenuElems[i].m_id)->Accept(node.get());
      EnableItem(i + MENU_USER_START, enableOp); 
    }
  }
}

mafGUIMDIFrame* mafLogicWithManagers::GetTopWin()
{
	return m_logic->m_frame;
};

bool mafLogicWithManagers::SetAppTag(mafNode *vme)
{
  if(!vme->GetTagArray()->GetTag(_R("APP_STAMP")))
  {
    mafTagItem tag_appstamp;
    tag_appstamp.SetName(_R("APP_STAMP"));
    if(!m_logic->m_AppStamp.empty())
      tag_appstamp.SetValue(m_logic->m_AppStamp.at(0));
    else
      tag_appstamp.SetValue(_R(""));
    vme->GetTagArray()->SetTag(tag_appstamp);
    return true;
  }
  return false;
}


void mafLogicWithManagers::AddCreationDate(mafNode *vme)
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
  for (int k=0; k< m_logic->m_AppStamp.size(); k++)
  {
    // Check with the Application name
    if (app_stamp == m_logic->m_AppStamp.at(k))
    {
      stamp_found = true;
    }
    // Check with the "Data Manager" tag
    if (m_logic->m_AppStamp.at(k) == _R("DataManager"))
    {
      stamp_data_manager_found = true;
    }
    // Check with the "OPEN_ALL_DATA" tag
    if (m_logic->m_AppStamp.at(k) == _R("OPEN_ALL_DATA"))
    {
      stamp_open_all_found = true;
    }
  }
  /*if(app_stamp.Equals("INVALID") || ((!stamp_found) && (!stamp_data_manager_found) && (!stamp_open_all_found)))
    return false;*/
  return true;
}

void mafLogicWithManagers::Show()
{
  if(m_logic->m_NodeManager && m_logic->m_RecentFileMenu)
  {
    m_logic->m_FileHistory.UseMenu(m_logic->m_RecentFileMenu);
    m_logic->m_FileHistory.Load(*m_logic->m_Config); // Loads file history from registry
  }

  wxAcceleratorEntry* entries = new wxAcceleratorEntry[m_logic->m_AccelTable.size()];
  for (int i = 0; i < m_logic->m_AccelTable.size(); i++)
    entries[i] = m_logic->m_AccelTable[i];
  wxAcceleratorTable atable(m_logic->m_AccelTable.size(), entries);
  if (atable.Ok())
    mafGetFrame()->SetAcceleratorTable(atable);
  delete[] entries;
  m_logic->m_frame->Show(TRUE);
  EnableOperations(true);

  // must be after the mafLogicWithGUI::Show(); because in that method is set the m_AppTitle var
  SetApplicationStamp(mafWxToString(wxTheApp->GetAppDisplayName()));
}

void mafLogicWithManagers::ShowSplashScreen()
{
  wxBitmap splashImage = mafPictureFactory::GetPictureFactory()->GetBmp(_R("SPLASH_SCREEN"));
  ShowSplashScreen(splashImage);
}
void mafLogicWithManagers::ShowSplashScreen(wxBitmap& splashImage)
{
  long splash_style = wxSIMPLE_BORDER | wxSTAY_ON_TOP;
  wxSplashScreen* splash = new wxSplashScreen(splashImage,
    wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
    2000, NULL, -1, wxDefaultPosition, wxDefaultSize,
    splash_style);
  mafYield();
}

void mafLogicWithManagers::SetApplicationStamp(const mafString &app_stamp)
{
  // Add a single application stamp; this is done automatically while creating the application with the application name
  m_logic->m_AppStamp.push_back(app_stamp);
}
void mafLogicWithManagers::SetApplicationStamp(const std::vector<mafString>& app_stamp)
{
  // Add a vector of time stamps; this can be done manually for adding compatibility with other applications. 
  // The application name itself must not be included since it was already added with the other call (see function above).
  m_logic->m_AppStamp.insert(m_logic->m_AppStamp.end(), app_stamp.begin(), app_stamp.end());
}
void mafLogicWithManagers::Init(int argc, char **argv)
{
  if(m_logic->m_NodeManager)
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
  if (m_logic->m_OpManager)
  {
    m_logic->m_OpManager->FillSettingDialog(m_logic->m_SettingsDialog.get());

    if(argc > 1 )
    {
      mafString op_type = _R(argv[1]);
      mafString op_param = _R(argv[2]);
      for (int p = 3; p < argc; p++)
      {
        op_param += _R(" ");
        op_param += _R(argv[p]);
      }
      m_logic->m_OpManager->OpRun(op_type, (void *)op_param.toStd().c_str());
    }
  }

  m_logic->m_ApplicationLayoutSettings->LoadLayout(true);
}

void mafLogicWithManagers::UpdateFrameTitle()
{
  mafString title = mafWxToString(wxTheApp->GetAppDisplayName());
  if(!m_logic->m_StorageData->m_MSFFile.empty())
    title += _R("   ") + m_logic->m_StorageData->m_MSFFile;
  m_logic->m_frame->SetTitle(title.toWx());
}
void mafLogicWithManagers::OnEvent(mafEventBase *maf_event)
{
  mafEvent *e = mafEvent::SafeDownCast(maf_event);
  if(!e)
  {
    return;
  }
  mafID eventId = e->GetId();
  for(int i = 0; i < m_logic->m_MenuElems.size(); i++)
  {
    if(eventId != m_logic->m_MenuElems[i].m_command)
      continue;
    if(!m_logic->m_MenuElems[i].m_op)
    {
      if(m_logic->m_ViewManager)
        m_logic->m_ViewManager->ViewCreate(m_logic->m_MenuElems[i].m_id + VIEW_START);
      return;
    }
    if(!m_logic->m_OpManager)
      break;
    EnableOperations(false);
    m_logic->m_OpManager->OpRun(m_logic->m_MenuElems[i].m_id, m_logic->m_OpManager->GetSelectedVme().get());
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
    VmeSelected(e->GetVme()->SharedFromThis(), false);
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
#ifdef MAF_USE_CURL
    if(m_RemoteLogic && (e->GetSender() != m_RemoteLogic.get()) && m_RemoteLogic->IsSocketConnected())
    {
      m_RemoteLogic->VmeShow(e->GetVme(), e->GetBool());
    }
#endif
    return;
  }
  if(VME_MODIFIED == eventId)
  {
    VmeModified(e->GetVme());
    if(!m_logic->m_PlugTimebar && mafVME::StaticDownCast(e->GetVme())->IsAnimated())
      m_logic->m_frame->ShowPane("timebar",!m_logic->m_frame->IsPaneShown("timebar") );
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
    VmeAdded(e->GetVme()->SharedFromThis());
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
    if (mafString *s = e->GetString())
    {
      auto nodeVector = VmeChoose(e->GetArg(), REPRESENTATION_AS_TREE, *s, e->GetBool());
      if (!e->GetBool())
      {
        if (!nodeVector.empty())
        {
          e->SetVme(nodeVector[0]);
        }
        else
        {
          e->SetVme(nullptr);
        }
      }
      else
      {
        e->SetVmeVector(nodeVector);
      }
    }
    else
    {
      auto nodeVector = VmeChoose(e->GetArg(), REPRESENTATION_AS_TREE, _R("Choose Node"), e->GetBool());
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
    VmeChooseMaterial(mafVME::StaticDownCast(e->GetVme()), e->GetBool());
    return;
  }
  if(VME_VISUAL_MODE_CHANGED == eventId)
  {
    auto vme = mafVME::StaticDownCast(e->GetVme());
    VmeShow(vme, false);
    VmeShow(vme, true);
    return;
  }
  if(UPDATE_PROPERTY == eventId)
  {
    VmeUpdateProperties(mafVME::StaticDownCast(e->GetVme()), e->GetBool());
    return;
  }
  if(SHOW_CONTEXTUAL_MENU == eventId)
  {
    if (e->GetSender() == m_logic->m_SideBar->GetTree())
      TreeContextualMenu(*e);
    else
      ViewContextualMenu(e->GetBool());
    return;
  }
  // ###############################################################
  // commands related to OP
  if(MENU_OP == eventId)
  {
    if(m_logic->m_OpManager)
    {
      m_logic->m_OpManager->OpRun(e->GetArg());
#ifdef MAF_USE_CURL
      if(/*m_OpManager->GetRunningOperation() && */m_RemoteLogic && m_RemoteLogic->IsSocketConnected() && !m_OpManager->m_FromRemote)
      {
        mafEvent re(this, mafOpManager::RUN_OPERATION_EVENT, e->GetArg());
        re.SetChannel(REMOTE_COMMAND_CHANNEL);
        m_RemoteLogic->OnEvent(&re);
      }
#endif
    }
    return;
  }
  if(PARSE_STRING == eventId)
  {
    if(m_logic->m_OpManager->Running())
    {
      wxMessageBox("There is an other operation running!!");
      return;
    }
    int menuId, opId;
    mafString *s = e->GetString();
    menuId = m_logic->m_MenuBar->FindMenu(_("Operations"));
    opId = m_logic->m_MenuBar->GetMenu(menuId)->FindItem(s->toWx());
    m_logic->m_OpManager->OpRun(opId);
    return;
  }
  if(MENU_OPTION_APPLICATION_SETTINGS == eventId)
  {
    if (m_logic->m_OpManager)
    {
      m_logic->m_OpManager->WarningIfCantUndo(m_logic->m_ApplicationSettings->GetWarnUserFlag());
      return;
    }
  }
  if(CLEAR_UNDO_STACK == eventId)
  {
    if (!m_logic->m_OpManager->Running())
    {
      m_logic->m_OpManager->ClearUndoStack();
      EnableOperations(true);
    }
    return;
  }
  if(OP_RUN_STARTING == eventId)
  {
    if (mafView* view = m_logic->m_ViewManager->GetSelectedView())
      view->SetAllowCloseWindow(false);
    OpRunStarting();
    return; 
  }
  if(OP_RUN_TERMINATED == eventId)
  {
    if (mafView* view = m_logic->m_ViewManager->GetSelectedView())
      view->SetAllowCloseWindow(true);
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
    m_logic->m_OpManager->StopCurrentOperation();
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
    view= m_logic->m_ViewManager->GetFromList(viewStr);
    if(view) 
    {
      view->GetFrame()->SetSize(e->GetWidth(),e->GetHeight());
      view->GetFrame()->SetPosition(wxPoint(e->GetX(),e->GetY()));
    }
  }
  if(VIEW_DELETE == eventId)
  {

    if(m_logic->m_PlugSidebar)
      m_logic->m_SideBar->ViewDeleted(e->GetView());

#ifdef MAF_USE_VTK
    // currently mafInteraction is strictly dependent on VTK (marco)
    if(m_logic->m_InteractionManager)
      m_logic->m_InteractionManager->ViewSelected(NULL);
#endif

    if (m_logic->m_ViewManager)
    {
      EnableItem(CAMERA_RESET, false);
      EnableItem(CAMERA_FIT,   false);
      EnableItem(CAMERA_FLYTO, false);

      EnableItem(wxID_PRINT, false);
      EnableItem(wxID_PREVIEW, false);
      EnableItem(wxID_PRINT_SETUP, false);
      EnableItem(wxID_PAGE_SETUP, false);
    }
    if (m_logic->m_OpManager)
    {
      EnableOperations(!m_logic->m_OpManager->Running());
    }
    return;
  }
  if(VIEW_SELECT == eventId)
  {
    ViewSelect();
    if (m_logic->m_OpManager)
    {
      if (mafView* view = m_logic->m_ViewManager->GetSelectedView())
        view->SetAllowCloseWindow(!m_logic->m_OpManager->Running());
    }
    return;
  }
  if(VIEW_MAXIMIZE_ == eventId)
  {
#ifdef MAF_USE_CURL
    if (m_RemoteLogic && m_RemoteLogic->IsSocketConnected() && !m_ViewManager->m_FromRemote)
    {
      m_RemoteLogic->RemoteMessage(*e->GetString());
    }
#endif
    return;
  }
  if(VIEW_SELECTED == eventId)
  {
    e->SetBool(m_logic->m_ViewManager->GetSelectedView() != NULL);
    e->SetView(m_logic->m_ViewManager->GetSelectedView());
    return;
  }
  if(VIEW_SAVE_IMAGE == eventId)
  {
    mafViewCompound *v = mafViewCompound::SafeDownCast(m_logic->m_ViewManager->GetSelectedView());
    if (v && e->GetBool())
    {
      v->GetRWI()->SaveAllImages(v->GetLabel(),v, m_logic->m_ApplicationSettings->GetImageTypeId());
    }
    else
    {
      mafView *v = m_logic->m_ViewManager->GetSelectedView();
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
    if(m_logic->m_ViewManager) m_logic->m_ViewManager->CameraReset();
    return; 
  }
  if(CAMERA_FIT == eventId)
  {
    if(m_logic->m_ViewManager) m_logic->m_ViewManager->CameraReset(true);
    return;
  }
  if(CAMERA_FLYTO == eventId)
  {
    if(m_logic->m_ViewManager) m_logic->m_ViewManager->CameraFlyToMode();
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if(m_logic->m_InteractionManager) m_logic->m_InteractionManager->CameraFlyToMode();  //modified by Marco. 15-9-2004 fly to with devices.
#endif
    return;
  }
  if(LINK_CAMERA_TO_INTERACTOR == eventId)
  {
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if (m_logic->m_InteractionManager == NULL || m_logic->m_InteractionManager->GetPER() == NULL)
      return;

    vtkCamera *cam = vtkCamera::SafeDownCast(e->GetVtkObj());
    bool link_camera = e->GetBool();
    if (!link_camera) 
    {
      if (cam) 
        m_logic->m_InteractionManager->GetPER()->LinkCameraRemove(cam);
      else
        m_logic->m_InteractionManager->GetPER()->LinkCameraRemoveAll();

      if (m_logic->m_CameraLinkingObserverFlag)
      {
        m_logic->m_InteractionManager->GetPER()->GetCameraMouseInteractor()->RemoveObserver(this);
        m_logic->m_CameraLinkingObserverFlag = false;
      }
    }
    else if (cam) 
    {
      if (!m_logic->m_CameraLinkingObserverFlag)
      {
        m_logic->m_InteractionManager->GetPER()->GetCameraMouseInteractor()->AddObserver(this);
        m_logic->m_CameraLinkingObserverFlag = true;
      }
      m_logic->m_InteractionManager->GetPER()->LinkCameraAdd(cam);
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
    m_logic->m_SettingsDialog->ShowModal();
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
    if(m_logic->m_InteractionManager)
    {
      vtkRenderer *ren = (vtkRenderer*)e->GetVtkObj();
      //assert(ren);
      m_logic->m_InteractionManager->PreResetCamera(ren);
      //mafLogMessage("CAMERA_PRE_RESET");
    }
#endif
    return;
  }
  if(CAMERA_POST_RESET == eventId)
  {
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if(m_logic->m_InteractionManager)
    {
      vtkRenderer *ren = (vtkRenderer*)e->GetVtkObj();
      //assert(ren); //modified by Marco. 2-11-2004 Commented out to allow reset camera of all cameras.
      m_logic->m_InteractionManager->PostResetCamera(ren);
      //mafLogMessage("CAMERA_POST_RESET");
    }
#endif
    return;
  }
  if(CAMERA_UPDATE == eventId)
  {
    if(m_logic->m_ViewManager) m_logic->m_ViewManager->CameraUpdate();
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
		//PERFORMANCE WARNING : if view manager exist this will cause another camera update!!
		// An else could be added to reduce CAMERA_UPDATE by a 2 factor. This has been done for the HipOp vertical App showing
		// big performance improvement in composite views creation.
    if(m_logic->m_InteractionManager) m_logic->m_InteractionManager->CameraUpdate(e->GetView());
#endif
    return;
  }
  if(CAMERA_SYNCHRONOUS_UPDATE == eventId)     
  {
    m_logic->m_ViewManager->CameraUpdate();
    return;
  }
  // ###############################################################
  // commands related to interaction manager
  if(PER_PUSH == eventId)
  {
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if(m_logic->m_InteractionManager)
    {
      auto per = mafInteractorPER::SafeDownCast(e->GetAgent());
      assert(per);
      m_logic->m_InteractionManager->PushPER(per);
    }
#endif
    return; 
  }
  if(PER_POP == eventId)
  {
    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if(m_logic->m_InteractionManager) m_logic->m_InteractionManager->PopPER();
#endif
    return;
  }
  if(DEVICE_GET == eventId)
  {
    return;
  }
  if(COLLABORATE_ENABLE == eventId)
  {
    bool collaborate = e->GetBool();
#ifdef MAF_USE_CURL
    if (collaborate)
    {
      m_RemoteLogic->SetRemoteMouse(m_InteractionManager->GetRemoteMouseDevice());
      GetGlobalMouse()->AddObserver(m_RemoteLogic.get(), REMOTE_COMMAND_CHANNEL);
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
        GetGlobalMouse()->RemoveObserver(m_RemoteLogic.get());
      }
    }
#endif
    m_logic->m_ViewManager->Collaborate(collaborate);
    m_logic->m_OpManager->Collaborate(collaborate);
    GetGlobalMouse()->Collaborate(collaborate);
    return;
  }
  if(HELP_HOME == eventId)
  {
    if (m_logic->m_HelpSettings)
    {
      m_logic->m_HelpSettings->OpenHelpPage(_R("HELP_HOME"));
    }
  }
  if(GET_BUILD_HELP_GUI == eventId)
  {
    int buildGui = -1;
    if (m_logic->m_HelpSettings == NULL)
    {
      buildGui = false;
      e->SetArg(buildGui);
    }
    else
    {
      buildGui = m_logic->m_HelpSettings->GetBuildHelpGui();
    }
    e->SetArg(buildGui);
  }
  if(OPEN_HELP_PAGE == eventId)
  {
  // open help for entity
    m_logic->m_HelpSettings->OpenHelpPage(*e->GetString());
  }
  if (mafEvent* e = mafEvent::SafeDownCast(maf_event))
  {
    switch (e->GetId())
    {
    case MENU_FILE_QUIT:
      OnQuit();
      break;
      //resize view
        // ###############################################################
        // commands related to the STATUSBAR
    case BIND_TO_PROGRESSBAR:
#ifdef MAF_USE_VTK
      m_logic->m_frame->BindToProgressBar(e->GetVtkObj());
#endif
      break;
    case PROGRESSBAR_SHOW:
      m_logic->m_frame->ProgressBarShow();
      break;
    case PROGRESSBAR_HIDE:
      m_logic->m_frame->ProgressBarHide();
      break;
    case PROGRESSBAR_SET_VALUE:
      m_logic->m_frame->ProgressBarSetVal(e->GetArg());
      break;
    case PROGRESSBAR_SET_TEXT:
    { wxString s = e->GetString()->toWx(); m_logic->m_frame->ProgressBarSetText(s); }
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
bool mafLogicWithManagers::OnFileClose(bool force)
{
  if(!m_logic->m_NodeManager)
    return true;
  if(!force && !AskConfirmAndSave())
    return false;
  {mafEvent evUnq(this,CLEAR_UNDO_STACK); OnEvent(&evUnq);} // ask logic to clear the undo stack
  if(m_logic->m_Storage && !m_logic->m_Storage->m_TmpDir.empty())
  {
    mafRemoveDirectory(m_logic->m_Storage->m_TmpDir); // remove the temporary directory
    m_logic->m_Storage->m_TmpDir.clear();
  }
  m_logic->m_NodeManager->SetRoot(NULL);
  m_logic->m_NodeManager->MSFModified(false);
  m_logic->m_Storage.reset();
  m_logic->m_NodeManager->SetListener(this);
  VmeSelected(NULL);
  m_logic->m_StorageData->m_MSFFile.clear();
  m_logic->m_StorageData->m_ZipFile.clear();
  UpdateFrameTitle();
  return true;
}
void mafLogicWithManagers::OnFileNew()
{
  if(!m_logic->m_NodeManager)
    return;
  if(!OnFileClose())
    return;
  auto root = mafVMERoot::NewSPtr();
  root->SetName(_R("root"));
  root->Initialize();
  //Add the application stamps
  SetAppTag(root.get());
  AddCreationDate(root.get());
  m_logic->m_NodeManager->SetRoot(root);
  VmeSelected(root);
  root->SetTreeTime(0.0); // set the tree time
  UpdateFrameTitle();
  m_logic->m_NodeManager->MSFModified(false);
}
bool mafLogicWithManagers::OnFileOpen(const mafString& file_to_open)
{
  //!!remember upgrade of data to current VERSION!!!!!!!!!!!!!!!!!!!!!!!
  if(!m_logic->m_NodeManager)
    return false;

  if(!AskConfirmAndSave())
    return false;

  mafString file = file_to_open;
  if(file.empty())
  {
#ifdef MAF_USE_CURL
    if (m_StorageSettings->GetStorageType() == mafGUISettingsStorage::HTTP)
    {
      mafGUIDialogRemoteFile remoteFile;
      remoteFile.ShowModal();
      file = remoteFile.GetFile();
    }
    else
#endif
    {
		  mafString wildc = _R("MAF Storage Format file (*.");
		  wildc += m_logic->m_StorageData->m_Extension;
		  wildc += _R(")|*.");
		  wildc += m_logic->m_StorageData->m_Extension;
			wildc += _R("|Compressed file (*.z") + m_logic->m_StorageData->m_Extension + _R(")|*.z") + m_logic->m_StorageData->m_Extension;
		  //mafString wildc    = _("MAF Storage Format file (*.msf)|*.msf|Compressed file (*.zmsf)|*.zmsf");
      file = mafGetOpenFile(_R(""), wildc);
    }
    if(file.empty())
      return false;
  }

  if(!OnFileClose(true))
    return false;

  mafString protocol;
#ifdef MAF_USE_CURL
  bool remote_file = false;
  if (IsRemote(file,protocol))
  {
    remote_file = true;
    const mafString& cache_folder = m_StorageSettings->GetCacheFolder();
    if (!mafDirExists(cache_folder))
      mafDirMake(cache_folder);
    auto rs = std::make_unique<mafRemoteStorage>();
    rs->SetTmpFolder(cache_folder);
    rs->SetHostName(m_StorageSettings->GetRemoteHostName());
    rs->SetRemotePort(m_StorageSettings->GetRemotePort());
    rs->SetUsername(m_StorageSettings->GetUserName());
    rs->SetPassword(m_StorageSettings->GetPassword());
    rs->Initialize();
    m_Storage = std::move(rs);
  }
  else
#endif
  {
    if(!mafFileExists(file))
    {
      mafString msg;
      msg = _L("File ");
      msg += file;
      msg += _L(" not found!");
      mafWarningMessage(_M(msg));

      if(m_logic->m_FileHistoryIdx != -1)
      {
        m_logic->m_FileHistory.RemoveFileFromHistory(m_logic->m_FileHistoryIdx); // remove filename to history
        m_logic->m_FileHistory.Save(*m_logic->m_Config); // Save file history to registry
        m_logic->m_FileHistoryIdx = -1;
      }
      return false;
    }
    m_logic->m_Storage = std::make_unique<mafStorage>();
  }
  m_logic->m_Storage->SetListener(this);
  m_logic->m_Storage->SetManager(m_logic->m_NodeManager.get());

  auto disableAll = std::make_unique<wxWindowDisabler>();
  auto wait_cursor = std::make_unique<wxBusyCursor>();

  mafString unixname = file;
  mafString path, name, ext;
  mafSplitPath(file,&path,&name,&ext);
  if(ext == _R("zmsf"))
  {
#ifdef MAF_USE_CURL
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
      ((mafRemoteStorage *)m_Storage.get())->GetRemoteFileManager()->DownloadRemoteFile(remote_filename, local_filename); // download the remote file in the download cache
      file = local_filename;
    }
#endif
    m_logic->m_StorageData->m_ZipFile = file;
    unixname = mafOpenZIP(file, m_logic->m_Storage->GetTmpFolder(), m_logic->m_Storage->m_TmpDir); // open the zmsf archive and extract it to the temporary directory
    if(unixname.empty())
    {
      mafMessage(_M(mafString(_L("Bad or corrupted zmsf file!"))));
      m_logic->m_NodeManager->SetListener(this);
      m_logic->m_Storage.reset();
      return false;
    }
    wxSetWorkingDirectory(m_logic->m_Storage->m_TmpDir.toWx());
  }

  ParsePathName(unixname);

  m_logic->m_StorageData->m_MSFFile = unixname;
  m_logic->m_Storage->SetURL(m_logic->m_StorageData->m_MSFFile);


  int res = m_logic->m_Storage->Restore();
  if(res != mafStorage::IO_OK && res != mafStorage::IO_WRONG_OBJECT_TYPE)
  {
    m_logic->m_Storage.reset();
    m_logic->m_NodeManager->SetListener(this);
    return false;
  }
  if(res == mafStorage::IO_WRONG_OBJECT_TYPE)
  {
    mafErrorMessage(_M(mafString(_L("Errors during file parsing! Look the log area for error messages."))));
  }
  auto root = mafVMERoot::SafeDownCast(m_logic->m_NodeManager->GetRoot());
  SetAppTag(root.get());
  if(!CheckAppTag(root.get()))
  {
    //Application stamp not valid
    mafMessage(_M(mafString(_L("File not valid for this application!"))));
    m_logic->m_Storage.reset();
    m_logic->m_NodeManager->SetListener(this);
    m_logic->m_NodeManager->SetRoot(NULL);
    return false;
  }
  //root->Initialize();
  //m_NodeManager->SetRoot(root);
  VmeSelected(root);
  mafTimeStamp b[2] = {0.0, 0.0};
  root->GetOutput()->GetTimeBounds(b);
  root->SetTreeTime(b[0]); // Set tree time to the starting time
  RestoreLayout();

  if (!m_logic->m_Storage->m_TmpDir.empty())
  {
    m_logic->m_FileHistory.AddFileToHistory(m_logic->m_StorageData->m_ZipFile.toWx()); // add the zmsf file to the history
  }
  else if(/*!remote_file && */res == MAF_OK)
  {
    m_logic->m_FileHistory.AddFileToHistory(m_logic->m_StorageData->m_MSFFile.toWx()); // add the msf file to the history
  }
  else if(res != MAF_OK && m_logic->m_FileHistoryIdx != -1)
  {
    m_logic->m_FileHistory.RemoveFileFromHistory(m_logic->m_FileHistoryIdx); // if something get wrong retoring the file remove it from istory
  }
  m_logic->m_FileHistory.Save(*m_logic->m_Config); // save file history to registry
  return true;
}
void mafLogicWithManagers::OnFileHistory(int fileId)
{
  m_logic->m_FileHistoryIdx = fileId;
  OnFileOpen(mafWxToString(m_logic->m_FileHistory.GetHistoryFile(fileId)));
  m_logic->m_FileHistoryIdx = -1;
}
void mafLogicWithManagers::Save()
{
  if(!m_logic->m_NodeManager)
    return;
	mafString save_default_folder = m_logic->m_StorageSettings->GetDefaultSaveFolder();
	ParsePathName(save_default_folder);
  m_logic->m_StorageData->m_MSFDir = save_default_folder;
  auto root = mafVMERoot::SafeDownCast(m_logic->m_NodeManager->GetRoot());
  if(!root)
    return;
  if(m_logic->m_StorageData->m_MSFFile.empty())
  {
    assert(false);
    return;
  }
  if(!m_logic->m_Storage)
    return;
  if(mafFileExists(m_logic->m_StorageData->m_MSFFile) && m_logic->m_StorageData->m_MakeBakFile) // an msf with the same name exists
  {
    mafString bak_filename = m_logic->m_StorageData->m_MSFFile + _R(".bak");                // create the backup for the saved msf
    mafFileRename(m_logic->m_StorageData->m_MSFFile, bak_filename);  // renaming the founded one
  }
  auto bi = std::make_unique<wxBusyInfo>(_("Saving MSF: Please wait"));
  if (m_logic->m_Storage->Store() != MAF_OK) // store the tree
  {
    mafLogMessage(_M(mafString(_L("Error during MSF saving"))));
    return;
  }
  // add the msf (or zmsf) to the history

  m_logic->m_StorageData->m_MakeBakFile = true;
  UpdateFrameTitle();
  m_logic->m_NodeManager->MSFModified(false);
  m_logic->m_FileHistory.Save(*m_logic->m_Config);
}
bool mafLogicWithManagers::OnFileSave()
{
  if(!m_logic->m_NodeManager)
    return true;
	mafString save_default_folder = m_logic->m_StorageSettings->GetDefaultSaveFolder();
	ParsePathName(save_default_folder);
  m_logic->m_StorageData->m_MSFDir = save_default_folder;
  auto root = mafVMERoot::SafeDownCast(m_logic->m_NodeManager->GetRoot());
  if(!root)
    return true;

  if(m_logic->m_Storage && m_logic->m_Storage->GetURL() != m_logic->m_StorageData->m_MSFFile)
  {
    assert(false);
  }
  if(m_logic->m_StorageData->m_MSFFile.empty())
    return OnFileSaveAs();
  Save();
  return true;
}
bool mafLogicWithManagers::OnFileSaveAs()
{
  if(!m_logic->m_NodeManager)
    return true;
  auto root = mafVMERoot::SafeDownCast(m_logic->m_NodeManager->GetRoot());
  if(!root)
    return true;

  m_logic->m_StorageData->m_MSFFile.clear(); // set filenames to empty so the MSFSave method will ask for them
  m_logic->m_StorageData->m_ZipFile.clear();
  m_logic->m_StorageData->m_MakeBakFile = false;

  // new file to save: ask to the application which is the default
  // modality to save binary files.
  /*mafEvent e(this,mafDataVector::SINGLE_FILE_DATA);
  OnEvent(&e);
  SetSingleBinaryFile(e.GetBool()); // set the save modality for time-varying vme*/

  // ask for the new file name.
  mafString wildc = _L("MAF Storage Format file (*.msf)|*.msf|Compressed file (*.zmsf)|*.zmsf");
  mafString file = mafGetSaveFile(m_logic->m_StorageData->m_MSFDir, wildc);
  if(file.empty())
    return false;
  mafString tmpDir;
  if(!mafFileExists(file))
  {
    mafString path, name, ext, file_dir;
    mafSplitPath(file,&path,&name,&ext);
    size_t length = path.length();
    if(length > 0 && path[length - 1] != '\\' && path[length - 1] != '/')
      file_dir = path + _R("/");
    file_dir = path + _R("/") + name;
    if(!mafDirExists(file_dir))
      mafDirMake(file_dir);
    if (ext == _R("zmsf"))
    {
      m_logic->m_StorageData->m_ZipFile = file;
      tmpDir = file_dir;
      ext = _R("msf");
    }
    file = file_dir + _R("/") + name + _R(".") + ext;
  }

  ParsePathName(file);

  m_logic->m_StorageData->m_MSFFile = file;

  if(m_logic->m_Storage && m_logic->m_StorageData->m_MSFFile != m_logic->m_Storage->GetURL())
  {
    auto iter = root->NewIterator();
    for(auto node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      if(auto vga = mafVMEGenericAbstract::SafeDownCast(node))
      {
        if (mafDataVector *dv = vga->GetDataVector())
        {
          dv->Modified();
        }
      }
    }
  }

  if(!m_logic->m_Storage)
  {
    m_logic->m_Storage = std::make_unique<mafStorage>();
    m_logic->m_Storage->SetListener(this);
    m_logic->m_Storage->SetManager(m_logic->m_NodeManager.get());
  }
  m_logic->m_Storage->SetURL(m_logic->m_StorageData->m_MSFFile);
  m_logic->m_Storage->m_TmpDir = tmpDir;
  Save();
  // add the msf (or zmsf) to the history
  if (!m_logic->m_StorageData->m_ZipFile.empty())
  {
    mafZIPSave(m_logic->m_StorageData->m_ZipFile, m_logic->m_Storage->m_TmpDir);
    m_logic->m_FileHistory.AddFileToHistory(m_logic->m_StorageData->m_ZipFile.toWx()); // add the zmsf to the file history
  }
  else
  {
    m_logic->m_FileHistory.AddFileToHistory(m_logic->m_StorageData->m_MSFFile.toWx()); // add the msf to the file history
  }
  return true;
}
void mafLogicWithManagers::OnQuit()
{
  if (m_logic->m_OpManager && m_logic->m_OpManager->Running())
  {
    return;
  }

  if(m_logic->m_ApplicationLayoutSettings->GetModifiedLayouts())
  {
    int answer = wxMessageBox(_("would you like to save your layout list ?"),_("Confirm"),wxYES_NO|wxCANCEL|wxICON_QUESTION , m_logic->m_frame);
    if(answer == wxCANCEL) 
      return;
    else if(answer == wxYES) 
      m_logic->m_ApplicationLayoutSettings->SaveApplicationLayout();
  }

  if(!OnFileClose())
    return;
  /*int answer = wxMessageBox(_("quit program ?"), _("Confirm"), wxYES_NO | wxICON_QUESTION , m_Win);
  if(answer != wxYES) 
  return;*/

#ifdef MAF_USE_CURL
  m_RemoteLogic.reset();
#endif
  m_logic->m_NodeManager.reset();
  m_logic->m_MaterialChooser.reset();
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  SetGlobalMouse(NULL);
  m_logic->m_InteractionManager.reset();
#endif
  m_logic->m_ViewManager.reset();
  m_logic->m_OpManager.reset();

  // must be deleted after m_NodeManager
  m_logic->m_SideBar.reset();

  // if OnQuit is redefined in a derived class,  mafLogicWithGUI::OnQuit() must be called last

  mafYield();
  if (m_logic->m_PlugLogbar)
  {
    delete wxLog::SetActiveTarget(NULL);
  }
  m_logic->m_frame->Destroy();
#ifdef MAF_USE_VTK 
  vtkTimerLog::CleanupLog();
  vtkDEL(m_logic->m_VtkLog);
#endif
}
void mafLogicWithManagers::VmeDoubleClicked(mafEvent &e)
{
  if (auto node = e.GetVme())
  {
    mafLogMessage(_M(_R("Double click on ") + node->GetName()));
  }
}
void mafLogicWithManagers::VmeSelect(mafEvent& e)	//modified by Paolo 10-9-2003
{
  auto node = e.GetVme();

  if(node == nullptr)
  {
    //node can be selected by its ID
    if(m_logic->m_NodeManager)
    {
		  long vme_id = e.GetArg();
		  if (auto root = mafVMERoot::SafeDownCast(m_logic->m_NodeManager->GetRoot()))
		  {
			  node = root->FindInTreeById(vme_id).get();
			  e.SetVme(node);
      }
    }
  }

  if(node && m_logic->m_OpManager && node != m_logic->m_OpManager->GetSelectedVme().get())
  {
    mafOpSelect opsel;
    opsel.SetNewSel(node->SharedFromThis());
    m_logic->m_OpManager->OpExec(&opsel);

    //OnEvent(&mafEvent(this,VME_SELECTED,node));
    mafLogMessage(_M(_R("node selected: ") + node->GetName()));
  }

#ifdef MAF_USE_CURL
  if(m_RemoteLogic && (e.GetSender() != m_RemoteLogic.get()) && m_RemoteLogic->IsSocketConnected())
  {
    m_RemoteLogic->VmeSelected(node);
  }
#endif
}
void mafLogicWithManagers::VmeSelected(std::shared_ptr<mafNode> vme, bool remote)
{
  if(m_logic->m_ViewManager) m_logic->m_ViewManager->VmeSelect(vme.get());
  if(m_logic->m_OpManager)   { m_logic->m_OpManager->VmeSelected(vme);    EnableOperations(true);}
	if(m_logic->m_SideBar)     m_logic->m_SideBar->VmeSelected(vme.get());
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if (m_logic->m_InteractionManager)
    m_logic->m_InteractionManager->VmeSelected(vme.get());
#endif

#ifdef MAF_USE_CURL
  if(remote && m_RemoteLogic && m_RemoteLogic->IsSocketConnected())
  {
    m_RemoteLogic->VmeSelected(vme);
  }
#endif
}
void mafLogicWithManagers::VmeShow(mafNode *vme, bool visibility)
{
	if(m_logic->m_ViewManager) m_logic->m_ViewManager->VmeShow(vme, visibility);
  bool vme_in_tree = vme->IsVisible(); //check VisibleToTraverse flag.
  if(m_logic->m_SideBar && vme_in_tree)
    m_logic->m_SideBar->VmeShow(vme,visibility);
}
void mafLogicWithManagers::VmeModified(mafNode *vme)
{
  if(m_logic->m_PlugTimebar) UpdateTimeBounds();
  bool vme_in_tree = vme->IsVisible();
  if(m_logic->m_SideBar && vme_in_tree)
    m_logic->m_SideBar->VmeModified(vme);
	if(m_logic->m_NodeManager) m_logic->m_NodeManager->MSFModified(true);
}
void mafLogicWithManagers::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
  if(m_logic->m_NodeManager)
    m_logic->m_NodeManager->VmeAdd(vme);
}
void mafLogicWithManagers::VmeAdded(std::shared_ptr<mafNode> vme)
{
  if(m_logic->m_NodeManager)
  {
    if (auto root = mafVMERoot::SafeDownCast(m_logic->m_NodeManager->GetRoot()))
    {
      if (auto vmenode = mafVME::SafeDownCast(vme))
      {
        // Update the new VME added to the tree with the current time-stamp
        // present in the tree.
        vmenode->SetTimeStamp(root->GetTimeStamp());
      }
    }
  }
  if(m_logic->m_ViewManager)
    m_logic->m_ViewManager->VmeAdd(vme);
  bool vme_in_tree = true;
  vme_in_tree = !vme->GetTagArray()->GetTag(_R("VISIBLE_IN_THE_TREE")) || 
    (vme->GetTagArray()->GetTag(_R("VISIBLE_IN_THE_TREE")) && vme->GetTagArray()->GetTag(_R("VISIBLE_IN_THE_TREE"))->GetValueAsDouble() != 0);
  if(m_logic->m_SideBar && vme_in_tree)
    m_logic->m_SideBar->VmeAdd(vme);
  if(m_logic->m_PlugTimebar)
    UpdateTimeBounds();
}
void mafLogicWithManagers::RestoreLayout()
{
  // Retrieve the saved layout.
  auto vme = m_logic->m_NodeManager->GetRoot();
  auto app_layout = mmaApplicationLayout::SafeDownCast(vme->GetAttribute(_R("ApplicationLayout")));
  if (app_layout)
  {
    int answer = wxMessageBox(_("Do you want to load the layout?"), _("Warning"), wxYES_NO);
    if (answer == wxNO)
    {
      return;
    }
    
    m_logic->m_ApplicationLayoutSettings->SetVisibilityVME(true);
    m_logic->m_ApplicationLayoutSettings->ApplyTreeLayout();
    m_logic->m_ApplicationLayoutSettings->SetVisibilityVME(false);
  }
}


void mafLogicWithManagers::VmeExpand(mafNode *vme)
{
  m_logic->m_SideBar->VmeExpand(vme);
}
void mafLogicWithManagers::VmeCollapse(mafNode *vme)
{
  m_logic->m_SideBar->VmeCollapse(vme);
}
void mafLogicWithManagers::VmeExpandSubTree(mafNode *vme)
{
  m_logic->m_SideBar->VmeExpandSubTree(vme);
}
void mafLogicWithManagers::VmeCollapseSubTree(mafNode *vme)
{
  m_logic->m_SideBar->VmeCollapseSubTree(vme);
}
void mafLogicWithManagers::VmeExpandVisible(mafNode *vme)
{
  m_logic->m_SideBar->VmeExpandVisible(vme);
}
void mafLogicWithManagers::VmeRemove(mafNode *vme)
{
  if(m_logic->m_NodeManager)
    m_logic->m_NodeManager->VmeRemove(vme);
}
void mafLogicWithManagers::VmeRemoving(mafNode *vme)
{
  bool vme_in_tree = true;
  vme_in_tree = !vme->GetTagArray()->GetTag(_R("VISIBLE_IN_THE_TREE")) || 
    (vme->GetTagArray()->GetTag(_R("VISIBLE_IN_THE_TREE")) && vme->GetTagArray()->GetTag(_R("VISIBLE_IN_THE_TREE"))->GetValueAsDouble() != 0);
  if(m_logic->m_SideBar && vme_in_tree)
    m_logic->m_SideBar->VmeRemove(vme);
	if(m_logic->m_ViewManager)
    m_logic->m_ViewManager->VmeRemove(vme);
  if(m_logic->m_PlugTimebar)
    UpdateTimeBounds();
  if (m_logic->m_ViewManager)
    m_logic->m_ViewManager->CameraUpdate();
}
void mafLogicWithManagers::OpRunStarting()
{
  EnableMenuAndToolbar(false);
  EnableOperations(false);
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if(m_logic->m_InteractionManager) m_logic->m_InteractionManager->EnableSelect(false);
#endif
  if(m_logic->m_SideBar)    m_logic->m_SideBar->EnableSelect(false);
  EnableItem(MENU_EDIT_FIND_VME, false);
}
void mafLogicWithManagers::OpRunTerminated()
{
  EnableMenuAndToolbar(true);
  EnableOperations(true);
// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
  if(m_logic->m_InteractionManager)
    m_logic->m_InteractionManager->EnableSelect(true);
#endif
  if(m_logic->m_SideBar)
    m_logic->m_SideBar->EnableSelect(true);
  EnableItem(MENU_EDIT_FIND_VME, true);
}
void mafLogicWithManagers::EnableMenuAndToolbar(bool enable)
{
  EnableItem(wxID_NEW,enable);
  EnableItem(MENU_FILE_OPEN,enable);
  EnableItem(wxID_SAVE,enable);
  EnableItem(wxID_SAVEAS,enable);
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
void mafLogicWithManagers::OpShowGui(bool push_gui, mafGUIPanel *panel)
{
	if(m_logic->m_SideBar) m_logic->m_SideBar->OpShowGui(push_gui, panel);
}
void mafLogicWithManagers::OpHideGui(bool view_closed)
{
	if(m_logic->m_SideBar) m_logic->m_SideBar->OpHideGui(view_closed);
}
void mafLogicWithManagers::ViewCreate(int viewId)
{
	if(m_logic->m_ViewManager)
  {
    mafView* v = m_logic->m_ViewManager->ViewCreate(viewId);
  }
}
void mafLogicWithManagers::ViewSelect()
{
  if(m_logic->m_ViewManager)
  {
    mafView *view = m_logic->m_ViewManager->GetSelectedView();
    if(m_logic->m_SideBar)	m_logic->m_SideBar->ViewSelect(view);

    EnableItem(CAMERA_RESET, view!=NULL);
    EnableItem(CAMERA_FIT,   view!=NULL);
    EnableItem(CAMERA_FLYTO, view!=NULL);

    EnableItem(wxID_PRINT, view != NULL);
    EnableItem(wxID_PREVIEW, view != NULL);
    EnableItem(wxID_PRINT_SETUP, view != NULL);
    EnableItem(wxID_PAGE_SETUP, view != NULL);

// currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    if (m_logic->m_InteractionManager)
    {
      m_logic->m_InteractionManager->ViewSelected(view);
    }
#endif

    if(m_logic->m_OpManager && !m_logic->m_OpManager->Running())
    {
      // needed to update all the operations that will be enabled on View Creation
      m_logic->m_OpManager->VmeSelected(m_logic->m_OpManager->GetSelectedVme());
    }
  }
}
void mafLogicWithManagers::ViewCreated(mafView *v)
{
  // removed temporarily support for external Views
  if(v) 
  {
#ifdef MAF_USE_CURL
    if(m_RemoteLogic && m_RemoteLogic->IsSocketConnected() && !m_ViewManager->m_FromRemote)
    {
      mafEvent ev(this,VIEW_CREATE,v);
      ev.SetChannel(REMOTE_COMMAND_CHANNEL);
      m_RemoteLogic->OnEvent(&ev);
    }
#endif

    if (GetExternalViewFlag())
    {
      mafGUIViewFrame *extern_view = new mafGUIViewFrame(v, m_logic->m_frame, v->GetLabel().toWx(), m_logic->m_frame->FromDIP(wxPoint(10,10)), m_logic->m_frame->FromDIP(wxSize(800,600)));
      extern_view->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
        [=](wxCommandEvent& event)
        {
          wxWindow* rwi = (wxWindow*)event.GetEventObject();
          { mafEvent evUnq(this, VIEW_SELECT); evUnq.SetView(v); evUnq.SetWin(rwi); extern_view->InvokeEvent(evUnq); }
        }, VIEW_CLICKED);
      extern_view->SetListener(m_logic->m_ViewManager.get());
      v->SetFrame(extern_view);
    }
    else
    {
      // child views
      mafGUIMDIChild *c = new mafGUIMDIChild(v, m_logic->m_frame);
      c->Bind(wxEVT_COMMAND_BUTTON_CLICKED, 
        [=](wxCommandEvent& event)
        {
          c->Activate();
          wxWindow* rwi = (wxWindow*)event.GetEventObject();
          { mafEvent evUnq(this, VIEW_SELECT); evUnq.SetView(v); evUnq.SetWin(rwi); c->InvokeEvent(evUnq); }
        }, VIEW_CLICKED);
      c->SetListener(m_logic->m_ViewManager.get());
      v->SetFrame(c);
    }
  }
}
void mafLogicWithManagers::TimeSet(double t)
{
  if(m_logic->m_NodeManager)
  {
    if(auto root = mafVMERoot::SafeDownCast(m_logic->m_NodeManager->GetRoot()))
      root->SetTreeTime(t);
  }
  if(m_logic->m_ViewManager)
  {
    m_logic->m_ViewManager->CameraUpdate(m_logic->m_TimeBarSettings->GetPlayingInActiveViewport() != 0);
  }
}
void mafLogicWithManagers::UpdateTimeBounds()
{
  double min, max; 
  if(m_logic->m_NodeManager)
  {
    mafTimeStamp b[2] = {0, 0};
    if(auto root = mafVMERoot::SafeDownCast(m_logic->m_NodeManager->GetRoot()))
      root->GetOutput()->GetTimeBounds(b);
    min = b[0];
    max = b[1];
  }
  if(m_logic->m_TimePanel)
  {
    m_logic->m_TimePanel->SetBounds(min,max);
    m_logic->m_frame->ShowPane("timebar", min<max);
  }
}
std::vector<mafNode* > mafLogicWithManagers::VmeChoose(intptr_t vme_accept_function, long style, mafString title, bool multiSelect)
{
  mafGUIVMEChooser vc(m_logic->m_SideBar->GetTree(),title, vme_accept_function, style, multiSelect);
  return vc.ShowChooserDialog();
}
void mafLogicWithManagers::VmeChooseMaterial(mafVME *vme, bool updateProperty)
{
  if (!m_logic->m_MaterialChooser)
  {
    m_logic->m_MaterialChooser = std::make_unique<mafGUIMaterialChooser>();
  }
  if(m_logic->m_MaterialChooser->ShowChooserDialog(vme))
  {
    m_logic->m_ViewManager->PropertyUpdate(updateProperty);
    m_logic->m_ViewManager->CameraUpdate();
    m_logic->m_NodeManager->MSFModified(true);
  }
}
void mafLogicWithManagers::VmeUpdateProperties(mafVME *vme, bool updatePropertyFromTag)
{
  m_logic->m_ViewManager->PropertyUpdate(updatePropertyFromTag);
  m_logic->m_ViewManager->CameraUpdate();
  m_logic->m_NodeManager->MSFModified(true);
}
void mafLogicWithManagers::FindVME()
{
  mafGUICheckTree *tree = m_logic->m_SideBar->GetTree();
  mafGUIDialogFindVme fd(_L("Find VME"));
  fd.SetTree(tree);
  fd.ShowModal();
}
void mafLogicWithManagers::ViewContextualMenu(bool vme_menu)
{
  auto contextMenu = std::make_unique<mafGUIContextualMenu>();
  contextMenu->SetListener(this);
  mafView *v = m_logic->m_ViewManager->GetSelectedView();
  mafGUIMDIChild *c = (mafGUIMDIChild *)m_logic->m_frame->GetActiveChild();
  if(c != NULL)
    contextMenu->ShowContextualMenu(c,v,vme_menu);
}
void mafLogicWithManagers::TreeContextualMenu(mafEvent &e)
{
  auto contextMenu = std::make_unique<mafGUITreeContextualMenu>();
  contextMenu->SetListener(m_logic->m_ApplicationLayoutSettings.get());
  mafView *v = m_logic->m_ViewManager->GetSelectedView();
  auto vme = mafVME::StaticDownCast(e.GetVme());
  bool vme_menu = e.GetBool();
  bool autosort = e.GetArg() != 0;
  contextMenu->CreateContextualMenu((mafGUICheckTree *)e.GetSender(),v,vme,vme_menu);
  contextMenu->ShowContextualMenu();
}
void mafLogicWithManagers::HandleException()
{
  int answare = wxMessageBox(_("Do you want to try to save the unsaved work ?"), _("Fatal Exception!!"), wxYES_NO|wxCENTER);
  if(answare == wxYES)
  {
    OnFileSaveAs();
    m_logic->m_OpManager->StopCurrentOperation();
  }
  OnQuit();
}
void mafLogicWithManagers::SetExternalViewFlag(bool external)
{
  m_logic->m_ExternalViewFlag = external;
  wxConfig *config = new wxConfig(wxEmptyString);
  config->Write("ExternalViewFlag", m_logic->m_ExternalViewFlag);
  cppDEL(config);
}
bool mafLogicWithManagers::GetExternalViewFlag()
{
  wxConfig *config = new wxConfig(wxEmptyString);
  config->Read("ExternalViewFlag", &m_logic->m_ExternalViewFlag, false);
  cppDEL(config);
  return m_logic->m_ExternalViewFlag;
}
void mafLogicWithManagers::UpdateMeasureUnit()
{
  const std::list<mafView *>& v = m_logic->m_ViewManager->GetList();
  for(std::list<mafView*>::const_iterator it = v.begin(); it != v.end(); ++it)
    (*it)->OptionsUpdate();
}
void mafLogicWithManagers::ImportExternalFile(mafString &filename)
{
  mafString path, name, ext;
  mafSplitPath(filename,&path,&name,&ext);
  ext.MakeLower();
  /*if (ext == _R("vtk"))
  {
    auto vtkImporter = std::make_unique<mafOpImporterVTK>(_R("importer"));
    vtkImporter->SetInput(m_logic->m_NodeManager->GetRoot());
    vtkImporter->SetListener(m_logic->m_OpManager.get());
    vtkImporter->SetFileName(filename);
    vtkImporter->ImportVTK();
    vtkImporter->OpDo();
  }
  else if (ext == _R("stl"))
  {
    auto stlImporter = std::make_unique<mafOpImporterSTL>(_R("importer"));
    stlImporter->SetInput(m_logic->m_NodeManager->GetRoot());
    stlImporter->SetListener(m_logic->m_OpManager.get());
    stlImporter->SetFileName(filename.GetCStr());
    stlImporter->ImportSTL();
    stlImporter->OpDo();
  }
  else*/
    mafWarningMessage(_M("Can not import this type of file!"));
}

void mafLogicWithManagers::AddToMenu(const mafString& name, long id, wxMenu* path_menu, const mafString& menuPath)
{
  if (!menuPath.empty())
  {
    wxString op_path = "";
    wxStringTokenizer path_tkz(menuPath.toWx(), "/");
    while (path_tkz.HasMoreTokens())
    {
      op_path = path_tkz.GetNextToken();
      int item = path_menu->FindItem(op_path);
      if (item != wxNOT_FOUND)
      {
        wxMenuItem* menu_item = path_menu->FindItem(item);
        if (menu_item)
          path_menu = menu_item->GetSubMenu();
      }
      else
      {
        wxMenu* sub_menu = new wxMenu;
        path_menu->Append(-1, op_path, sub_menu);
        path_menu = sub_menu;
      }
    }
  }
  path_menu->Append(id, name.toWx(), name.toWx());
  SetAccelerator(name, id);
}

void mafLogicWithManagers::SetAccelerator(const mafString& name, long id)
{
  wxString accelerator, flag = "", extra_flag = "", key_code = "";
  int flag_num;
  accelerator = name.GetCStr();
  wxStringTokenizer tkz(accelerator, "\t");
  int token = tkz.CountTokens();

  if (token > 1)
  {
    accelerator = tkz.GetNextToken();
    accelerator = tkz.GetNextToken();
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
    else if (flag == "Alt")
      flag_num = wxACCEL_ALT;
    else if (flag == "Shift")
      flag_num = wxACCEL_SHIFT;

    if (extra_flag == "Ctrl")
      flag_num |= wxACCEL_CTRL;
    else if (extra_flag == "Alt")
      flag_num |= wxACCEL_ALT;
    else if (extra_flag == "Shift")
      flag_num |= wxACCEL_SHIFT;

    m_logic->m_AccelTable.push_back(wxAcceleratorEntry(flag_num, (int)*key_code.c_str(), id));
  }
}

void mafLogicWithManagers::EnableItem(int item, bool enable)
{
  if (m_logic->m_MenuBar)
    // must always check if a menu item exist because
    // during application shutdown it is not guaranteed
    if (m_logic->m_MenuBar->FindItem(item))
      m_logic->m_MenuBar->Enable(item, enable);
  if (m_logic->m_ToolBar)
    m_logic->m_ToolBar->EnableTool(item, enable);
}

void mafLogicWithManagers::SetDirName(const mafString& dirname)
{
  m_logic->m_StorageData->m_MSFDir = dirname;
};

void mafLogicWithManagers::SetFileExtension(mafString& extension)
{
	m_logic->m_StorageData->m_Extension = extension;
};
