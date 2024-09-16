#pragma once

#include "ftkConfigure.h"

#include <wx/laywin.h>
#include <wx/mdi.h>
#include <wx/aui/aui.h>

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

#include <memory>

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class mafGUIMDIFrameCallback;
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif //MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

BEGIN_FTK_NAMESPACE

template <class BaseFrame, long DefaultStyle>
class MainFrame: public BaseFrame
{
 public:

	MainFrame();

	MainFrame(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = DefaultStyle, const wxString& name = wxASCII_STR(wxFrameNameStr));

  ~MainFrame() override;

  bool Create(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = DefaultStyle, const wxString& name = wxASCII_STR(wxFrameNameStr));

	mafGUI* GetDockSettingGui() {return m_DockSettings->GetGui();}

  wxAuiManager& GetDockManager()  {return m_auiManager;}

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

  void ProgressBarSetText(wxString* msg);

  void RenderStart();

  void RenderEnd();

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

  void BindToProgressBar(vtkObject* vtkobj);

  void BindToProgressBar(vtkAlgorithm* filter);

  void BindToProgressBar(vtkViewport* ren);

protected:
  mafGUIMDIFrameCallback *m_StartCallback; 
  mafGUIMDIFrameCallback *m_EndCallback; ; 
  mafGUIMDIFrameCallback *m_ProgressCallback; 

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif  //MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
protected:
  bool         *m_Busy;
  wxGauge      *m_Gauge;
  std::vector<int>    m_PBCalls;
  int  m_ID_PBCall;

  wxAuiManager    m_auiManager;
  std::unique_ptr<mafGUIDockSettings> m_DockSettings;
private:
   void CreateControls();
   void CreateStatus();

};

template <class BaseFrame, long DefaultStyle>
MainFrame<BaseFrame, DefaultStyle>::MainFrame() = default;

template <class BaseFrame, long DefaultStyle>
MainFrame<BaseFrame, DefaultStyle>::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: MainFrame()
{
  Create(title, pos, size, style, name);
}

template <class BaseFrame, long DefaultStyle>
MainFrame<BaseFrame, DefaultStyle>::~MainFrame()
{
#ifdef MAF_USE_VTK
  vtkDEL(m_ProgressCallback);
  vtkDEL(m_StartCallback);
  vtkDEL(m_EndCallback);
#endif 

  m_PBCalls.clear();
  mafSetFrame(nullptr);
}

template <class BaseFrame, long DefaultStyle>
bool MainFrame<BaseFrame, DefaultStyle>::Create(const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
  if (!BaseFrame::Create(nullptr, wxID_ANY, title, pos, size, style, name))
  {
    return false;
  }

  mafSetFrame(this);
  CreateControls();

  m_auiManager.SetManagedWindow(this);
  m_auiManager.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
  m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR));
  m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR));
  m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR));
  m_auiManager.Update();
  m_DockSettings = std::make_unique<mafGUIDockSettings>(m_auiManager);

  m_ID_PBCall = 0;
  m_PBCalls.clear();

  BaseFrame::Centre();

#ifdef __WIN32__
  DragAcceptFiles(true);
#endif

  wxIconBundle ib;
  ib.AddIcon(mafPictureFactory::GetPictureFactory()->GetIcon(_R("FRAME_ICON16x16")));
  ib.AddIcon(mafPictureFactory::GetPictureFactory()->GetIcon(_R("FRAME_ICON32x32")));
  BaseFrame::SetIcons(ib);

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
  return true;
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::CreateControls()
{
  CreateStatus();
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::CreateStatus()
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
bool MainFrame<BaseFrame, DefaultStyle>::AddPane(wxWindow* window, wxAuiPaneInfo& pane_info)
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
    BaseFrame::Bind(wxEVT_MENU, &MainFrame::OnSwitchPane, this, window->GetId(), window->GetId());
    BaseFrame::Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdatePaneUI, this, window->GetId(), window->GetId());
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
    BaseFrame::Bind(wxEVT_MENU, &MainFrame::OnSwitchPane, this, window->GetId(), window->GetId());
    BaseFrame::Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdatePaneUI, this, window->GetId(), window->GetId());
  }
  return true;
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::OnSwitchPane(wxCommandEvent& event)
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
void MainFrame<BaseFrame, DefaultStyle>::OnUpdatePaneUI(wxUpdateUIEvent& event)
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
wxWindow* MainFrame<BaseFrame, DefaultStyle>::RemovePane(const wxString& pane_name)
{
  wxAuiPaneInfo& pi = m_auiManager.GetPane(pane_name);
  if (pi.IsOk())
  {
    return RemovePane(pi);
  }
  return nullptr;
}

template <class BaseFrame, long DefaultStyle>
wxWindow* MainFrame<BaseFrame, DefaultStyle>::RemovePane(wxWindow* window)
{
  wxAuiPaneInfo& pi = m_auiManager.GetPane(window);
  if (pi.IsOk())
  {
    return RemovePane(pi);
  }
  return nullptr;
}

template <class BaseFrame, long DefaultStyle>
wxWindow* MainFrame<BaseFrame, DefaultStyle>::RemovePane(wxAuiPaneInfo& pane_info)
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
      BaseFrame::Unbind(wxEVT_MENU, &MainFrame::OnSwitchPane, this, pane_info.window->GetId(), pane_info.window->GetId());
      BaseFrame::Unbind(wxEVT_UPDATE_UI, &MainFrame::OnUpdatePaneUI, this, pane_info.window->GetId(), pane_info.window->GetId());
      delete toolbarsMenu->Remove(pane_info.window->GetId());
      if (toolbarsMenu->GetMenuItemCount() == 0)
      {
        delete viewMenu->Remove(sm);
      }
    }
    else
    {
      BaseFrame::Unbind(wxEVT_MENU, &MainFrame::OnSwitchPane, this, pane_info.window->GetId(), pane_info.window->GetId());
      BaseFrame::Unbind(wxEVT_UPDATE_UI, &MainFrame::OnUpdatePaneUI, this, pane_info.window->GetId(), pane_info.window->GetId());
      delete viewMenu->Remove(pane_info.window->GetId());
    }
  }
  wxWindow* w = pane_info.window;
  if (!m_auiManager.DetachPane(pane_info.window))
    return nullptr;
  return w;
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::ShowPane(const wxString& pane_name, bool show)
{
  wxAuiPaneInfo& pi = m_auiManager.GetPane(pane_name);
  if (pi.IsOk())
  {
    pi.Show(show);
    m_auiManager.Update();
  }
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::ShowPane(wxWindow* window, bool show)
{
  ShowPane(m_auiManager.GetPane(window), show);
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::ShowPane(wxAuiPaneInfo& pane_info, bool show)
{
  if (pane_info.IsOk())
  {
    pane_info.Show(show);
    m_auiManager.Update(); // sync the MenuItems check 
  }
}

template <class BaseFrame, long DefaultStyle>
bool MainFrame<BaseFrame, DefaultStyle>::IsPaneShown(const wxString& pane_name)
{
  return IsPaneShown(m_auiManager.GetPane(pane_name));
}

template <class BaseFrame, long DefaultStyle>
bool MainFrame<BaseFrame, DefaultStyle>::IsPaneShown(wxWindow* window)
{
  return IsPaneShown(m_auiManager.GetPane(window));
}

template <class BaseFrame, long DefaultStyle>
bool MainFrame<BaseFrame, DefaultStyle>::IsPaneShown(const wxAuiPaneInfo& pane_info)
{
  return pane_info.IsOk() && pane_info.IsShown();
 
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::Busy()
{
  BaseFrame::SetStatusText("Busy", 2);
  BaseFrame::SetStatusText("", 3);
  m_Gauge->Show(true);
  m_Gauge->SetValue(0);
  BaseFrame::Refresh(false);
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::Ready()
{
  BaseFrame::SetStatusText("", 2);
  BaseFrame::SetStatusText("", 3);
  m_Gauge->Show(false);
  BaseFrame::Refresh(false);
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::ProgressBarShow()
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
void MainFrame<BaseFrame, DefaultStyle>::ProgressBarHide()
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
void MainFrame<BaseFrame, DefaultStyle>::ProgressBarSetVal(int progress)
{
  if (!m_PBCalls.empty() && m_PBCalls.back() == m_ID_PBCall)
  {
    m_Gauge->SetValue(progress);
    BaseFrame::SetStatusText(wxString::Format(" %d%% ", progress), 3);
    mafYield(); //fix on bug #2082
  }
}

template <class BaseFrame, long DefaultStyle>
int MainFrame<BaseFrame, DefaultStyle>::ProgressBarGetStatus()
{
  return m_Gauge->GetValue();
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::ProgressBarSetText(wxString* msg)
{
  if (msg) BaseFrame::SetStatusText(*msg, 0);
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::RenderStart()
{
  BaseFrame::SetStatusText("Rendering", 1);
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::RenderEnd()
{
  BaseFrame::SetStatusText(" ", 1);
}

using mafGUIMDIFrame = MainFrame<wxMDIParentFrame, wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL>;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_VTK
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
        { wxString s = po->GetProgressText(); frame->ProgressBarSetText(&s); }
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
void MainFrame<BaseFrame, DefaultStyle>::BindToProgressBar(vtkObject* vtkobj)
{
  if (vtkobj->IsA("vtkViewport"))
    BindToProgressBar((vtkViewport*)vtkobj);
  else if (vtkobj->IsA("vtkAlgorithm"))
    BindToProgressBar((vtkAlgorithm*)vtkobj);
  else
    mafLogMessage(_M("wrong vtkObject passed to BindToProgressBar"));
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::BindToProgressBar(vtkAlgorithm* filter)
{
  filter->AddObserver(vtkCommand::ProgressEvent, m_ProgressCallback);
  filter->AddObserver(vtkCommand::StartEvent, m_StartCallback);
  filter->AddObserver(vtkCommand::EndEvent, m_EndCallback);
}

template <class BaseFrame, long DefaultStyle>
void MainFrame<BaseFrame, DefaultStyle>::BindToProgressBar(vtkViewport* ren)
{
  ren->AddObserver(vtkCommand::StartEvent, m_StartCallback);
  ren->AddObserver(vtkCommand::EndEvent, m_EndCallback);
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif  //MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

END_FTK_NAMESPACE
