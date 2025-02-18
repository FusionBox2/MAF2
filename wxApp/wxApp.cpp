#include "ftkConfigure.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ftk/Gui/MainFrame.h"

#include "mafPics.h"

#include <wx/laywin.h>
#include <wx/mdi.h>
#include <wx/aui/aui.h>
#include "mafStorage.h"

#include "mafPics.h"
#include "mafGUIDockSettings.h" // tmp //SIL. 05-jun-2006 :

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#include "vtkCommand.h"
#include "vtkAlgorithm.h"
#include "vtkViewport.h"
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif //MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#include "wx/preferences.h"
#include "wx/artprov.h"
#include <wx/aboutdlg.h>
#include "mafGUITimeBar.h"
#include "mafWXLog.h"
#include "mafPics.h"
#ifdef MAF_USE_VTK
#include "mafVTKLog.h"
#include "vtkTimerLog.h"
#endif
#include "mafGUITimeBar.h"
#include "mafGUILocaleSettings.h"
#include "mafGUIMeasureUnitSettings.h"
#include "mafGUIApplicationSettings.h"
#include "mafGUISettingsStorage.h"
#include "mafGUISettingsTimeBar.h"
#include "mafSideBar.h"

#include <memory>
#include "mafGUIVMEChooser.h"
#include "mafNodeManager.h"
#include "mafStorage.h"
#include "wx/filehistory.h"




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
#include "medWizardManager.h"

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class mafGUIMDIFrameCallback;
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif //MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

BEGIN_FTK_NAMESPACE

template<class BaseClass, long DefaultStyle>
class MainFrame : public BaseClass
{
public:
  MainFrame();

  MainFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = DefaultStyle, const wxString& name = wxASCII_STR(wxFrameNameStr));

  ~MainFrame() override;

  bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = DefaultStyle, const wxString& name = wxASCII_STR(wxFrameNameStr));

  mafGUI* GetDockSettingGui() { return m_DockSettings->GetGui(); }

	wxAuiManager& GetDockManager() { return m_auiManager; }

private:
  wxAuiManager m_auiManager;
  std::unique_ptr<mafGUIDockSettings> m_DockSettings;
};

template <class BaseFrame, long DefaultStyle>
MainFrame<BaseFrame, DefaultStyle>::MainFrame() = default;

template <class BaseFrame, long DefaultStyle>
MainFrame<BaseFrame, DefaultStyle>::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
  : MainFrame()
{
  Create(parent, id, title, pos, size, style, name);
}

template <class BaseFrame, long DefaultStyle>
MainFrame<BaseFrame, DefaultStyle>::~MainFrame() = default;

template <class BaseFrame, long DefaultStyle>
bool MainFrame<BaseFrame, DefaultStyle>::Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
  if (!BaseFrame::Create(parent, id, title, pos, size, style, name))
  {
    return false;
  }

  m_auiManager.SetManagedWindow(this);
  m_auiManager.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
  m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR));
  m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR));
  m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR));
  m_auiManager.Update();
  m_DockSettings = std::make_unique<mafGUIDockSettings>(m_auiManager);

#if 0
  mafSetFrame(this);
  CreateControls();

  m_ID_PBCall = 0;
  m_PBCalls.clear();

  BaseFrame::Centre();

#ifdef MAF_USE_VTK
  vtkNEW(m_ProgressCallback);
  m_ProgressCallback->SetFrame(this);
  m_ProgressCallback->SetMode(0);
  vtkNEW(m_StartCallback);
  m_StartCallback->SetFrame(this);
  m_StartCallback->SetMode(1);
  vtkNEW(m_EndCallback);
  m_EndCallback->SetFrame(this);
  m_EndCallback->SetMode(2);
#endif //MAF_USE_VTK
#endif
  return true;
}

template<class BaseClass, long DefaultStyle>
bool AddPane(MainFrame<BaseClass, DefaultStyle>& frame, wxWindow* window, wxAuiPaneInfo& pane_info)
{
  if (!frame.GetDockManager().AddPane(window, pane_info))
  {
    return false;
  }

  if (pane_info.caption.IsEmpty())
  {
    return true;
  }

  wxMenuBar* menubar = frame.GetMenuBar();
  if (!menubar)
    return true;
  int idx = menubar->FindMenu("View");
  if (idx == wxNOT_FOUND)
    return true;
  wxMenu* viewMenu = menubar->GetMenu(idx);
  if (pane_info.IsToolbar())
  {
    wxMenu* toolbarsMenu = nullptr;
    int ids = viewMenu->FindItem("Toolbars");
    if (ids == wxNOT_FOUND)
    {
      toolbarsMenu = new wxMenu;
      wxMenuItem* toolbarsItem = viewMenu->Insert(0, wxID_ANY, "Toolbars", toolbarsMenu);
    }
    else
    {
      wxMenuItem* sm = viewMenu->FindItem(ids);
      toolbarsMenu = sm->GetSubMenu();
    }
    if (toolbarsMenu)
    {
      toolbarsMenu->Append(window->GetId(), pane_info.caption, "", wxITEM_CHECK);
    }
  }
  else
  {
    int ids = viewMenu->FindItem("Layout");
    if (ids != wxNOT_FOUND)
    {
      size_t pos = 0;
      wxMenuItem* sm = viewMenu->FindChildItem(ids, &pos);
      viewMenu->Insert(pos, window->GetId(), pane_info.caption, "", wxITEM_CHECK);
    }
    else
    {
      viewMenu->Append(window->GetId(), pane_info.caption, "", wxITEM_CHECK);
    }
  }
  //frame.BaseFrame::Bind(wxEVT_MENU, &MainFrame::OnSwitchPane, this, window->GetId(), window->GetId());
  //frame.::Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdatePaneUI, this, window->GetId(), window->GetId());
  return true;
}

template<class BaseClass, long DefaultStyle>
wxWindow* RemovePane(MainFrame<BaseClass, DefaultStyle>& frame, wxAuiPaneInfo& pane_info)
{
  wxMenuBar* menubar = frame.GetMenuBar();
  int idx = menubar->FindMenu("View");
  if (idx != wxNOT_FOUND)
  {
    wxMenu* viewMenu = menubar->GetMenu(idx);
    if (pane_info.IsToolbar())
    {
      int ids = viewMenu->FindItem("Toolbars");
      wxMenuItem* sm = viewMenu->FindItem(ids);
      wxMenu* toolbarsMenu = sm->GetSubMenu();
      delete toolbarsMenu->Remove(pane_info.window->GetId());
      if (toolbarsMenu->GetMenuItemCount() == 0)
      {
        delete viewMenu->Remove(sm);
      }
    }
    else
    {
      delete viewMenu->Remove(pane_info.window->GetId());
    }
  }
  wxWindow* w = pane_info.window;
  if (!frame.GetDockManager().DetachPane(pane_info.window))
    return nullptr;
  return w;
}

bool AskConfirmAndSave()
{
  return true;
}

END_FTK_NAMESPACE

namespace ftk
{
  const long fbDefaultStyle = wxDEFAULT_FRAME_STYLE;
  auto CreateFrame(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize, long style = fbDefaultStyle, const wxString& name = wxFrameNameStr)
  {
    return new MainFrame<wxMDIParentFrame, fbDefaultStyle>(parent, id, title, pos, size, style, name);
  }
}

BEGIN_FTK_NAMESPACE

class MAF_EXPORT mafStorageDataNew
{
public:
  mafStorageDataNew(const mafString& extention, bool makeBakFile, const mafString& msfDir)
    : m_Extension(extention)
    , m_MakeBakFile(makeBakFile)
    , m_MSFDir(msfDir)
  {
  }
  mafString m_Extension;
  bool      m_MakeBakFile;      ///< Flag used to create or not the backup file of the saved msf.
  mafString m_MSFDir;           ///< Directory name in which is present the msf file.
  mafString m_MSFFile;          ///< File name of the data associated to the tree.
  mafString m_ZipFile;          ///< File name of compressed archive in which save the data associated to the tree.
};


class MainLogic
{
public:
  MainLogic(mafBaseEventHandler *listener);
  ~MainLogic();

  std::unique_ptr<mafGUILocaleSettings> m_LocaleSettings;
  std::unique_ptr<mafGUIMeasureUnitSettings> m_MeasureUnitSettings;
  std::unique_ptr<mafGUIApplicationSettings> m_ApplicationSettings;
  std::unique_ptr<mafGUISettingsStorage>     m_StorageSettings;
  std::unique_ptr<mafGUISettingsTimeBar>     m_TimeBarSettings;

  MainFrame<wxMDIParentFrame, ftk::fbDefaultStyle>* m_frame;

  wxToolBar* m_ToolBar = nullptr;
  wxMenuBar* m_MenuBar = nullptr;
  wxString m_LastSelectedPanel;
  mafGUITimeBar* m_TimePanel;
  bool m_LogToFile = false;// m_ApplicationSettings->GetLogToFileStatus();
  bool m_LogAllEvents = false;// m_ApplicationSettings->GetLogVerboseStatus();
  mafWXLog* m_Logger = nullptr;
  mafVTKLog* m_VtkLog = nullptr;

	std::vector<wxAcceleratorEntry> m_AccelTable; ///< List of Accelerators for menu items.

  bool m_PlugMenu = true;
  bool m_PlugToolbar = true;
  bool m_PlugSidebar = true;
  long m_SidebarStyle = mafSideBar::DOUBLE_NOTEBOOK;
  bool m_PlugTimebar = true;
  bool m_PlugLogbar = true;




  bool m_ExternalViewFlag = false;
  bool m_CameraLinkingObserverFlag = false;

  std::unique_ptr<mafSideBar>             m_SideBar;
  std::unique_ptr<mafNodeManager>         m_NodeManager;
  std::unique_ptr<mafViewManager>         m_ViewManager;
  std::unique_ptr<mafOpManager>           m_OpManager;
  std::unique_ptr<mafInteractionManager>  m_InteractionManager;
#ifdef MAF_USE_CURL
  std::unique_ptr<mafRemoteLogic>         m_RemoteLogic;
#endif

  std::unique_ptr<mafGUIMaterialChooser>  m_MaterialChooser;
  std::unique_ptr<mafPrintSupport> m_PrintSupport;

  wxMenu* m_ImportMenu = nullptr;
  wxMenu* m_ExportMenu = nullptr;
  wxMenu* m_RecentFileMenu = nullptr;
  wxMenu* m_OpMenu = nullptr;
  wxMenu* m_ViewMenu = nullptr;
  wxMenu* m_EditMenu = nullptr;
  wxMenu* m_ViewListMenu = nullptr;
  wxFileHistory	m_FileHistory;      ///< Used to hold recently opened files
  wxConfigBase* m_Config;           ///< Application configuration for file history management


  std::unique_ptr<mafGUISettingsDialog> m_SettingsDialog;
  std::unique_ptr<mafGUIApplicationLayoutSettings> m_ApplicationLayoutSettings;
  std::unique_ptr<mafGUISettingsHelp> m_HelpSettings;

  std::unique_ptr<mafStorage> m_Storage;          ///< Associated storage
  std::unique_ptr<mafStorageDataNew> m_StorageData;

  std::vector<mafString> m_AppStamp;      ///< Application stamps for our application.
  int                     m_FileHistoryIdx;   ///< Identifier of the file to open

  struct mafMenuElems
  {
    mafMenuElems(bool op, int id, mafID command) :m_op(op), m_id(id), m_command(command) {}
    bool m_op;
    int  m_id;
    mafID m_command;
  };
  std::vector<mafMenuElems> m_MenuElems;

  long m_UserCommandIndex = 0;



  medWizardManager* m_WizardManager = nullptr;
  bool m_UseWizardManager = false;
  bool m_WizardRunning = false;
  wxGauge* m_WizardGauge = nullptr;
  wxStaticText* m_WizardLabel = nullptr;
  bool m_CancelledBeforeOpStarting = false;
  wxMenu* m_WizardMenu = nullptr;
};

MainLogic::MainLogic(mafBaseEventHandler* listener)
{
  m_LocaleSettings = std::make_unique<mafGUILocaleSettings>(listener);
  m_MeasureUnitSettings = std::make_unique<mafGUIMeasureUnitSettings>(listener);
  m_ApplicationSettings = std::make_unique<mafGUIApplicationSettings>(listener);
  m_StorageSettings = std::make_unique<mafGUISettingsStorage>(listener);
  m_TimeBarSettings = std::make_unique<mafGUISettingsTimeBar>(listener);

  m_MeasureUnitSettings->SetListener(listener);
  m_PrintSupport = std::make_unique<mafPrintSupport>();
  //m_SettingsDialog = std::make_unique<mafGUISettingsDialog>();


  m_Config = wxConfigBase::Get();

  mafString msfDir = mafGetApplicationDirectory();
  ParsePathName(msfDir);
  m_StorageData = std::make_unique<mafStorageDataNew>(_R("msf"), true, msfDir);
  m_FileHistoryIdx = -1;
}

MainLogic::~MainLogic() = default;

template<class BaseClass, long DefaultStyle>
class MainFrameOld : public BaseClass
{
public:
  MainFrameOld();

  MainFrameOld(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = DefaultStyle, const wxString& name = wxASCII_STR(wxFrameNameStr));

  ~MainFrameOld() override;

  bool Create(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = DefaultStyle, const wxString& name = wxASCII_STR(wxFrameNameStr));

  mafGUI* GetDockSettingGui() { return m_DockSettings->GetGui(); }

  wxAuiManager& GetDockManager() { return m_auiManager; }

  void OnSwitchPane(wxCommandEvent&);

  void OnUpdatePaneUI(wxUpdateUIEvent& event);

  bool AddPane(wxWindow* window, wxAuiPaneInfo& pane_info);

  wxWindow* RemovePane(const wxString& pane_name);

  wxWindow* RemovePane(wxWindow* window);

  wxWindow* RemovePane(wxAuiPaneInfo& pane_info);

  void ShowPane(const wxString& pane_name, bool show = true);

  void ShowPane(wxWindow* window, bool show = true);

  void ShowPane(wxAuiPaneInfo& pane_info, bool show = true);

  bool IsPaneShown(const wxString& pane_name);

  bool IsPaneShown(wxWindow* window);

  bool IsPaneShown(const wxAuiPaneInfo& pane_info);

  void Busy();

  void Ready();

  void ProgressBarShow();

  void ProgressBarHide();

  void ProgressBarSetVal(int progress);

  int ProgressBarGetStatus();

  void ProgressBarSetText(const wxString& msg);

  void RenderStart();

  void RenderEnd();

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  void BindToProgressBar(vtkObject* vtkobj);

  void BindToProgressBar(vtkAlgorithm* filter);

  void BindToProgressBar(vtkViewport* ren);

protected:
  mafGUIMDIFrameCallback* m_StartCallback;
  mafGUIMDIFrameCallback* m_EndCallback; ;
  mafGUIMDIFrameCallback* m_ProgressCallback;

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif  //MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
protected:
  bool* m_Busy;
  wxGauge* m_Gauge;
  std::vector<int>    m_PBCalls;
  int  m_ID_PBCall;

  wxAuiManager    m_auiManager;
  std::unique_ptr<mafGUIDockSettings> m_DockSettings;
private:
  void CreateControls();
  void CreateStatus();

};

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::CreateControls()
{
  CreateStatus();
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::CreateStatus()
{
  if (auto statusBar = BaseFrame::CreateStatusBar())
  {
    int widths[] = { -1,60,60,60,210,150 }; // enlarged the last tab for the "free mem watch" information
    std::transform(std::begin(widths), std::end(widths), std::begin(widths), [this](int w) {return BaseFrame::FromDIP(w); });
    statusBar->SetFieldsCount(6);
    statusBar->SetStatusWidths(6, widths);
    statusBar->SetStatusText(_("welcome"), 0);
    statusBar->SetStatusText(" ", 1);
    statusBar->SetStatusText(" ", 2);
    statusBar->SetStatusText(" ", 3);

    // Paolo 10 Jul 2006: due to position correctly the progress bar.
    wxRect pr;
    BaseFrame::GetStatusBar()->GetFieldRect(4, pr);
    m_Busy = nullptr;
    m_Gauge = new wxGauge(BaseFrame::GetStatusBar(), -1, 100, pr.GetPosition(), pr.GetSize(), wxGA_SMOOTH);
    m_Gauge->SetForegroundColour(*wxRED);
    m_Gauge->Show(false);
    BaseFrame::GetStatusBar()->Bind(wxEVT_SIZE,
      [this](const wxSizeEvent& event)
      {
        wxRect r;
        BaseFrame::GetStatusBar()->GetFieldRect(4, r);
        m_Gauge->SetPosition(r.GetPosition());
        m_Gauge->SetSize(r.GetSize());
      }
    );
  }
}

template <class BaseFrame, long DefaultStyle>
bool MainFrameOld<BaseFrame, DefaultStyle>::AddPane(wxWindow* window, wxAuiPaneInfo& pane_info)
{
  if (!m_auiManager.AddPane(window, pane_info))
  {
    return false;
  }

  if (pane_info.caption.IsEmpty())
  {
    return true;
  }

  wxMenuBar* menubar = BaseFrame::GetMenuBar();
  if (!menubar)
    return true;
  int idx = menubar->FindMenu("View");
  if (idx == wxNOT_FOUND)
    return true;
  wxMenu* viewMenu = menubar->GetMenu(idx);
  if (pane_info.IsToolbar())
  {
    wxMenu* toolbarsMenu = nullptr;
    int ids = viewMenu->FindItem("Toolbars");
    if (ids == wxNOT_FOUND)
    {
      toolbarsMenu = new wxMenu;
      wxMenuItem* toolbarsItem = viewMenu->Insert(0, wxID_ANY, "Toolbars", toolbarsMenu);
    }
    else
    {
      wxMenuItem* sm = viewMenu->FindItem(ids);
      toolbarsMenu = sm->GetSubMenu();
    }
    if (toolbarsMenu)
    {
      toolbarsMenu->Append(window->GetId(), pane_info.caption, "", wxITEM_CHECK);
    }
    BaseFrame::Bind(wxEVT_MENU, &MainFrameOld::OnSwitchPane, this, window->GetId(), window->GetId());
    BaseFrame::Bind(wxEVT_UPDATE_UI, &MainFrameOld::OnUpdatePaneUI, this, window->GetId(), window->GetId());
  }
  else
  {
    int ids = viewMenu->FindItem("Layout");
    if (ids != wxNOT_FOUND)
    {
      size_t pos = 0;
      wxMenuItem* sm = viewMenu->FindChildItem(ids, &pos);
      viewMenu->Insert(pos, window->GetId(), pane_info.caption, "", wxITEM_CHECK);
    }
    else
    {
      viewMenu->Append(window->GetId(), pane_info.caption, "", wxITEM_CHECK);
    }
    BaseFrame::Bind(wxEVT_MENU, &MainFrameOld::OnSwitchPane, this, window->GetId(), window->GetId());
    BaseFrame::Bind(wxEVT_UPDATE_UI, &MainFrameOld::OnUpdatePaneUI, this, window->GetId(), window->GetId());
  }
  return true;
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::OnSwitchPane(wxCommandEvent& event)
{
  wxAuiPaneInfoArray& all_panes = m_auiManager.GetAllPanes();
  size_t count = all_panes.GetCount();
  for (size_t i = 0; i < count; i++)
  {
    if (event.GetId() == all_panes[i].window->GetId())
    {
      all_panes[i].Show(!all_panes[i].IsShown());
      m_auiManager.Update();
      return;
    }
  }
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::OnUpdatePaneUI(wxUpdateUIEvent& event)
{
  wxAuiPaneInfoArray& all_panes = m_auiManager.GetAllPanes();
  size_t count = all_panes.GetCount();
  for (size_t i = 0; i < count; i++)
  {
    if (event.GetId() == all_panes[i].window->GetId())
    {
      event.Check(all_panes[i].IsShown());
    }
  }
}

template <class BaseFrame, long DefaultStyle>
wxWindow* MainFrameOld<BaseFrame, DefaultStyle>::RemovePane(const wxString& pane_name)
{
  wxAuiPaneInfo& pi = m_auiManager.GetPane(pane_name);
  if (pi.IsOk())
  {
    return RemovePane(pi);
  }
  return nullptr;
}

template <class BaseFrame, long DefaultStyle>
wxWindow* MainFrameOld<BaseFrame, DefaultStyle>::RemovePane(wxWindow* window)
{
  wxAuiPaneInfo& pi = m_auiManager.GetPane(window);
  if (pi.IsOk())
  {
    return RemovePane(pi);
  }
  return nullptr;
}

template <class BaseFrame, long DefaultStyle>
wxWindow* MainFrameOld<BaseFrame, DefaultStyle>::RemovePane(wxAuiPaneInfo& pane_info)
{
  wxMenuBar* menubar = BaseFrame::GetMenuBar();
  int idx = menubar->FindMenu("View");
  if (idx != wxNOT_FOUND)
  {
    wxMenu* viewMenu = menubar->GetMenu(idx);
    if (pane_info.IsToolbar())
    {
      int ids = viewMenu->FindItem("Toolbars");
      wxMenuItem* sm = viewMenu->FindItem(ids);
      wxMenu* toolbarsMenu = sm->GetSubMenu();
      BaseFrame::Unbind(wxEVT_MENU, &MainFrameOld::OnSwitchPane, this, pane_info.window->GetId(), pane_info.window->GetId());
      BaseFrame::Unbind(wxEVT_UPDATE_UI, &MainFrameOld::OnUpdatePaneUI, this, pane_info.window->GetId(), pane_info.window->GetId());
      delete toolbarsMenu->Remove(pane_info.window->GetId());
      if (toolbarsMenu->GetMenuItemCount() == 0)
      {
        delete viewMenu->Remove(sm);
      }
    }
    else
    {
      BaseFrame::Unbind(wxEVT_MENU, &MainFrameOld::OnSwitchPane, this, pane_info.window->GetId(), pane_info.window->GetId());
      BaseFrame::Unbind(wxEVT_UPDATE_UI, &MainFrameOld::OnUpdatePaneUI, this, pane_info.window->GetId(), pane_info.window->GetId());
      delete viewMenu->Remove(pane_info.window->GetId());
    }
  }
  wxWindow* w = pane_info.window;
  if (!m_auiManager.DetachPane(pane_info.window))
    return nullptr;
  return w;
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::ShowPane(const wxString& pane_name, bool show)
{
  wxAuiPaneInfo& pi = m_auiManager.GetPane(pane_name);
  if (pi.IsOk())
  {
    pi.Show(show);
    m_auiManager.Update();
  }
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::ShowPane(wxWindow* window, bool show)
{
  ShowPane(m_auiManager.GetPane(window), show);
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::ShowPane(wxAuiPaneInfo& pane_info, bool show)
{
  if (pane_info.IsOk())
  {
    pane_info.Show(show);
    m_auiManager.Update(); // sync the MenuItems check 
  }
}

template <class BaseFrame, long DefaultStyle>
bool MainFrameOld<BaseFrame, DefaultStyle>::IsPaneShown(const wxString& pane_name)
{
  return IsPaneShown(m_auiManager.GetPane(pane_name));
}

template <class BaseFrame, long DefaultStyle>
bool MainFrameOld<BaseFrame, DefaultStyle>::IsPaneShown(wxWindow* window)
{
  return IsPaneShown(m_auiManager.GetPane(window));
}

template <class BaseFrame, long DefaultStyle>
bool MainFrameOld<BaseFrame, DefaultStyle>::IsPaneShown(const wxAuiPaneInfo& pane_info)
{
  return pane_info.IsOk() && pane_info.IsShown();

}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::Busy()
{
  BaseFrame::SetStatusText("Busy", 2);
  BaseFrame::SetStatusText("", 3);
  m_Gauge->Show(true);
  m_Gauge->SetValue(0);
  BaseFrame::Refresh(false);
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::Ready()
{
  BaseFrame::SetStatusText("", 2);
  BaseFrame::SetStatusText("", 3);
  m_Gauge->Show(false);
  BaseFrame::Refresh(false);
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::ProgressBarShow()
{
  if (m_PBCalls.empty())
  {
    BaseFrame::SetStatusText("", 0);
    Busy();
    m_PBCalls.push_back(m_ID_PBCall);
  }
  else
  {
    m_ID_PBCall++;
  }
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::ProgressBarHide()
{
  if (!m_PBCalls.empty() && m_PBCalls.back() == m_ID_PBCall)
  {
    BaseFrame::SetStatusText("", 0);
    Ready();
    m_PBCalls.pop_back();
  }
  else
  {
    m_ID_PBCall--;
  }
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::ProgressBarSetVal(int progress)
{
  if (!m_PBCalls.empty() && m_PBCalls.back() == m_ID_PBCall)
  {
    m_Gauge->SetValue(progress);
    BaseFrame::SetStatusText(wxString::Format(" %d%% ", progress), 3);
    mafYield(); //fix on bug #2082
  }
}

template <class BaseFrame, long DefaultStyle>
int MainFrameOld<BaseFrame, DefaultStyle>::ProgressBarGetStatus()
{
  return m_Gauge->GetValue();
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::ProgressBarSetText(const wxString& msg)
{
  BaseFrame::SetStatusText(msg, 0);
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::RenderStart()
{
  BaseFrame::SetStatusText("Rendering", 1);
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::RenderEnd()
{
  BaseFrame::SetStatusText(" ", 1);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#if 0 // def MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class mafGUIMDIFrameCallback : public vtkCommand
{
public:
  vtkTypeMacro(mafGUIMDIFrameCallback, vtkCommand);

  static mafGUIMDIFrameCallback* New() { return new mafGUIMDIFrameCallback; }
  mafGUIMDIFrameCallback() { m_mode = 0; m_Frame = NULL; };
  void SetMode(int mode) { m_mode = mode; };
  void SetFrame(wxFrame* frame) { m_Frame = frame; };

  void Execute(vtkObject* caller, unsigned long, void*) override
  {
    mafGUIMDIFrame* frame = static_cast<mafGUIMDIFrame*>(m_Frame);
    assert(frame);
    if (caller->IsA("vtkAlgorithm"))
    {
      vtkAlgorithm* po = (vtkAlgorithm*)caller;

      if (m_mode == 0) // ProgressEvent-Callback
      {
        mafYield(); //fix on bug #2082 
        frame->ProgressBarSetVal(po->GetProgress() * 100);
        //mafLogMessage("progress = %g", po->GetProgress()*100);
      }
      else if (m_mode == 1) // StartEvent-Callback
      {
        frame->ProgressBarShow();
        frame->ProgressBarSetVal(0);
        //m_Frame->ProgressBarSetText(&wxString(po->GetClassName()));
        { wxString s = po->GetProgressText(); frame->ProgressBarSetText(s); }
      }
      else if (m_mode == 2) // EndEvent-Callback
      {
        frame->ProgressBarHide();
      }
    }
    else if (caller->IsA("vtkViewport"))
    {
      if (m_mode == 1) // StartRenderingEvent-Callback
      {
        frame->RenderStart();
      }
      else if (m_mode == 2) // StartRenderingEvent-Callback
      {
        frame->RenderEnd();
      }
    }
  }
protected:
  int m_mode;
  wxFrame* m_Frame;
};

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::BindToProgressBar(vtkObject* vtkobj)
{
  if (vtkobj->IsA("vtkViewport"))
    BindToProgressBar((vtkViewport*)vtkobj);
  else if (vtkobj->IsA("vtkAlgorithm"))
    BindToProgressBar((vtkAlgorithm*)vtkobj);
  else
    mafLogMessage(_M("wrong vtkObject passed to BindToProgressBar"));
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::BindToProgressBar(vtkAlgorithm* filter)
{
  filter->AddObserver(vtkCommand::ProgressEvent, m_ProgressCallback);
  filter->AddObserver(vtkCommand::StartEvent, m_StartCallback);
  filter->AddObserver(vtkCommand::EndEvent, m_EndCallback);
}

template <class BaseFrame, long DefaultStyle>
void MainFrameOld<BaseFrame, DefaultStyle>::BindToProgressBar(vtkViewport* ren)
{
  ren->AddObserver(vtkCommand::StartEvent, m_StartCallback);
  ren->AddObserver(vtkCommand::EndEvent, m_EndCallback);
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif  //MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

END_FTK_NAMESPACE

  // This struct combines the settings edited in the preferences dialog.
  struct MySettings
  {
    MySettings()
    {
      // Normally we would initialize values by loading them from some
      // persistent storage, e.g. using wxConfig.
      // For demonstration purposes, we just set hardcoded values here.
      m_useMarkdown = true;
      m_spellcheck = false;
    }

    bool m_useMarkdown;
    bool m_spellcheck;

    // We don't do the same thing for the second preferences page fields, but
    // we would have included them in a real application.
  };

  class App : public wxApp
  {
  public:
    App();

    ~App();

    bool OnInit() override;

    int  OnExit() override;

    void OnAbout(wxCommandEvent& e);

  	void ShowPreferencesEditor(wxWindow* parent);
    void DismissPreferencesEditor();

    const MySettings& GetSettings() const { return m_settings; }
    void UpdateSettings(const MySettings& settings);

    std::unique_ptr<wxPreferencesEditor> m_prefEditor;

    MySettings m_settings;

    std::unique_ptr<MainLogic> m_logic;

  };
  wxDECLARE_APP(App);

class PrefsPageGeneralPanel : public wxPanel
{
public:
  PrefsPageGeneralPanel(wxWindow* parent) : wxPanel(parent)
  {
    m_useMarkdown = new wxCheckBox(this, wxID_ANY, "Use Markdown syntax");
    m_spellcheck = new wxCheckBox(this, wxID_ANY, "Check spelling");

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_useMarkdown, wxSizerFlags().Border());
    sizer->Add(m_spellcheck, wxSizerFlags().Border());

    SetSizerAndFit(sizer);

    m_useMarkdown->Bind(wxEVT_CHECKBOX,
      &PrefsPageGeneralPanel::ChangedUseMarkdown,
      this);
    m_spellcheck->Bind(wxEVT_CHECKBOX,
      &PrefsPageGeneralPanel::ChangedSpellcheck,
      this);
  }

  virtual bool TransferDataToWindow() override
  {
    m_settingsCurrent = wxGetApp().GetSettings();
    m_useMarkdown->SetValue(m_settingsCurrent.m_useMarkdown);
    m_spellcheck->SetValue(m_settingsCurrent.m_spellcheck);
    return true;
  }

  virtual bool TransferDataFromWindow() override
  {
    // Called on platforms with modal preferences dialog to save and apply
    // the changes.
    wxGetApp().UpdateSettings(m_settingsCurrent);
    return true;
  }

private:
  void UpdateSettingsIfNecessary()
  {
    // On some platforms (OS X, GNOME), changes to preferences are applied
    // immediately rather than after the OK or Apply button is pressed, so
    // we need to take them into account as soon as they happen. On others
    // (MSW), we need to wait until the changes are accepted by the user by
    // pressing the "OK" button. To reuse the same code for both cases, we
    // always update m_settingsCurrent object under all platforms, but only
    // update the real application settings if necessary here.
    if (wxPreferencesEditor::ShouldApplyChangesImmediately())
    {
      wxGetApp().UpdateSettings(m_settingsCurrent);
    }
  }

  void ChangedUseMarkdown(wxCommandEvent& e)
  {
    m_settingsCurrent.m_useMarkdown = e.IsChecked();
    UpdateSettingsIfNecessary();
  }

  void ChangedSpellcheck(wxCommandEvent& e)
  {
    m_settingsCurrent.m_spellcheck = e.IsChecked();
    UpdateSettingsIfNecessary();
  }

  wxCheckBox* m_useMarkdown;
  wxCheckBox* m_spellcheck;

  // Settings corresponding to the current values in this dialog.
  MySettings m_settingsCurrent;
};

class PrefsPageTimeBarPanel : public wxPanel
{
public:
  PrefsPageTimeBarPanel(wxWindow* parent) : wxPanel(parent)
  {
    m_useMarkdown = new wxCheckBox(this, wxID_ANY, "Use Markdown syntax");
    m_spellcheck = new wxCheckBox(this, wxID_ANY, "Check spelling");

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_useMarkdown, wxSizerFlags().Border());
    sizer->Add(m_spellcheck, wxSizerFlags().Border());

    SetSizerAndFit(sizer);

    m_useMarkdown->Bind(wxEVT_CHECKBOX,
      &PrefsPageTimeBarPanel::ChangedUseMarkdown,
      this);
    m_spellcheck->Bind(wxEVT_CHECKBOX,
      &PrefsPageTimeBarPanel::ChangedSpellcheck,
      this);
  }

  virtual bool TransferDataToWindow() override
  {
    m_settingsCurrent = wxGetApp().GetSettings();
    m_useMarkdown->SetValue(m_settingsCurrent.m_useMarkdown);
    m_spellcheck->SetValue(m_settingsCurrent.m_spellcheck);
    return true;
  }

  virtual bool TransferDataFromWindow() override
  {
    // Called on platforms with modal preferences dialog to save and apply
    // the changes.
    wxGetApp().UpdateSettings(m_settingsCurrent);
    return true;
  }

private:
  void UpdateSettingsIfNecessary()
  {
    // On some platforms (OS X, GNOME), changes to preferences are applied
    // immediately rather than after the OK or Apply button is pressed, so
    // we need to take them into account as soon as they happen. On others
    // (MSW), we need to wait until the changes are accepted by the user by
    // pressing the "OK" button. To reuse the same code for both cases, we
    // always update m_settingsCurrent object under all platforms, but only
    // update the real application settings if necessary here.
    if (wxPreferencesEditor::ShouldApplyChangesImmediately())
    {
      wxGetApp().UpdateSettings(m_settingsCurrent);
    }
  }

  void ChangedUseMarkdown(wxCommandEvent& e)
  {
    m_settingsCurrent.m_useMarkdown = e.IsChecked();
    UpdateSettingsIfNecessary();
  }

  void ChangedSpellcheck(wxCommandEvent& e)
  {
    m_settingsCurrent.m_spellcheck = e.IsChecked();
    UpdateSettingsIfNecessary();
  }

  wxCheckBox* m_useMarkdown;
  wxCheckBox* m_spellcheck;

  // Settings corresponding to the current values in this dialog.
  MySettings m_settingsCurrent;
};

class PrefsPageGeneral : public wxStockPreferencesPage
{
public:
  PrefsPageGeneral() : wxStockPreferencesPage(Kind_General) {}

  ~PrefsPageGeneral() override = default;

  virtual wxWindow* CreateWindow(wxWindow* parent) override
  {
    return new PrefsPageGeneralPanel(parent);
  }
};

class PrefsPageTopicsPanel : public wxPanel
{
public:
  PrefsPageTopicsPanel(wxWindow* parent) : wxPanel(parent)
  {
    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(this, wxID_ANY, "Search in these topics:"), wxSizerFlags().Border());
    wxListBox* box = new wxListBox(this, wxID_ANY);
    box->SetMinSize(wxSize(400, 300));
    sizer->Add(box, wxSizerFlags(1).Border().Expand());
    m_fulltext = new wxCheckBox(this, wxID_ANY, "Automatically build fulltext index");
    sizer->Add(m_fulltext, wxSizerFlags().Border());

    SetSizerAndFit(sizer);

    if (wxPreferencesEditor::ShouldApplyChangesImmediately())
    {
      m_fulltext->Bind(wxEVT_CHECKBOX,
        &PrefsPageTopicsPanel::ChangedFulltext,
        this);
    }
  }

  virtual bool TransferDataToWindow() override
  {
    // This is the place where you can initialize values, e.g. from wxConfig.
    // For demonstration purposes, we just set hardcoded values.
    m_fulltext->SetValue(true);
    // TODO: handle the listbox
    return true;
  }

  virtual bool TransferDataFromWindow() override
  {
    // Called on platforms with modal preferences dialog to save and apply
    // the changes.
    wxCommandEvent dummy;
    ChangedFulltext(dummy);
    // TODO: handle the listbox
    return true;
  }

private:
  void ChangedFulltext(wxCommandEvent& WXUNUSED(e))
  {
    // save new m_fulltext value and apply the change to the app
  }

  wxCheckBox* m_fulltext;
};

class PrefsPageTopics : public wxPreferencesPage
{
public:
  virtual wxString GetName() const override { return "Topics"; }
  virtual wxBitmapBundle GetIcon() const override
  {
    return wxArtProvider::GetBitmapBundle(wxART_HELP, wxART_TOOLBAR);
  }
  virtual wxWindow* CreateWindow(wxWindow* parent) override
  {
    return new PrefsPageTopicsPanel(parent);
  }
};

#include "pic/lhpBuilder/FRAME_ICON16x16.xpm"
#include "pic/lhpBuilder/FRAME_ICON32x32.xpm"
#include "pic/lhpBuilder/MDICHILD_ICON.xpm"
  App::App()
  {
#ifdef WIN32
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF) | _CRTDBG_LEAK_CHECK_DF);
#endif
    SetAppName("wxApp");
  }

  App::~App() = default;

  bool App::OnInit()
  {
    if (!wxApp::OnInit())
      return false;

    mafPictureFactory::GetPictureFactory()->Initialize();

    mafADDPIC(FRAME_ICON16x16);
    mafADDPIC(FRAME_ICON32x32);
    mafADDPIC(MDICHILD_ICON);

    wxString frameCaption = "LHPFusionBox";
    SetAppName("lhpFusionBox");
    SetAppDisplayName("LHPFusionBox");

    auto frame = ftk::CreateFrame(nullptr, wxID_ANY, frameCaption, wxDefaultPosition, wxWindow::FromDIP(wxSize(800, 600), nullptr));

  	if (!frame)
      return false;

  	mafSetFrame(frame);

  	mafBaseEventHandler* listener = nullptr;

    m_logic = std::make_unique<MainLogic>(listener);

    m_logic->m_frame = frame;

#ifdef __WIN32__
    m_logic->m_frame->DragAcceptFiles(true);
#endif

  	{
      auto statusBar = m_logic->m_frame->CreateStatusBar(1, wxSTB_DEFAULT_STYLE, MENU_VIEW_STATUSBAR_);
      int widths[] = { -1,60,60,60,210,150 }; // enlarged the last tab for the "free mem watch" information
      std::transform(std::begin(widths), std::end(widths), std::begin(widths), [&](int w) {return m_logic->m_frame->FromDIP(w); });
      statusBar->SetFieldsCount(6);
      statusBar->SetStatusWidths(6, widths);
      statusBar->SetStatusText(_("welcome"), 0);
      statusBar->SetStatusText(" ", 1);
      statusBar->SetStatusText(" ", 2);
      statusBar->SetStatusText(" ", 3);
    }

    {
      wxIconBundle ib;
      ib.AddIcon(mafPictureFactory::GetPictureFactory()->GetIcon(_R("FRAME_ICON16x16")));
      ib.AddIcon(mafPictureFactory::GetPictureFactory()->GetIcon(_R("FRAME_ICON32x32")));
      m_logic->m_frame->SetIcons(ib);
    }

    m_logic->m_frame->Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent& e)
      {
        DismissPreferencesEditor();
        e.Skip();
        wxGetApp().m_logic->m_NodeManager.reset();
        wxGetApp().m_logic->m_MaterialChooser.reset();
        // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
        SetGlobalMouse(nullptr);
        wxGetApp().m_logic->m_InteractionManager.reset();
#endif
        wxGetApp().m_logic->m_ViewManager.reset();
        wxGetApp().m_logic->m_OpManager.reset();

        // must be deleted after m_NodeManager
        wxGetApp().m_logic->m_SideBar.reset();
      }
    );

    m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent&) { ShowPreferencesEditor(m_logic->m_frame); }, wxID_PREFERENCES);
    m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent&) { m_logic->m_frame->Close(); }, wxID_EXIT);

    m_logic->m_frame->Bind(wxEVT_MENU, [this](const wxCommandEvent& event) {mafEvent evUnq(this, event.GetId());
    //OnEvent(&evUnq);
      }, MENU_START, MENU_END);
    m_logic->m_frame->Bind(wxEVT_MENU, [this](const wxCommandEvent& event) {mafEvent evUnq(this, event.GetId());
    //OnEvent(&evUnq);
      }, wxID_FILE1, wxID_FILE9);

#ifdef __WIN32__
    m_logic->m_frame->Bind(wxEVT_IDLE, [this](const wxIdleEvent& event) { MEMORYSTATUS ms; GlobalMemoryStatus(&ms); wxString s; s << "free mem " << (ms.dwAvailPhys >> 20) << " MB"; if (m_logic->m_frame->GetStatusBar()) m_logic->m_frame->SetStatusText(s, 5); });
#endif

    m_logic->m_frame->Bind(wxEVT_UPDATE_UI,
      [this](wxUpdateUIEvent& event)
      {
        mafEvent evUnq(this, UPDATE_UI, &event);
        //OnEvent(&evUnq);
      }, MENU_START, MENU_END);

    m_logic->m_frame->Bind(wxEVT_DROP_FILES, [this](const wxDropFilesEvent& event) {});
    //m_Win->SetListener(this);


    if (m_logic->m_PlugMenu)
    {
    	auto menuBar = new wxMenuBar;
      wxMenu* fileMenu = new wxMenu;
      fileMenu->Append(wxID_NEW);
      fileMenu->Append(wxID_OPEN);
      fileMenu->Append(wxID_SAVE);
      fileMenu->Append(wxID_SAVEAS);

      fileMenu->AppendSeparator();

      auto m_ImportMenu = new wxMenu;
      fileMenu->Append(0, _("Import"), m_ImportMenu);
      auto m_ExportMenu = new wxMenu;
      fileMenu->Append(0, _("Export"), m_ExportMenu);

      fileMenu->AppendSeparator();

      fileMenu->Append(wxID_PRINT);
      fileMenu->Append(wxID_PREVIEW);
      fileMenu->Append(wxID_PRINT_SETUP, _("Printer Setup"));
      fileMenu->Append(wxID_PAGE_SETUP, _("Page Setup"));

      fileMenu->AppendSeparator();

      auto m_RecentFileMenu = new wxMenu;
      fileMenu->Append(0, _("Recent Files"), m_RecentFileMenu);

      fileMenu->AppendSeparator();

      fileMenu->Append(wxID_EXIT);
      //if (m_StorageSettings->UseRemoteStorage())
      {
        //file_menu->Append(MENU_FILE_UPLOAD, _("&Upload"));
      }
      menuBar->Append(fileMenu, wxGetStockLabel(wxID_FILE));

    	auto editMenu = new wxMenu;
      editMenu->Append(wxID_UNDO);
      editMenu->Append(wxID_REDO);
      editMenu->AppendSeparator();
      //m_MenuElems.push_back(mafMenuElems(true, 0, undoCommand));
      //m_MenuElems.push_back(mafMenuElems(true, 0, redoCommand));
      editMenu->Append(MENU_EDIT_FIND_VME, _("Find VME \tCtrl+F"));
      menuBar->Append(editMenu, wxGetStockLabel(wxID_EDIT));

    	auto viewMenu = new wxMenu;
      viewMenu->Append(MENU_VIEW_STATUSBAR_, _("Status bar"), wxEmptyString, wxITEM_CHECK);
      menuBar->Append(viewMenu, _("&View"));
      //viewMenu->Append(ID_VIEW_START, _("Add view"));
      //viewMenu->Append(ID_VIEW_END, _("Add external view"));
      //wxMenuItem* layoutItem = viewMenu->Append(wxID_ANY, _("Layout"), layoutMenu);
      viewMenu->Append(MENU_USER_START, _("Add view"));
      viewMenu->Append(MENU_USER_START + 1, _("Add external view"));

      auto opMenu = new wxMenu;
      menuBar->Append(opMenu, _("&Operations"));

      auto toolsMenu = new wxMenu;
      toolsMenu->Append(wxID_PREFERENCES);
      menuBar->Append(toolsMenu, _("Tools"));

      auto helpMenu = new wxMenu;
      helpMenu->Append(wxID_ABOUT);
      //helpMenu->Append(HELP_HOME, _("Help"));
      menuBar->Append(helpMenu, wxGetStockLabel(wxID_HELP));
      m_logic->m_frame->Bind(wxEVT_MENU, &App::OnAbout, this, wxID_ABOUT);
      m_logic->m_frame->SetMenuBar(menuBar);
    }

    m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {auto sb = m_logic->m_frame->GetStatusBar();  if (sb) sb->Show(event.IsChecked()); m_logic->m_frame->GetDockManager().Update(); }, MENU_VIEW_STATUSBAR_);
    m_logic->m_frame->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {auto sb = m_logic->m_frame->GetStatusBar();  event.Check(sb && sb->IsShown()); }, MENU_VIEW_STATUSBAR_);

    m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {auto v = new wxMDIChildFrame(m_logic->m_frame, wxID_ANY, "View"); v->Show(true); }, MENU_USER_START);
    m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {auto v = new wxFrame(m_logic->m_frame, wxID_ANY, "External view"); v->Show(); }, MENU_USER_START + 1);

    if (m_logic->m_PlugToolbar)
    {
      auto toolbar = new wxAuiToolBar(m_logic->m_frame, MENU_VIEW_TOOLBAR_);
      toolbar->AddTool(wxID_NEW, wxGetStockLabel(wxID_NEW, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_NEW), wxGetStockLabel(wxID_NEW, wxSTOCK_WITH_ACCELERATOR));
      toolbar->AddTool(wxID_OPEN, wxGetStockLabel(wxID_OPEN, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN), wxGetStockLabel(wxID_OPEN, wxSTOCK_WITH_ACCELERATOR));
      toolbar->AddTool(wxID_SAVE, wxGetStockLabel(wxID_SAVE, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_FILE_SAVE), wxGetStockLabel(wxID_SAVE, wxSTOCK_WITH_ACCELERATOR));

    	toolbar->AddSeparator();

      toolbar->AddTool(wxID_PRINT, wxGetStockLabel(wxID_PRINT, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_PRINT), wxGetStockLabel(wxID_PRINT, wxSTOCK_WITH_ACCELERATOR));
      toolbar->AddTool(wxID_PREVIEW, wxGetStockLabel(wxID_PREVIEW, wxSTOCK_WITH_ACCELERATOR), mafPictureFactory::GetPictureFactory()->GetBmp(_R("PRINT_PREVIEW")), wxGetStockLabel(wxID_PREVIEW, wxSTOCK_WITH_ACCELERATOR));
      toolbar->AddSeparator();

      toolbar->AddTool(wxID_UNDO, wxGetStockLabel(wxID_OPEN, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_UNDO), wxGetStockLabel(wxID_UNDO, wxSTOCK_WITH_ACCELERATOR));
      toolbar->AddTool(wxID_REDO, wxGetStockLabel(wxID_OPEN, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_REDO), wxGetStockLabel(wxID_REDO, wxSTOCK_WITH_ACCELERATOR));

    	toolbar->AddSeparator();

      toolbar->AddTool(wxID_DELETE, wxGetStockLabel(wxID_DELETE, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_DELETE), wxGetStockLabel(wxID_DELETE, wxSTOCK_WITH_ACCELERATOR));
      toolbar->AddTool(wxID_CUT, wxGetStockLabel(wxID_CUT, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_CUT), wxGetStockLabel(wxID_CUT, wxSTOCK_WITH_ACCELERATOR));
      toolbar->AddTool(wxID_COPY, wxGetStockLabel(wxID_COPY, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_COPY), wxGetStockLabel(wxID_COPY, wxSTOCK_WITH_ACCELERATOR));
      toolbar->AddTool(wxID_PASTE, wxGetStockLabel(wxID_PASTE, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_PASTE), wxGetStockLabel(wxID_PASTE, wxSTOCK_WITH_ACCELERATOR));

    	toolbar->AddSeparator();

      toolbar->AddTool(CAMERA_RESET, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("ZOOM_ALL")), _("reset camera to fit all (ctrl+f)"));
      toolbar->AddTool(CAMERA_FIT, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("ZOOM_SEL")), _("reset camera to fit selected object (ctrl+shift+f)"));
      toolbar->AddTool(CAMERA_FLYTO, wxEmptyString, mafPictureFactory::GetPictureFactory()->GetBmp(_R("FLYTO")), _("fly to object under mouse"));

      //toolbar->AddSeparator();
      //toolbar->AddTool(wxID_ABOUT, wxGetStockLabel(wxID_ABOUT, wxSTOCK_WITH_ACCELERATOR), wxArtProvider::GetBitmapBundle(wxART_INFORMATION), wxGetStockLabel(wxID_ABOUT, wxSTOCK_WITH_ACCELERATOR));
      toolbar->Realize();
      AddPane(*m_logic->m_frame, toolbar, wxAuiPaneInfo().ToolbarPane().Caption("Standard").Name("Standard Toolbar").Top().Floatable(false));
      //delete RemovePane(*m_frame, m_frame->GetDockManager().GetPane(toolbar));
      m_logic->m_frame->Bind(wxEVT_MENU, [&, toolbar](wxCommandEvent& event) {m_logic->m_frame->GetDockManager().GetPane(wxWindow::FindWindowById(event.GetId())).Show(event.IsChecked()); m_logic->m_frame->GetDockManager().Update(); }, MENU_VIEW_TOOLBAR_);
      m_logic->m_frame->Bind(wxEVT_UPDATE_UI, [&, toolbar](wxUpdateUIEvent& event) {event.Check(m_logic->m_frame->GetDockManager().GetPane(wxWindow::FindWindowById(event.GetId())).IsShown()); }, MENU_VIEW_TOOLBAR_);
    }

    if (m_logic->m_PlugTimebar)
    {
      auto timePanel = new mafGUITimeBar(m_logic->m_frame, MENU_VIEW_TIMEBAR_, true);
      AddPane(*m_logic->m_frame, timePanel, wxAuiPaneInfo().Caption("TimeBar").Name("timebar").Bottom()
        .Row(1)
        .Layer(2)
        .ToolbarPane()
        .LeftDockable(false)
        .RightDockable(false)
        .MinSize(100, 22)
        .Floatable(false)
        .Gripper(false)
        .Resizable(false)
        .Movable(false));
      m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {m_logic->m_frame->GetDockManager().GetPane(wxWindow::FindWindowById(event.GetId())).Show(event.IsChecked()); m_logic->m_frame->GetDockManager().Update(); }, MENU_VIEW_TIMEBAR_);
      m_logic->m_frame->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {event.Check(m_logic->m_frame->GetDockManager().GetPane(wxWindow::FindWindowById(event.GetId())).IsShown()); }, MENU_VIEW_TIMEBAR_);
      timePanel->SetListener(listener);

      // Events coming from settings are forwarded to the time bar.
      timePanel->SetTimeSettings(m_logic->m_TimeBarSettings.get());
      m_logic->m_TimeBarSettings->SetListener(timePanel);
    }

    {
		  auto logToFile = m_logic->m_ApplicationSettings->GetLogToFileStatus();
    	auto logAllEvents = m_logic->m_ApplicationSettings->GetLogVerboseStatus();
      if (m_logic->m_PlugLogbar)
      {
#ifdef MAF_USE_VTK
        vtkNew<mafVTKLog> m_VtkLog;
        vtkOutputWindow::SetInstance(m_VtkLog);
#endif
        wxTextCtrl* log = new wxTextCtrl(m_logic->m_frame, MENU_VIEW_LOGBAR_, "", wxPoint(0, 0), wxSize(100, 300), /*wxNO_BORDER |*/ wxTE_MULTILINE);
        auto m_Logger = new mafWXLog(log);
        m_Logger->LogToFile(logToFile);
        if (logToFile)
        {
          mafString s = m_logic->m_ApplicationSettings->GetLogFolder();
          wxDateTime log_time = wxDateTime::Now();
          s += _R("\\");
          s += mafWxToString(m_logic->m_frame->GetTitle());
          s += mafString::Format(_R("_%02d_%02d_%d_%02d_%2d"), log_time.GetYear(), log_time.GetMonth() + 1, log_time.GetDay(), log_time.GetHour(), log_time.GetMinute());
          s += _R(".log");
          if (m_Logger->SetFileName(s.toWx()) == MAF_ERROR)
          {
            mafLogMessage(_M(_R("Unable to create log file ") + s));
          }
        }
        m_Logger->SetVerbose(logAllEvents);

        wxLog* old_log = wxLog::SetActiveTarget(m_Logger);
        cppDEL(old_log);

        AddPane(*m_logic->m_frame, log, wxAuiPaneInfo().Caption("LogBar").Name("logbar").Bottom()
          .Bottom()
          .Layer(0)
          .MinSize(100, 10)
          .TopDockable(false) // prevent docking on top side - otherwise may dock also beside the toolbar -- and it's hugely
        );
        m_logic->m_frame->Bind(wxEVT_MENU, [&](wxCommandEvent& event) {m_logic->m_frame->GetDockManager().GetPane(wxWindow::FindWindowById(event.GetId())).Show(event.IsChecked()); m_logic->m_frame->GetDockManager().Update(); }, MENU_VIEW_LOGBAR_);
        m_logic->m_frame->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {event.Check(m_logic->m_frame->GetDockManager().GetPane(wxWindow::FindWindowById(event.GetId())).IsShown()); }, MENU_VIEW_LOGBAR_);

        //mafLogMessage(_M(mafString(_L("welcome"))));
      }
      else
      {
#ifdef MAF_USE_VTK
        vtkNew<mafVTKLog> m_VtkLog;
        vtkOutputWindow::SetInstance(m_VtkLog);
#endif  
        wxTextCtrl* log = new wxTextCtrl(m_logic->m_frame, -1, "", wxPoint(0, 0), wxSize(100, 300), wxNO_BORDER | wxTE_MULTILINE);
        auto m_Logger = new mafWXLog(log);
        log->Show(false);
        wxLog* old_log = wxLog::SetActiveTarget(m_Logger);
        cppDEL(old_log);
      }
    }

    m_logic->m_frame->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {event.Enable(false); }, CAMERA_RESET);
    m_logic->m_frame->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {event.Enable(false); }, CAMERA_FIT);
    m_logic->m_frame->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) {event.Enable(false); }, CAMERA_FLYTO);

    if (m_logic->m_PlugSidebar)
    {
      m_logic->m_SideBar = std::make_unique<mafSideBar>(m_logic->m_frame, MENU_VIEW_SIDEBAR_, nullptr, mafSideBar::DOUBLE_NOTEBOOK);
      AddPane(*m_logic->m_frame, m_logic->m_SideBar->m_Notebook, wxAuiPaneInfo()
        .Name("sidebar")
        .Caption(wxT("ControlBar"))
        .Right()
        .Layer(2)
        .MinSize(wxWindow::FromDIP(wxSize(240, 450), m_logic->m_frame))
        .TopDockable(false)
        .BottomDockable(false)
      );
    }

    m_logic->m_frame->GetDockManager().Update();

    {
      m_logic->m_NodeManager = std::make_unique<mafNodeManager>();
      m_logic->m_NodeManager->SetListener(listener);
    }

    // currently mafInteraction is strictly dependent on VTK (marco)
#ifdef MAF_USE_VTK
    {
      m_logic->m_InteractionManager = std::make_unique<mafInteractionManager>();
      m_logic->m_InteractionManager->SetListener(listener);
#ifdef MAF_USE_CURL
      mafPlugDevice<mmdRemoteFileManager>("mmdRemoteFileManager");
#endif

      SetGlobalMouse(m_logic->m_InteractionManager->GetMouseDevice());
      //SIL m_InteractionManager->GetClientDevice()->AddObserver(this, MCH_INPUT);
    }
#endif

    {
      m_logic->m_ViewManager = std::make_unique<mafViewManager>();
      m_logic->m_ViewManager->SetListener(listener);
    }

    {
      m_logic->m_OpManager = std::make_unique<mafOpManager>();
      m_logic->m_OpManager->SetListener(listener);
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
      m_logic->m_ApplicationLayoutSettings = std::make_unique<mafGUIApplicationLayoutSettings>(listener);
      m_logic->m_ApplicationLayoutSettings->SetViewManager(m_logic->m_ViewManager.get());
      m_logic->m_ApplicationLayoutSettings->SetApplicationFrame(m_logic->m_frame);
      m_logic->m_SettingsDialog->AddPage(m_logic->m_ApplicationLayoutSettings->GetGui(), m_logic->m_ApplicationLayoutSettings->GetLabel());
    }

    m_logic->m_SettingsDialog->AddPage(m_logic->m_frame->GetDockSettingGui(), _("User Interface Preferences"));

    m_logic->m_HelpSettings = std::make_unique<mafGUISettingsHelp>(listener);
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



    if (m_logic->m_UseWizardManager)
    {
      //CreateWizardToolbar();
      m_logic->m_WizardManager = new medWizardManager();
      m_logic->m_WizardManager->SetListener(listener);
      //m_logic->m_WizardManager->WarningIfCantUndo(m_logic->m_ApplicationSettings->GetWarnUserFlag());
    }


    m_logic->m_frame->Show();

    return true;
  }

  int  App::OnExit()
  {
    return wxApp::OnExit();
  }

  void App::OnAbout(wxCommandEvent& e)
  {
    wxAboutDialogInfo info;
    info.SetName("FTK Demo App");
    info.SetCopyright("(C) 2025 FRAMOSOFT");
    info.SetVersion("v1.0");
    //info.SetWebSite("framosoft.com");
    info.AddDeveloper("Fedor Moiseev");
    wxAboutBox(info);
  }

  void App::ShowPreferencesEditor(wxWindow* parent)
  {
    if (!m_prefEditor)
    {
      m_prefEditor = std::make_unique<wxPreferencesEditor>();
      m_prefEditor->AddPage(new PrefsPageGeneral());
      m_prefEditor->AddPage(new PrefsPageTopics());
    }

    m_prefEditor->Show(parent);
  }

  void App::DismissPreferencesEditor()
  {
    if (m_prefEditor)
      m_prefEditor->Dismiss();
  }

  void App::UpdateSettings(const MySettings& settings)
  {
    m_settings = settings;
    //m_frame->UpdateSettings();
  }
  wxIMPLEMENT_APP(App);
