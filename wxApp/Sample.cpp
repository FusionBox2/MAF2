// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include <vtkSmartPointer.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkWin32OpenGLRenderWindow.h"
#include "vtkHardwareWindow.h"
#include "vtkStringArray.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkRenderWindowInteractor.h"
#include "vtksys/Encoding.hxx"

class wxVTKRenderWindowInteractor_ : public vtkRenderWindowInteractor
{
public:
  static wxVTKRenderWindowInteractor_* New();

  vtkTypeMacro(wxVTKRenderWindowInteractor_, vtkRenderWindowInteractor);

  void SetWxWindow(wxWindow* wnd);

	void PrintSelf(ostream& os, vtkIndent indent) override;

  void Initialize() override;

  void Enable() override;
  void Disable() override;

  void ProcessEvents() override;

	virtual void OnEnterWindow(wxMouseEvent& event);
  virtual void OnLeaveWindow(wxMouseEvent& event);
  virtual void OnMouseMove(wxMouseEvent& event);
  virtual void OnRButtonDown(wxMouseEvent& event);
  virtual void OnRButtonUp(wxMouseEvent& event);
  virtual void OnMButtonDown(wxMouseEvent& event);
  virtual void OnMButtonUp(wxMouseEvent& event);
  virtual void OnLButtonDown(wxMouseEvent& event);
  virtual void OnLButtonUp(wxMouseEvent& event);
  virtual void OnSize(wxSizeEvent& event);
  virtual void OnTimer(wxTimerEvent &event);
  virtual void OnKeyDown(wxKeyEvent &event);
  virtual void OnKeyUp(wxKeyEvent &event);
  virtual void OnChar(wxKeyEvent &event);
  virtual void OnMouseWheel(wxMouseEvent &event);
  virtual void OnSetFocus(wxFocusEvent& event);
  virtual void OnKillFocus(wxFocusEvent& event);

protected:
  wxVTKRenderWindowInteractor_();
  ~wxVTKRenderWindowInteractor_() override;

  wxWindow *Window;
  int MouseInWindow;
  std::map<int, wxTimer> timers;

  int InternalCreateTimer(int timerId, int timerType, unsigned long duration) override;
  int InternalDestroyTimer(int platformTimerId) override;

  void StartEventLoop() override;

private:
  wxVTKRenderWindowInteractor_(const wxVTKRenderWindowInteractor_&) = delete;
  wxVTKRenderWindowInteractor_& operator=(const wxVTKRenderWindowInteractor_&) = delete;
};

vtkStandardNewMacro(wxVTKRenderWindowInteractor_);

wxVTKRenderWindowInteractor_::wxVTKRenderWindowInteractor_()
{
  this->Window = nullptr;
  this->MouseInWindow = 0;
}

wxVTKRenderWindowInteractor_::~wxVTKRenderWindowInteractor_()
{
  this->Enabled = 0;
}

void wxVTKRenderWindowInteractor_::ProcessEvents()
{
}

void wxVTKRenderWindowInteractor_::StartEventLoop()
{
}

void wxVTKRenderWindowInteractor_::Initialize()
{
  // make sure we have a RenderWindow and camera
  if (!this->RenderWindow)
  {
    vtkErrorMacro(<< "No renderer defined!");
    return;
  }
  if (!this->Window)
  {
    vtkErrorMacro(<< "No main window defined!");
    return;
  }
  if (this->Initialized)
  {
    return;
  }
  this->Initialized = 1;
  // get the info we need from the RenderingWindow
  vtkRenderWindow* ren = this->RenderWindow;
  ren->Start();
  ren->End();
  int* size = ren->GetSize();
  ren->GetPosition();

  this->Enable();
  this->Size[0] = size[0];
  this->Size[1] = size[1];
}

void wxVTKRenderWindowInteractor_::SetWxWindow(wxWindow* wnd)
{
  this->Window = wnd;
}

void wxVTKRenderWindowInteractor_::Enable()
{
  if (this->Enabled)
  {
    return;
  }

	this->Enabled = 1;
  this->Modified();
}

void wxVTKRenderWindowInteractor_::Disable()
{
  if (!this->Enabled)
  {
    return;
  }

  this->Enabled = 0;
  this->Modified();
}

int wxVTKRenderWindowInteractor_::InternalCreateTimer(
  int timerId, int timerType, unsigned long duration)
{
  auto& timer = timers[timerId];
  timer.SetOwner(this->Window, timerId);
  if (!timer.Start(duration, timerType == OneShotTimer))
  {
    return 0;
  }
  return timerId;
}

int wxVTKRenderWindowInteractor_::InternalDestroyTimer(int platformTimerId)
{
  if (auto timerIt = timers.find(platformTimerId); timerIt != end(timers))
  {
    timerIt->second.Stop();
    timers.erase(timerIt);
    return 1;
  }
  return 0;
}

void wxVTKRenderWindowInteractor_::OnEnterWindow(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  this->SetAltKey(event.AltDown());
  if (!this->MouseInWindow)
  {
    event.Skip(this->InvokeEvent(vtkCommand::EnterEvent, nullptr) == 0);
    this->MouseInWindow = 1;
  }
}

void wxVTKRenderWindowInteractor_::OnLeaveWindow(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  this->SetAltKey(event.AltDown());
  if (this->MouseInWindow)
  {
    event.Skip(this->InvokeEvent(vtkCommand::LeaveEvent, nullptr) == 0);
    this->MouseInWindow = 0;
  }
}

void wxVTKRenderWindowInteractor_::OnMouseMove(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  auto X = event.GetX();
  auto Y = event.GetY();
  this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  this->SetAltKey(event.AltDown());
  event.Skip(this->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr) == 0);
}

void wxVTKRenderWindowInteractor_::OnMouseWheel(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown());
  this->SetAltKey(event.AltDown());
  if (event.GetWheelRotation() > 0)
  {
    event.Skip(this->InvokeEvent(vtkCommand::MouseWheelForwardEvent, nullptr) == 0);
  }
  else
  {
    event.Skip(this->InvokeEvent(vtkCommand::MouseWheelBackwardEvent, nullptr) == 0);
  }
}

void wxVTKRenderWindowInteractor_::OnLButtonDown(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  this->Window->SetFocus();
  this->Window->CaptureMouse();
  this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  this->SetAltKey(event.AltDown());
  event.Skip(this->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr) == 0);
}

void wxVTKRenderWindowInteractor_::OnLButtonUp(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  this->SetAltKey(event.AltDown());
  event.Skip(this->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, nullptr) == 0);
  this->Window->ReleaseMouse();
  event.Skip(false);
}

void wxVTKRenderWindowInteractor_::OnMButtonDown(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  this->Window->SetFocus();
  this->Window->CaptureMouse();
  this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  this->SetAltKey(event.AltDown());
  event.Skip(this->InvokeEvent(vtkCommand::MiddleButtonPressEvent, nullptr) == 0);
}

void wxVTKRenderWindowInteractor_::OnMButtonUp(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  this->SetAltKey(event.AltDown());
  event.Skip(this->InvokeEvent(vtkCommand::MiddleButtonReleaseEvent, nullptr) == 0);
  this->Window->ReleaseMouse();
}

void wxVTKRenderWindowInteractor_::OnRButtonDown(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  this->Window->SetFocus();
  this->Window->CaptureMouse();
  this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  this->SetAltKey(event.AltDown());
  event.Skip(this->InvokeEvent(vtkCommand::RightButtonPressEvent, nullptr) == 0);
}

void wxVTKRenderWindowInteractor_::OnRButtonUp(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  this->SetAltKey(event.AltDown());
  event.Skip(this->InvokeEvent(vtkCommand::RightButtonReleaseEvent, nullptr) == 0);
  this->Window->ReleaseMouse();
}

void wxVTKRenderWindowInteractor_::OnSize(wxSizeEvent& event)
{
  this->UpdateSize(event.GetSize().GetWidth(), event.GetSize().GetHeight());
  if (this->Enabled)
  {
    event.Skip(this->InvokeEvent(vtkCommand::ConfigureEvent, nullptr) == 0);
  }
}

void wxVTKRenderWindowInteractor_::OnTimer(wxTimerEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }
  int tid = static_cast<int>(event.GetTimer().GetId());
  event.Skip(this->InvokeEvent(vtkCommand::TimerEvent, (void*)&tid) == 0);
}

void wxVTKRenderWindowInteractor_::OnKeyDown(wxKeyEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }
  this->SetKeyEventInformation(event.ControlDown(), event.ShiftDown(), event.GetKeyCode(), 1, "none");
  this->SetAltKey(event.AltDown());
  event.Skip(this->InvokeEvent(vtkCommand::KeyPressEvent, nullptr) == 0);
}

void wxVTKRenderWindowInteractor_::OnKeyUp(wxKeyEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }
  this->SetKeyEventInformation(event.ControlDown(), event.ShiftDown(), event.GetKeyCode(), 1, "none");
  this->SetAltKey(event.AltDown());
  event.Skip(this->InvokeEvent(vtkCommand::KeyReleaseEvent, nullptr) == 0);
}

void wxVTKRenderWindowInteractor_::OnChar(wxKeyEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }
  this->SetKeyEventInformation(event.ControlDown(), event.ShiftDown(), event.GetKeyCode(), 1, "none");
  this->SetAltKey(event.AltDown());
  event.Skip(this->InvokeEvent(vtkCommand::CharEvent, nullptr) == 0);
}

void wxVTKRenderWindowInteractor_::OnSetFocus(wxFocusEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }
	event.Skip(true);
}

void wxVTKRenderWindowInteractor_::OnKillFocus(wxFocusEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }
  event.Skip(true);
}

void wxVTKRenderWindowInteractor_::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

class wxVTKWindow : public wxWindow
{
public:
  DECLARE_DYNAMIC_CLASS(wxVTKWindow)

  wxVTKWindow();

  wxVTKWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxASCII_STR(wxPanelNameStr));

  virtual ~wxVTKWindow();

  bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxASCII_STR(wxPanelNameStr));

  virtual void SetRenderWindow(vtkRenderWindow*);

  virtual vtkRenderWindow* GetRenderWindow();

  virtual vtkRenderWindowInteractor* GetInteractor();

protected:
  void OnSize(wxSizeEvent& event);

	void OnPaint(wxPaintEvent& event);

	void OnEraseBackground(wxEraseEvent& event);

  void OnDestroy(wxWindowDestroyEvent& event);

	vtkSmartPointer<vtkRenderWindow> m_renderWindow;
};

IMPLEMENT_DYNAMIC_CLASS(wxVTKWindow, wxWindow)

wxVTKWindow::wxVTKWindow() = default;

wxVTKWindow::wxVTKWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
  Create(parent, id, pos, size, style, name);
}

wxVTKWindow::~wxVTKWindow()
{
  this->SetRenderWindow(nullptr);
}

bool wxVTKWindow::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
  Bind(wxEVT_SIZE, &wxVTKWindow::OnSize, this);
  Bind(wxEVT_PAINT, &wxVTKWindow::OnPaint, this);
  Bind(wxEVT_ERASE_BACKGROUND, &wxVTKWindow::OnEraseBackground, this);
  Bind(wxEVT_DESTROY, &wxVTKWindow::OnDestroy, this);

  Bind(wxEVT_MOUSE_CAPTURE_LOST, [this](wxMouseCaptureLostEvent&) {});

  Bind(wxEVT_TIMER, [this](wxTimerEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnTimer(event); });

  Bind(wxEVT_LEFT_DCLICK, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) { SetFocus(); CaptureMouse(); } });
  Bind(wxEVT_RIGHT_DCLICK, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) { SetFocus(); CaptureMouse(); } });
  Bind(wxEVT_MIDDLE_DCLICK, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) { SetFocus(); CaptureMouse(); } });

	Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& event) {SetFocus(); if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnLButtonDown(event);});
  Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnLButtonUp(event); });
  Bind(wxEVT_RIGHT_DOWN, [this](wxMouseEvent& event) {SetFocus(); if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnRButtonDown(event); });
  Bind(wxEVT_RIGHT_UP, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnRButtonUp(event); });
  Bind(wxEVT_MIDDLE_DOWN, [this](wxMouseEvent& event) {SetFocus(); if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnMButtonDown(event); });
  Bind(wxEVT_MIDDLE_UP, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnMButtonUp(event); });
  Bind(wxEVT_MOTION, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnMouseMove(event); });
  Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnKeyDown(event); });
  Bind(wxEVT_KEY_UP, [this](wxKeyEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnKeyUp(event); });
  Bind(wxEVT_CHAR, [this](wxKeyEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnChar(event); });
  Bind(wxEVT_MOUSEWHEEL, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnMouseWheel(event); });
  Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnEnterWindow(event); });
  Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnLeaveWindow(event); });
  Bind(wxEVT_SET_FOCUS, [this](wxFocusEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnSetFocus(event); });
  Bind(wxEVT_KILL_FOCUS, [this](wxFocusEvent& event) {if (auto iren = static_cast<wxVTKRenderWindowInteractor_*>(GetInteractor())) iren->OnKillFocus(event); });

  if (!wxWindow::Create(parent, id, pos, size, style, name))
    return false;
#if wxCHECK_VERSION(3,3,0)
#if __WXMSW__
  MSWDisableComposited();
#endif
#endif
  vtkNew<vtkRenderWindow> win;
  this->SetRenderWindow(win);
  return true;
}

void wxVTKWindow::OnDestroy(wxWindowDestroyEvent& event)
{
  this->SetRenderWindow(nullptr);
}

void wxVTKWindow::SetRenderWindow(vtkRenderWindow* win)
{
  if (this->m_renderWindow)
  {
    if (this->m_renderWindow->GetMapped())
      this->m_renderWindow->Finalize();
    this->m_renderWindow->UnRegister(nullptr);
  }

  this->m_renderWindow = win;

  if (this->m_renderWindow)
  {
    this->m_renderWindow->Register(nullptr);

    // setup the parent window
    this->m_renderWindow->SetWindowId(this->GetHandle());
    this->m_renderWindow->SetParentId(GetParent() ? GetParent()->GetHandle() : nullptr);
    //((vtkWin32OpenGLRenderWindow*)this->m_renderWindow.Get())->InitializeApplication();
    //((vtkWin32OpenGLRenderWindow*)this->m_renderWindow.Get())->SetDeviceContext(GetDC(GetHWND()));
    //this->m_renderWindow->SetDisplayId(GetDC(GetHWND()));

    // update size
    wxRect cRect(0, 0, 1, 1);
    if (this->GetParent())
      cRect = this->GetParent()->GetClientRect();

    vtkSmartPointer<wxVTKRenderWindowInteractor_> iren;
    iren = vtkNew<wxVTKRenderWindowInteractor_>();
    if (iren)
    {
      iren->SetWxWindow(this);
      //vtkNew<vtkInteractorStyleTrackballCamera> style;
      //iren->SetInteractorStyle(style);
      iren->SetRenderWindow(this->m_renderWindow);
      iren->Initialize();
      if (iren->GetInitialized())
      {
        iren->UpdateSize(cRect.GetWidth(), cRect.GetHeight());
      }
    }
    else
    {
      this->m_renderWindow->Initialize();
      this->m_renderWindow->SetSize(cRect.GetWidth(), cRect.GetHeight());
    }
  }
}

vtkRenderWindow* wxVTKWindow::GetRenderWindow()
{
  return this->m_renderWindow;
}

vtkRenderWindowInteractor* wxVTKWindow::GetInteractor()
{
  if (!this->m_renderWindow)
  {
    return nullptr;
  }
  return this->m_renderWindow->GetInteractor();
}

void wxVTKWindow::OnPaint(wxPaintEvent& event)
{
  wxPaintDC pDC(this);
  if (auto iren = this->GetInteractor(); iren && iren->GetInitialized())
  {
    iren->Render();
  }
  else if(auto rw = this->GetRenderWindow())
  {
    rw->Render();
  }
}

void wxVTKWindow::OnEraseBackground(wxEraseEvent& event)
{
  event.Skip(false);
}

void wxVTKWindow::OnSize(wxSizeEvent& event)
{
  if (auto iren = this->GetInteractor(); iren && iren->GetInitialized())
  {
	  iren->UpdateSize(event.GetSize().GetWidth(), event.GetSize().GetHeight());
  }
  else if (auto rw = this->GetRenderWindow())
  {
    rw->SetSize(event.GetSize().GetWidth(), event.GetSize().GetHeight());
  }
}

// the application icon
#ifndef __WXMSW__
    #include "mondrian.xpm"
#endif

class MyApp;
class MyFrame;

#if 1
//using MyRenderWnd = wxVTKRenderWindowInteractor_;
using MyRenderWnd = wxVTKWindow;
#else
using MyRenderWnd = mafRWIBase;
#endif
// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
	virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
  MyRenderWnd *m_pVTKWindow = nullptr;

  // vtk classes
  vtkSmartPointer<vtkRenderer> pRenderer;
  vtkSmartPointer<vtkRenderWindow> pRenderWindow;
  vtkSmartPointer<vtkPolyDataMapper> pConeMapper;
  vtkSmartPointer<vtkActor> pConeActor;
  vtkSmartPointer<vtkConeSource> pConeSource;

	// any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()
};

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = 1,
    Minimal_About
};

#define MY_FRAME      101
#define MY_VTK_WINDOW 102

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // create the main application window
    MyFrame *frame = new MyFrame(_T("wxWidgets-VTK App"),
      wxWindow::FromDIP(wxPoint(50, 50), nullptr), wxWindow::FromDIP(wxSize(450, 340), nullptr));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);
    frame->Layout();

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
#ifdef __WXMAC__
    // we need this in order to allow the about menu relocation, since ABOUT is
    // not the default id of the about menu
    wxApp::s_macAboutMenuItemId = Minimal_About;
#endif

    // set the frame icon
    SetIcon(wxICON(mondrian));

    // create a menu bar
    wxMenu *menuFile = new wxMenu(_T(""), wxMENU_TEAROFF);

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tCtrl-A"), _T("Show about dialog"));

    menuFile->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if erwxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Drag the mouse here! (wxWidgets 3.0.3)"));
#endif // wxUSE_STATUSBAR
    m_pVTKWindow = new MyRenderWnd;
	  m_pVTKWindow->Create(this, MY_VTK_WINDOW);

    pRenderer = vtkNew<vtkRenderer>();
    pConeMapper = vtkNew<vtkPolyDataMapper>();
    pConeActor = vtkNew<vtkActor>();
    pConeSource = vtkNew<vtkConeSource>();

    // connect the render window and wxVTK window
    auto pRenderWindow = m_pVTKWindow->GetRenderWindow();
    // connect renderer and render window and configure render window
    pRenderWindow->AddRenderer(pRenderer);
    // initialize cone
    pConeSource->SetResolution(200);
    // connect pipeline
    pConeMapper->SetInputConnection(pConeSource->GetOutputPort());
    pConeActor->SetMapper(pConeMapper);
    pRenderer->AddActor(pConeActor);
    // configure renderer
    pRenderer->SetBackground(0.0, 1.0, 0.0);
    pRenderer->GradientBackgroundOn();
    pRenderer->SetBackground(1, 1, 1);
    pRenderer->SetBackground2(0, 0, 1);
    pRenderer->GetActiveCamera()->Elevation(30.0);
    pRenderer->GetActiveCamera()->Azimuth(30.0);
    pRenderer->GetActiveCamera()->Zoom(1.0);
    pRenderer->GetActiveCamera()->SetClippingRange(1, 1000);

    if (m_pVTKWindow)
    {
      //pRenderWindow->SetWindowId(reinterpret_cast<void*>(m_pVTKWindow->GetHWND()));
      //pRenderWindow->SetParentId(reinterpret_cast<void*>(m_pVTKWindow->GetParent()->GetHWND()));
     // m_pVTKWindow->SetRenderWindow(pRenderWindow);
      //m_pVTKWindow->Initialize();
      //m_pVTKWindow->Render();
    }
}

MyFrame::~MyFrame() = default;

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("This is the about dialog of wx-vtk sample.\n"));

    wxMessageBox(msg, _T("About wx-vtk"), wxOK | wxICON_INFORMATION, this);
}
