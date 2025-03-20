#include "ftk/wxVTK/wxGLVTKWindow.h"

#include "mafDeviceButtonsPad.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafEventInteraction.h"
#include "mafEvent.h"

#include "vtkObjectFactory.h"
#include <vtkRenderWindow.h>
#include "vtkRendererCollection.h"
#include "vtkCamera.h"

#include "vtkWindowToImageFilter.h"
#include "vtkImageExport.h"

#include <wx/filename.h>

#include <vector>
#include <memory>

class wxGLVTKRenderWindowInteractor : public vtkRenderWindowInteractor
{
public:
  static wxGLVTKRenderWindowInteractor* New();

  vtkTypeMacro(wxGLVTKRenderWindowInteractor, vtkRenderWindowInteractor);

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
  virtual void OnRButtonDoubleClick(wxMouseEvent& event);
  virtual void OnRButtonUp(wxMouseEvent& event);
  virtual void OnMButtonDown(wxMouseEvent& event);
  virtual void OnMButtonDoubleClick(wxMouseEvent& event);
  virtual void OnMButtonUp(wxMouseEvent& event);
  virtual void OnLButtonDown(wxMouseEvent& event);
  virtual void OnLButtonDoubleClick(wxMouseEvent& event);
  virtual void OnLButtonUp(wxMouseEvent& event);
  virtual void OnSize(wxSizeEvent& event);
  virtual void OnTimer(wxTimerEvent& event);
  virtual void OnKeyDown(wxKeyEvent& event);
  virtual void OnKeyUp(wxKeyEvent& event);
  virtual void OnChar(wxKeyEvent& event);
  virtual void OnMouseWheel(wxMouseEvent& event);
  virtual void OnSetFocus(wxFocusEvent& event);
  virtual void OnKillFocus(wxFocusEvent& event);

protected:
  wxGLVTKRenderWindowInteractor();
  ~wxGLVTKRenderWindowInteractor() override;

  wxWindow* Window;
  int MouseInWindow;
  std::map<int, wxTimer> timers;

  int InternalCreateTimer(int timerId, int timerType, unsigned long duration) override;
  int InternalDestroyTimer(int platformTimerId) override;

  void StartEventLoop() override;

private:
  wxGLVTKRenderWindowInteractor(const wxGLVTKRenderWindowInteractor&) = delete;
  wxGLVTKRenderWindowInteractor& operator=(const wxGLVTKRenderWindowInteractor&) = delete;
};

vtkStandardNewMacro(wxGLVTKRenderWindowInteractor);

wxGLVTKRenderWindowInteractor::wxGLVTKRenderWindowInteractor()
{
  this->Window = nullptr;
  this->MouseInWindow = 0;
}

wxGLVTKRenderWindowInteractor::~wxGLVTKRenderWindowInteractor()
{
  this->Enabled = 0;
}

void wxGLVTKRenderWindowInteractor::ProcessEvents()
{
}

void wxGLVTKRenderWindowInteractor::StartEventLoop()
{
}

void wxGLVTKRenderWindowInteractor::Initialize()
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

void wxGLVTKRenderWindowInteractor::SetWxWindow(wxWindow* wnd)
{
  this->Window = wnd;
}

void wxGLVTKRenderWindowInteractor::Enable()
{
  if (this->Enabled)
  {
    return;
  }

  this->Enabled = 1;
  this->Modified();
}

void wxGLVTKRenderWindowInteractor::Disable()
{
  if (!this->Enabled)
  {
    return;
  }

  this->Enabled = 0;
  this->Modified();
}

int wxGLVTKRenderWindowInteractor::InternalCreateTimer(
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

int wxGLVTKRenderWindowInteractor::InternalDestroyTimer(int platformTimerId)
{
  if (auto timerIt = timers.find(platformTimerId); timerIt != end(timers))
  {
    timerIt->second.Stop();
    timers.erase(timerIt);
    return 1;
  }
  return 0;
}

void wxGLVTKRenderWindowInteractor::OnEnterWindow(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  if (false)
	{
		this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  	this->SetAltKey(event.AltDown());
  	if (!this->MouseInWindow)
  	{
  		event.Skip(this->InvokeEvent(vtkCommand::EnterEvent, nullptr) == 0);
  		this->MouseInWindow = 1;
  	}
	}
}

void wxGLVTKRenderWindowInteractor::OnLeaveWindow(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  if (false)
  {
		this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  	this->SetAltKey(event.AltDown());
  	if (this->MouseInWindow)
  	{
  		event.Skip(this->InvokeEvent(vtkCommand::LeaveEvent, nullptr) == 0);
  		this->MouseInWindow = 0;
  	}
	}
}

void wxGLVTKRenderWindowInteractor::OnMouseMove(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  if (false)
  {
    this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
    this->SetAltKey(event.AltDown());
    event.Skip(this->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr) == 0);
  }
  else
  {
    mafEventInteraction e(this->Window, mafDeviceButtonsPadMouse::GetMouse2DMoveId());
    e.Set2DPosition(event.GetX(), this->Size[1] - event.GetY() - 1);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      static_cast<wxGLVTKWindow*>(this->Window)->InvokeEvent(&e);
  }
}

void wxGLVTKRenderWindowInteractor::OnMouseWheel(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  if (false)
  {
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
}

void wxGLVTKRenderWindowInteractor::OnLButtonDown(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  if (false)
  {
    this->Window->SetFocus();
    this->Window->CaptureMouse();
    this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
    this->SetAltKey(event.AltDown());
    event.Skip(this->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr) == 0);
  }
  else
  {
    mafEventInteraction e(this->Window, mafDeviceButtonsPadMouse::GetButtonDownId());
    e.Set2DPosition(event.GetX(), this->Size[1] - event.GetY() - 1);
    e.SetButton(MAF_LEFT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      static_cast<wxGLVTKWindow*>(this->Window)->InvokeEvent(&e);
  }

}

void wxGLVTKRenderWindowInteractor::OnLButtonDoubleClick(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  if (false)
  {
		this->Window->SetFocus();
  	this->Window->CaptureMouse();
  	this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  	this->SetAltKey(event.AltDown());
  	event.Skip(this->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr) == 0);
	}
  else
  {
    mafEventInteraction e(this->Window, mafDeviceButtonsPadMouse::GetMouseDClickId());
    e.Set2DPosition(event.GetX(), this->Size[1] - event.GetY() - 1);
    e.SetButton(MAF_LEFT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      static_cast<wxGLVTKWindow*>(this->Window)->InvokeEvent(&e);
  }
}

void wxGLVTKRenderWindowInteractor::OnLButtonUp(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  if (false)
  {
    this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
    this->SetAltKey(event.AltDown());
    event.Skip(this->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, nullptr) == 0);
    this->Window->ReleaseMouse();
    event.Skip(false);
  }
  else
  {
    mafEventInteraction e(this->Window, mafDeviceButtonsPadMouse::GetButtonUpId());
    e.Set2DPosition(event.GetX(), this->Size[1] - event.GetY() - 1);
    e.SetButton(MAF_LEFT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      static_cast<wxGLVTKWindow*>(this->Window)->InvokeEvent(&e);
  }
}

void wxGLVTKRenderWindowInteractor::OnMButtonDown(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  if (false)
  {
		this->Window->SetFocus();
  	this->Window->CaptureMouse();
  	this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  	this->SetAltKey(event.AltDown());
  	event.Skip(this->InvokeEvent(vtkCommand::MiddleButtonPressEvent, nullptr) == 0);
	}
  else
  {
    mafEventInteraction e(this->Window, mafDeviceButtonsPadMouse::GetButtonDownId());
    e.Set2DPosition(event.GetX(), this->Size[1] - event.GetY() - 1);
    e.SetButton(MAF_MIDDLE_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      static_cast<wxGLVTKWindow*>(this->Window)->InvokeEvent(&e);
  }
}

void wxGLVTKRenderWindowInteractor::OnMButtonDoubleClick(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  if (false)
  {
		this->Window->SetFocus();
  	this->Window->CaptureMouse();
  	this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  	this->SetAltKey(event.AltDown());
  	event.Skip(this->InvokeEvent(vtkCommand::MiddleButtonPressEvent, nullptr) == 0);
	}
  else
  {
    mafEventInteraction e(this->Window, mafDeviceButtonsPadMouse::GetMouseDClickId());
    e.Set2DPosition(event.GetX(), this->Size[1] - event.GetY() - 1);
    e.SetButton(MAF_MIDDLE_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      static_cast<wxGLVTKWindow*>(this->Window)->InvokeEvent(&e);
  }
}

void wxGLVTKRenderWindowInteractor::OnMButtonUp(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  if (false)
  {
		this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  	this->SetAltKey(event.AltDown());
  	event.Skip(this->InvokeEvent(vtkCommand::MiddleButtonReleaseEvent, nullptr) == 0);
  	this->Window->ReleaseMouse();
	}
  else
  {
    mafEventInteraction e(this->Window, mafDeviceButtonsPadMouse::GetButtonUpId());
    e.Set2DPosition(event.GetX(), this->Size[1] - event.GetY() - 1);
    e.SetButton(MAF_MIDDLE_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      static_cast<wxGLVTKWindow*>(this->Window)->InvokeEvent(&e);
  }
}

void wxGLVTKRenderWindowInteractor::OnRButtonDown(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  if (false)
  {
		this->Window->SetFocus();
  	this->Window->CaptureMouse();
  	this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  	this->SetAltKey(event.AltDown());
  	event.Skip(this->InvokeEvent(vtkCommand::RightButtonPressEvent, nullptr) == 0);
	}
  else
  {
    mafEventInteraction e(this->Window, mafDeviceButtonsPadMouse::GetButtonDownId());
    e.Set2DPosition(event.GetX(), this->Size[1] - event.GetY() - 1);
    e.SetButton(MAF_RIGHT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      static_cast<wxGLVTKWindow*>(this->Window)->InvokeEvent(&e);
  }
}

void wxGLVTKRenderWindowInteractor::OnRButtonDoubleClick(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  if (false)
  {
		this->Window->SetFocus();
  	this->Window->CaptureMouse();
  	this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  	this->SetAltKey(event.AltDown());
  	event.Skip(this->InvokeEvent(vtkCommand::RightButtonPressEvent, nullptr) == 0);
	}
  else
  {
    mafEventInteraction e(this->Window, mafDeviceButtonsPadMouse::GetMouseDClickId());
    e.Set2DPosition(event.GetX(), this->Size[1] - event.GetY() - 1);
    e.SetButton(MAF_RIGHT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      static_cast<wxGLVTKWindow*>(this->Window)->InvokeEvent(&e);
  }
}

void wxGLVTKRenderWindowInteractor::OnRButtonUp(wxMouseEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }

  if (false)
  {
		this->SetEventInformationFlipY(event.GetX(), event.GetY(), event.ControlDown(), event.ShiftDown(), 0, event.GetClickCount());
  	this->SetAltKey(event.AltDown());
  	event.Skip(this->InvokeEvent(vtkCommand::RightButtonReleaseEvent, nullptr) == 0);
  	this->Window->ReleaseMouse();
	}
  else
  {
    mafEventInteraction e(this->Window, mafDeviceButtonsPadMouse::GetButtonUpId());
    e.Set2DPosition(event.GetX(), this->Size[1] - event.GetY() - 1);
    e.SetButton(MAF_RIGHT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      static_cast<wxGLVTKWindow*>(this->Window)->InvokeEvent(&e);
  }
}

void wxGLVTKRenderWindowInteractor::OnSize(wxSizeEvent& event)
{
  this->UpdateSize(event.GetSize().GetWidth(), event.GetSize().GetHeight());
  if (this->Enabled)
  {
    event.Skip(this->InvokeEvent(vtkCommand::ConfigureEvent, nullptr) == 0);
  }
}

void wxGLVTKRenderWindowInteractor::OnTimer(wxTimerEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }
  int tid = static_cast<int>(event.GetTimer().GetId());
  event.Skip(this->InvokeEvent(vtkCommand::TimerEvent, (void*)&tid) == 0);
}

void wxGLVTKRenderWindowInteractor::OnKeyDown(wxKeyEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }
  if (false)
  {
		this->SetKeyEventInformation(event.ControlDown(), event.ShiftDown(), event.GetKeyCode(), 1, "none");
  	this->SetAltKey(event.AltDown());
  	event.Skip(this->InvokeEvent(vtkCommand::KeyPressEvent, nullptr) == 0);
	}
  event.Skip();
}

void wxGLVTKRenderWindowInteractor::OnKeyUp(wxKeyEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }
  if (false)
  {
		this->SetKeyEventInformation(event.ControlDown(), event.ShiftDown(), event.GetKeyCode(), 1, "none");
  	this->SetAltKey(event.AltDown());
  	event.Skip(this->InvokeEvent(vtkCommand::KeyReleaseEvent, nullptr) == 0);
	}
  event.Skip();
}

void wxGLVTKRenderWindowInteractor::OnChar(wxKeyEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }
  if (false)
	{
		this->SetKeyEventInformation(event.ControlDown(), event.ShiftDown(), event.GetKeyCode(), 1, "none");
  	this->SetAltKey(event.AltDown());
  	event.Skip(this->InvokeEvent(vtkCommand::CharEvent, nullptr) == 0);
	}
  else
  {
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
    {
      mafEvent e(this, mafDeviceButtonsPadMouse::GetMouseCharEventId()); e.SetArg(event.GetKeyCode());
      e.SetChannel(MCH_OUTPUT);
      m_Mouse->OnEvent(&e);
    }
  }

  event.Skip();
}

void wxGLVTKRenderWindowInteractor::OnSetFocus(wxFocusEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }
  event.Skip(true);
}

void wxGLVTKRenderWindowInteractor::OnKillFocus(wxFocusEvent& event)
{
  if (!this->Enabled)
  {
    return;
  }
  event.Skip(true);
}

void wxGLVTKRenderWindowInteractor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

IMPLEMENT_DYNAMIC_CLASS(wxGLVTKWindow, wxGLVTKWindowBase)

#if defined(USE_WXGLGLCANVAS)
static int wxvtk_attributes[] = {
  WX_GL_DOUBLEBUFFER,
  WX_GL_RGBA,
  WX_GL_DEPTH_SIZE,
  16,
  0
};
#endif

wxGLVTKWindow::wxGLVTKWindow() = default;

wxGLVTKWindow::wxGLVTKWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
  Create(parent, id, pos, size, style, name);
}

wxGLVTKWindow::~wxGLVTKWindow()
{
  this->SetRenderWindow(nullptr);
}

bool wxGLVTKWindow::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
  Bind(wxEVT_SIZE, &wxGLVTKWindow::OnSize, this);
  Bind(wxEVT_PAINT, &wxGLVTKWindow::OnPaint, this);
  Bind(wxEVT_ERASE_BACKGROUND, &wxGLVTKWindow::OnEraseBackground, this);
  Bind(wxEVT_DESTROY, &wxGLVTKWindow::OnDestroy, this);

  Bind(wxEVT_MOUSE_CAPTURE_LOST, [this](wxMouseCaptureLostEvent&) {});

  Bind(wxEVT_TIMER, [this](wxTimerEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnTimer(event); });

  Bind(wxEVT_LEFT_DCLICK, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnLButtonDoubleClick(event); });
  Bind(wxEVT_RIGHT_DCLICK, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnRButtonDoubleClick(event); });
  Bind(wxEVT_MIDDLE_DCLICK, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnMButtonDoubleClick(event); });

  Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& event) {SetFocus(); if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnLButtonDown(event); });
  Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnLButtonUp(event); });
  Bind(wxEVT_RIGHT_DOWN, [this](wxMouseEvent& event) {SetFocus(); if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnRButtonDown(event); });
  Bind(wxEVT_RIGHT_UP, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnRButtonUp(event); });
  Bind(wxEVT_MIDDLE_DOWN, [this](wxMouseEvent& event) {SetFocus(); if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnMButtonDown(event); });
  Bind(wxEVT_MIDDLE_UP, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnMButtonUp(event); });
  Bind(wxEVT_MOTION, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnMouseMove(event); });
  Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnKeyDown(event); });
  Bind(wxEVT_KEY_UP, [this](wxKeyEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnKeyUp(event); });
  Bind(wxEVT_CHAR, [this](wxKeyEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnChar(event); });
  Bind(wxEVT_MOUSEWHEEL, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnMouseWheel(event); });
  Bind(wxEVT_ENTER_WINDOW, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnEnterWindow(event); });
  Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnLeaveWindow(event); });
  Bind(wxEVT_SET_FOCUS, [this](wxFocusEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnSetFocus(event); });
  Bind(wxEVT_KILL_FOCUS, [this](wxFocusEvent& event) {if (auto iren = static_cast<wxGLVTKRenderWindowInteractor*>(GetInteractor())) iren->OnKillFocus(event); });

#ifdef USE_WXGLGLCANVAS
  if (!wxGLVTKWindowBase::Create(parent, id, pos, size, style, name, wxvtk_attributes))
    return false;
  m_glContext = std::make_unique<wxGLContext>(this);
#else
  if (!wxGLVTKWindowBase::Create(parent, id, pos, size, style, name))
    return false;
#if wxCHECK_VERSION(3,3,0)
#if __WXMSW__
  MSWDisableComposited();
#endif
#endif
#endif
	vtkNew<vtkRenderWindow> win;
  this->SetRenderWindow(win);
  return true;
}

void wxGLVTKWindow::OnDestroy(wxWindowDestroyEvent& event)
{
  this->SetRenderWindow(nullptr);
#ifdef USE_WXGLGLCANVAS
  m_glContext = nullptr;
#endif
}

void wxGLVTKWindow::SetRenderWindow(vtkRenderWindow* win)
{
  if (this->m_renderWindow)
  {
    if (this->m_renderWindow->GetMapped())
      this->m_renderWindow->Finalize();
    else
      this->m_renderWindow->ReleaseGraphicsResources(this->m_renderWindow);
    this->m_renderWindow->UnRegister(nullptr);
  }

  this->m_renderWindow = win;

  if (this->m_renderWindow)
  {
    this->m_renderWindow->Register(nullptr);

    // setup the parent window
#ifdef USE_WXGLGLCANVAS
    SetCurrent(*m_glContext);
    this->m_renderWindow->InitializeFromCurrentContext();
#endif
    this->m_renderWindow->SetWindowId(this->GetHandle());
    this->m_renderWindow->SetParentId(GetParent() ? GetParent()->GetHandle() : nullptr);

    // update size
    wxRect cRect(0, 0, 1, 1);
    if (this->GetParent())
      cRect = this->GetParent()->GetClientRect();

    vtkSmartPointer<wxGLVTKRenderWindowInteractor> iren;
    iren = vtkNew<wxGLVTKRenderWindowInteractor>();
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

vtkRenderWindow* wxGLVTKWindow::GetRenderWindow()
{
  return this->m_renderWindow;
}

vtkRenderWindowInteractor* wxGLVTKWindow::GetInteractor()
{
  if (!this->m_renderWindow)
  {
    return nullptr;
  }
  return this->m_renderWindow->GetInteractor();
}

void wxGLVTKWindow::OnPaint(wxPaintEvent& event)
{
  wxPaintDC pDC(this);
  if (auto iren = this->GetInteractor(); iren && iren->GetInitialized())
  {
    iren->Render();
  }
  else if (auto rw = this->GetRenderWindow(); rw && rw->GetInitialized())
  {
    rw->Render();
  }
}

void wxGLVTKWindow::OnEraseBackground(wxEraseEvent& event)
{
  event.Skip(false);
}

void wxGLVTKWindow::OnSize(wxSizeEvent& event)
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

vtkCamera* wxGLVTKWindow::GetCamera()
//---------------------------------------------------------------------------
{
  if (vtkRenderWindow* rw = this->GetRenderWindow())
  {
    if (vtkRendererCollection* rc = rw->GetRenderers())
    {
      rc->InitTraversal();
      vtkRenderer* ren = rc->GetNextItem();
      if (ren)
      {
        return ren->GetActiveCamera();
      }
    }
  }
  return nullptr;
}

void wxGLVTKWindow::GetImage(wxBitmap& bitmap, int magnification)
{
  int dim[3];
  GetRenderWindow()->OffScreenRenderingOn();
  vtkNew<vtkWindowToImageFilter> w2i;
  w2i->SetInput(GetRenderWindow());
#if VTK_MAJOR_VERSION > 7
  w2i->SetScale(magnification);
#else
  w2i->SetMagnification(magnification);
#endif
  w2i->Update();
  w2i->GetOutput()->GetDimensions(dim);
  GetRenderWindow()->OffScreenRenderingOff();

  assert(dim[0] > 0 && dim[1] > 0);
  std::vector<unsigned char> buffer(dim[0] * dim[1] * 3);

  //flip it - windows Bitmap are upside-down
  vtkNew<vtkImageExport> ie;
  ie->SetInputConnection(w2i->GetOutputPort());
  ie->ImageLowerLeftOff();
  ie->SetExportVoidPointer(buffer.data());
  ie->Export();

  //translate to a wxBitmap
  auto img = std::make_unique<wxImage>(dim[0], dim[1], buffer.data(), TRUE);
  bitmap = wxBitmap(*img, 24);
}

void wxGLVTKWindow::SaveImage(const mafString& filename, int magnification, int forceExtension)
{
}

void wxGLVTKWindow::SaveAllImages(const mafString& filename_, mafViewCompound* v, int forceExtension)
{

}

void wxGLVTKWindow::SetStereoMovieDirectory(const char* dir)
{
	
}

void wxGLVTKWindow::EnableStereoMovie(bool enable)
{
	
}

#if 0

#ifdef ORIG
void wxGLVTKWindow::DrawDC(CDC* pDC)
{
  // Obtain the size of the printer page in pixels.
  int cxPage = pDC->GetDeviceCaps(HORZRES);
  int cyPage = pDC->GetDeviceCaps(VERTRES);

  // Get the size of the window in pixels.
  const int* size = this->pvtkWin32OpenGLRW->GetSize();
  int cxWindow = size[0];
  int cyWindow = size[1];
  float fx = float(cxPage) / float(cxWindow);
  float fy = float(cyPage) / float(cyWindow);
  float scale = min(fx, fy);
  int x = int(scale * float(cxWindow));
  int y = int(scale * float(cyWindow));

  this->pvtkWin32OpenGLRW->SetUseOffScreenBuffers(true);
  this->pvtkWin32OpenGLRW->Render();

  unsigned char* pixels =
    this->pvtkWin32OpenGLRW->GetPixelData(0, 0, size[0] - 1, size[1] - 1, 0, 0);

  // now copy he result to the HDC
  int dataWidth = ((cxWindow * 3 + 3) / 4) * 4;

  BITMAPINFO MemoryDataHeader;
  MemoryDataHeader.bmiHeader.biSize = 40;
  MemoryDataHeader.bmiHeader.biWidth = cxWindow;
  MemoryDataHeader.bmiHeader.biHeight = cyWindow;
  MemoryDataHeader.bmiHeader.biPlanes = 1;
  MemoryDataHeader.bmiHeader.biBitCount = 24;
  MemoryDataHeader.bmiHeader.biCompression = BI_RGB;
  MemoryDataHeader.bmiHeader.biClrUsed = 0;
  MemoryDataHeader.bmiHeader.biClrImportant = 0;
  MemoryDataHeader.bmiHeader.biSizeImage = dataWidth * cyWindow;
  MemoryDataHeader.bmiHeader.biXPelsPerMeter = 10000;
  MemoryDataHeader.bmiHeader.biYPelsPerMeter = 10000;

  unsigned char* MemoryData; // the data in the DIBSection
  HDC MemoryHdc = (HDC)CreateCompatibleDC(pDC->GetSafeHdc());
  HBITMAP dib = CreateDIBSection(
    MemoryHdc, &MemoryDataHeader, DIB_RGB_COLORS, (void**)(&(MemoryData)), nullptr, 0);

  // copy the pixels over
  for (int i = 0; i < cyWindow; i++)
  {
    for (int j = 0; j < cxWindow; j++)
    {
      MemoryData[i * dataWidth + j * 3] = pixels[i * cxWindow * 3 + j * 3 + 2];
      MemoryData[i * dataWidth + j * 3 + 1] = pixels[i * cxWindow * 3 + j * 3 + 1];
      MemoryData[i * dataWidth + j * 3 + 2] = pixels[i * cxWindow * 3 + j * 3];
    }
  }

  // Put the bitmap into the device context
  SelectObject(MemoryHdc, dib);
  StretchBlt(pDC->GetSafeHdc(), 0, 0, x, y, MemoryHdc, 0, 0, cxWindow, cyWindow, SRCCOPY);

  this->pvtkWin32OpenGLRW->SetUseOffScreenBuffers(false);
  delete[] pixels;
}
#endif


#include "mafDecl.h"

#include <wx/event.h>
#include <wx/utils.h>
#include <wx/dcclient.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "wx/scrolwin.h"
#include "wx/gdicmn.h"
#include "wx/timer.h"

#ifdef __WXGTK__

//#include <glib/gmacros.h>
//#include <gdk/gdkprivate.h>
//#include "wx/gtk/win_gtk.h"


//#include "gdk/gdkprivate.h"
#include <wx/gtk/win_gtk.h> // ok c'e'

#endif

#include "mafGUIApplicationSettings.h"
#include "mafDevice.h"
#include "mafDeviceButtonsPad.h"
#include "mafDeviceButtonsPadMouse.h"
#include "mafEventInteraction.h"
#include "mafEvent.h"

#include "vtkSmartPointer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkCamera.h"
#include "vtkInteractorObserver.h"
#include "vtkInteractorStyle.h"
#include "vtkCommand.h"
#include "vtkMatrix4x4.h"
#include "vtkLight.h"
#include "vtkWindowToImageFilter.h"
#include "vtkBMPWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkTIFFWriter.h"
#include "vtkPostScriptWriter.h"
#include "vtkPNGWriter.h"
#include "vtkImageData.h"
#include "vtkImageExport.h"
#include "vtkImageAppend.h"
#include "vtkBMPReader.h"
#include "vtkMath.h"

#include <fstream>
//----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGLVTKWindow, wxWindow)
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(wxGLVTKWindow, wxWindow)
EVT_PAINT(wxGLVTKWindow::OnPaint)
EVT_ERASE_BACKGROUND(wxGLVTKWindow::OnEraseBackground)
EVT_LEFT_DCLICK(wxGLVTKWindow::OnLeftMouseDoubleClick)
EVT_LEFT_DOWN(wxGLVTKWindow::OnLeftMouseButtonDown)
EVT_LEFT_UP(wxGLVTKWindow::OnLeftMouseButtonUp)
EVT_MIDDLE_DOWN(wxGLVTKWindow::OnMiddleMouseButtonDown)
EVT_MIDDLE_UP(wxGLVTKWindow::OnMiddleMouseButtonUp)
EVT_RIGHT_DOWN(wxGLVTKWindow::OnRightMouseButtonDown)
EVT_RIGHT_UP(wxGLVTKWindow::OnRightMouseButtonUp)
EVT_MOTION(wxGLVTKWindow::OnMouseMotion)
EVT_TIMER(ID_wxGLVTKWindow_TIMER, wxGLVTKWindow::OnTimer)
EVT_KEY_DOWN(wxGLVTKWindow::OnKeyDown)
EVT_KEY_UP(wxGLVTKWindow::OnKeyUp)
EVT_CHAR(wxGLVTKWindow::OnChar)
EVT_SIZE(wxGLVTKWindow::OnSize)
EVT_IDLE(wxGLVTKWindow::OnIdle)
EVT_MOUSE_CAPTURE_LOST(wxGLVTKWindow::OnMouseCaptureLost)
END_EVENT_TABLE()
//----------------------------------------------------------------------------
wxGLVTKWindow::wxGLVTKWindow() : wxWindow(), vtkRenderWindowInteractor(), m_Timer(this, ID_wxGLVTKWindow_TIMER)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
wxGLVTKWindow::wxGLVTKWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos,
  const wxSize& size, long style, const wxString& name)
  : wxWindow(parent, id, pos, size, style, name), vtkRenderWindowInteractor(),
  m_Timer(this, ID_wxGLVTKWindow_TIMER)
  //----------------------------------------------------------------------------
{
#if wxCHECK_VERSION(3,3,0)
  MSWDisableComposited();
#endif

  m_Hidden = true;
  this->Show(false);
  //m_SaveDir = ::wxGetHomeDir().c_str(); 
  m_SaveDir = _R("");
  m_Width = m_Height = 10;

  m_Camera = NULL;

  m_StereoMovieDir = _R("");
  m_StereoMovieFrameCounter = 0;
  m_StereoMovieLeftEye = NULL;
  m_StereoMovieRightEye = NULL;
  m_StereoImage = NULL;
  m_StereoMoviewFrameWriter = NULL;
  m_StereoMovieEnable = false;
  m_StereoFrameGenerate = false;

  m_LastX = 0;
  m_LastY = 0;

  m_CustomInteractorStyle = false;
}
//----------------------------------------------------------------------------
wxGLVTKWindow::~wxGLVTKWindow()
//----------------------------------------------------------------------------
{
  vtkDEL(m_StereoMovieLeftEye);
  vtkDEL(m_StereoMovieRightEye);
  vtkDEL(m_StereoImage);
  vtkDEL(m_StereoMoviewFrameWriter);

  this->SetRenderWindow(NULL);
}
//----------------------------------------------------------------------------
wxGLVTKWindow* wxGLVTKWindow::New()
//----------------------------------------------------------------------------
{
  return new wxGLVTKWindow();
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::Initialize()
//----------------------------------------------------------------------------
{
  // if don't have render window then stuck
  if (!RenderWindow)
  {
    // mafLogMessage("wxGLVTKWindow::Initialize has no render window");
    return;
  }

#ifdef __WXMSW__
  if (RenderWindow->GetGenericWindowId() == 0)
    RenderWindow->SetWindowId((HWND)this->GetHWND());
#endif
#ifdef __WXGTK__
  // SIL ---- if (RenderWindow->GetGenericWindowId() == 0)
  // SIL ----   RenderWindow->SetParentId( (void *)(((GdkWindowPrivate *)GTK_PIZZA(m_wxwindow)->bin_window)->xwindow) );

  if (RenderWindow->GetGenericWindowId() == 0)
    RenderWindow->SetWindowId(
      (void*)GDK_WINDOW_XWINDOW(GTK_PIZZA(m_wxwindow)->bin_window)
    );
#endif

  //  #ifdef __WXMOTIF__
  //     if (RenderWindow->GetGenericWindowId() == 0)
  //     RenderWindow->SetWindowId( this->GetXWindow() );
  //  #endif

    // set minimum size of window
  int* size = RenderWindow->GetSize();
  size[0] = ((size[0] > 0) ? size[0] : 300);
  size[1] = ((size[1] > 0) ? size[1] : 300);

  // enable everything and start rendering
  Enable();
  RenderWindow->Start();

  // set the size in the render window interactor
  Size[0] = size[0];
  Size[1] = size[1];

  // this is initialized
  Initialized = 1;
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::Enable()
//----------------------------------------------------------------------------
{
  // if already enabled then done
  if (Enabled)
    return;

  // that's it
  Enabled = 1;
  Modified();
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::Disable()
//----------------------------------------------------------------------------
{
  // if already disabled then done
  if (!Enabled)
    return;

  // that's it
  Enabled = 0;
  Modified();
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::Start()
//----------------------------------------------------------------------------
{
  // the interactor cannot control the event loop
  mafLogMessage(_M("wxGLVTKWindow::Start() interactor cannot control event loop."));
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::UpdateSize(int x, int y)
//----------------------------------------------------------------------------
{
  // if the size changed tell render window
  if (((x != Size[0]) || (y != Size[1])) && (RenderWindow != 0))
  {
    Size[0] = x;
    Size[1] = y;
    RenderWindow->SetSize(x, y);
  }
}
//----------------------------------------------------------------------------
int wxGLVTKWindow::CreateTimer(int timertype)
//----------------------------------------------------------------------------
{
  // it's a one shot timer
  if (!m_Timer.Start(10, TRUE))
    assert(false);
  return 1;
}
//----------------------------------------------------------------------------
int wxGLVTKWindow::DestroyTimer()
//----------------------------------------------------------------------------
{
  // do nothing
  return 1;
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::TerminateApp()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnPaint(wxPaintEvent& event)
//----------------------------------------------------------------------------
{
  wxPaintDC pDC(this);

  //int w, h;
  //wxScrolledWindow::GetSize( &w,&h );
  //UpdateSize(w,h);
  //RenderWindow->SetSize(w, h);
  //vtkRenderWindowInteractor::SetSize(w, h);

  if (!RenderWindow)
    return; //rare - may happen during Debug

  if (!Initialized)
    Initialize();
  Render();
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnEraseBackground(wxEraseEvent& event)
//----------------------------------------------------------------------------
{
  event.Skip(false);
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnLeftMouseDoubleClick(wxMouseEvent& event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  CaptureMouse();
  NotifyClick(); // this will advise the mafGUIMDIChild of the click and it fires the VIEW_SELECT
  // event. If it is necessary will be defined a NotifyDoubleClick with a new ID VIEW_DOUBLE_CLICKED

  if (m_CustomInteractorStyle)
  {
    // VTK does not support double click event, so manage it through the mafDeviceButtonsPadMouse
  }
  else
  {
    mafEventInteraction e(this, mafDeviceButtonsPadMouse::GetMouseDClickId());
    e.Set2DPosition(event.GetX(), m_Height - event.GetY() - 1);
    e.SetButton(MAF_LEFT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      mafEventSender::InvokeEvent(&e);
  }
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnLeftMouseButtonDown(wxMouseEvent& event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  CaptureMouse();
  NotifyClick();

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(), m_Height - event.GetY() - 1, event.ControlDown(), event.ShiftDown());
    vtkRenderWindowInteractor::InvokeEvent(vtkCommand::LeftButtonPressEvent, NULL);
  }
  else
  {
    mafEventInteraction e(this, mafDeviceButtonsPad::GetButtonDownId());
    e.Set2DPosition(event.GetX(), m_Height - event.GetY() - 1);
    e.SetButton(MAF_LEFT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      mafEventSender::InvokeEvent(&e);
  }
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnMiddleMouseButtonDown(wxMouseEvent& event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  CaptureMouse();
  NotifyClick();

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(), m_Height - event.GetY() - 1, event.ControlDown(), event.ShiftDown());
    vtkRenderWindowInteractor::InvokeEvent(vtkCommand::MiddleButtonPressEvent, NULL);
  }
  else
  {
    mafEventInteraction e(this, mafDeviceButtonsPad::GetButtonDownId());
    e.Set2DPosition(event.GetX(), m_Height - event.GetY() - 1);
    e.SetButton(MAF_MIDDLE_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      mafEventSender::InvokeEvent(&e);
  }
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnRightMouseButtonDown(wxMouseEvent& event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  CaptureMouse();
  NotifyClick();

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(), m_Height - event.GetY() - 1, event.ControlDown(), event.ShiftDown());
    vtkRenderWindowInteractor::InvokeEvent(vtkCommand::RightButtonPressEvent, NULL);
  }
  else
  {
    mafEventInteraction e(this, mafDeviceButtonsPad::GetButtonDownId());
    e.Set2DPosition(event.GetX(), m_Height - event.GetY() - 1);
    e.SetButton(MAF_RIGHT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      mafEventSender::InvokeEvent(&e);
  }
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnLeftMouseButtonUp(wxMouseEvent& event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  m_StereoFrameGenerate = false;

  if (GetCapture() == this)
    ReleaseMouse();

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(), m_Height - event.GetY() - 1, event.ControlDown(), event.ShiftDown());
    vtkRenderWindowInteractor::InvokeEvent(vtkCommand::LeftButtonReleaseEvent, NULL);
  }
  else
  {
    mafEventInteraction e(this, mafDeviceButtonsPad::GetButtonUpId());
    e.Set2DPosition(event.GetX(), m_Height - event.GetY() - 1);
    e.SetButton(MAF_LEFT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      mafEventSender::InvokeEvent(&e);
  }
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnMiddleMouseButtonUp(wxMouseEvent& event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  m_StereoFrameGenerate = false;

  if (GetCapture() == this)
    ReleaseMouse();

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(), m_Height - event.GetY() - 1, event.ControlDown(), event.ShiftDown());
    vtkRenderWindowInteractor::InvokeEvent(vtkCommand::MiddleButtonReleaseEvent, NULL);
  }
  else
  {
    mafEventInteraction e(this, mafDeviceButtonsPad::GetButtonUpId());
    e.Set2DPosition(event.GetX(), m_Height - event.GetY() - 1);
    e.SetButton(MAF_MIDDLE_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      mafEventSender::InvokeEvent(&e);
  }
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnRightMouseButtonUp(wxMouseEvent& event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  m_StereoFrameGenerate = false;

  if (GetCapture() == this)
    ReleaseMouse();

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(), m_Height - event.GetY() - 1, event.ControlDown(), event.ShiftDown());
    vtkRenderWindowInteractor::InvokeEvent(vtkCommand::RightButtonReleaseEvent, NULL);
  }
  else
  {
    mafEventInteraction e(this, mafDeviceButtonsPad::GetButtonUpId());
    e.Set2DPosition(event.GetX(), m_Height - event.GetY() - 1);
    e.SetButton(MAF_RIGHT_BUTTON);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      mafEventSender::InvokeEvent(&e);
  }
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnMouseMotion(wxMouseEvent& event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_LastX == event.GetX() && m_LastY == event.GetY())
  {
    return;
  }
  m_LastX = event.GetX();
  m_LastY = event.GetY();

  if (m_StereoMovieEnable && m_StereoFrameGenerate)
  {
    GenerateStereoFrames();
  }

  if (m_CustomInteractorStyle)
  {
    SetEventInformation(event.GetX(), m_Height - event.GetY() - 1, event.ControlDown(), event.ShiftDown());
    vtkRenderWindowInteractor::InvokeEvent(vtkCommand::MouseMoveEvent, NULL);
  }
  else
  {
    mafEventInteraction e(this, mafDeviceButtonsPadMouse::GetMouse2DMoveId());
    e.Set2DPosition(event.GetX(), m_Height - event.GetY() - 1);
    e.SetModifier(MAF_SHIFT_KEY, event.ShiftDown());
    e.SetModifier(MAF_CTRL_KEY, event.ControlDown());
    e.SetModifier(MAF_ALT_KEY, event.AltDown());
    e.SetChannel(MCH_OUTPUT);
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
      m_Mouse->OnEvent(&e);
    else
      mafEventSender::InvokeEvent(&e);
  }
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnTimer(wxTimerEvent& event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_CustomInteractorStyle)
  {
    vtkRenderWindowInteractor::InvokeEvent(vtkCommand::TimerEvent, NULL);
  }
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnKeyDown(wxKeyEvent& event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_CustomInteractorStyle)
  {
    SetKeyEventInformation(event.ControlDown(), event.ShiftDown(), event.GetKeyCode(), 1, "none");
    vtkRenderWindowInteractor::InvokeEvent(vtkCommand::KeyPressEvent, NULL);
  }

  event.Skip();
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnKeyUp(wxKeyEvent& event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_CustomInteractorStyle)
  {
    SetKeyEventInformation(event.ControlDown(), event.ShiftDown(), event.GetKeyCode(), 1, "none");
    vtkRenderWindowInteractor::InvokeEvent(vtkCommand::KeyReleaseEvent, NULL);
  }

  event.Skip();
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnChar(wxKeyEvent& event)
//----------------------------------------------------------------------------
{
  if (!Enabled) return;

  if (m_CustomInteractorStyle)
  {
    SetKeyEventInformation(event.ControlDown(), event.ShiftDown(), event.GetKeyCode(), 1);
    vtkRenderWindowInteractor::InvokeEvent(vtkCommand::CharEvent, NULL);
  }
  else
  {
    if (mafDeviceButtonsPadMouse* m_Mouse = GetGlobalMouse())
    {
      mafEvent e(this, mafDeviceButtonsPadMouse::GetMouseCharEventId(), (intptr_t)event.GetKeyCode());
      e.SetChannel(MCH_OUTPUT);
      m_Mouse->OnEvent(&e);
    }
  }

  event.Skip();
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnIdle(wxIdleEvent& event)
//----------------------------------------------------------------------------
{
  //if(m_Hidden)
  //{
  //   m_Hidden = false;
  //   this->Show(true);
  //}
}

//----------------------------------------------------------------------------
void wxGLVTKWindow::OnMouseCaptureLost(wxMouseCaptureLostEvent&)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void wxGLVTKWindow::NotifyClick()
//----------------------------------------------------------------------------
{
  m_StereoFrameGenerate = m_StereoMovieEnable;

  wxCommandEvent e(wxEVT_COMMAND_BUTTON_CLICKED, VIEW_CLICKED);
  e.SetEventObject(this);
  ProcessEvent(e);
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{
  //   this->Show(false); 
     //m_Hidden = true;

  m_Width = event.GetSize().GetWidth();
  m_Height = event.GetSize().GetHeight();

#define DONT_FIX_CAMERA_RESET
#ifdef FIX_CAMERA_RESET

  //find the current camera and set UseHorizontalViewAngle
  //depending on the win aspect ratio

  vtkCamera* cam = GetCamera();
  if (cam)
  {
    if (m_Width < m_Height)
      cam->UseHorizontalViewAngleOn();
    else
      cam->UseHorizontalViewAngleOff();
  }
}
#endif

// should be so - otherwise disappear the windowing widget - TODO: understand better
// UpdateSize(event.GetSize().GetWidth(), event.GetSize().GetHeight());

if (m_CustomInteractorStyle)
{
  vtkRenderWindowInteractor::InvokeEvent(vtkCommand::ConfigureEvent, NULL); // mah! - should introduce noise     */
}
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::SaveImage(const mafString& filename_, int magnification, int forceExtension)
//---------------------------------------------------------------------------
{
#pragma message ("argument is modified below, so we need to copy it, refactor")
  mafString filename = filename_;//argument is modified below, so we need to copy it, refactor
  mafString path, name, ext;
  mafSplitPath(filename, &path, &name, &ext);
  if (filename.empty() || ext.empty())
  {
    mafString wildc = _R("Image (*.bmp)|*.bmp|Image (*.jpg)|*.jpg|Image (*.png)|*.png|Image (*.ps)|*.ps|Image (*.tiff)|*.tiff");
    //    wxString file = wxString::Format("%s\\%sSnapshot", m_SaveDir.GetCStr(),filename.GetCStr());
    mafString file;
    switch (forceExtension)
    {
    case mafGUIApplicationSettings::JPG:
      wildc = _R("Image (*.jpg)|*.jpg");
      break;
    case mafGUIApplicationSettings::BMP:
      wildc = _R("Image (*.bmp)|*.bmp");
      break;
    case mafGUIApplicationSettings::PNG:
      wildc = _R("Image (*.png)|*.png");
      break;
    }
    /*
        if(!mafDirExists(path))
        {
          file = m_SaveDir;
          file +=  _R("\\");
          filename = name;
        }
    */
    file.append(filename);
    file = mafGetSaveFile(file, wildc);
    if (file.empty())
      return;
    filename = file;
  }

  mafString temporary = mafWxToString(filename.toWx().AfterLast('\\').AfterFirst('.'));

  switch (forceExtension)
  {
  case mafGUIApplicationSettings::JPG:
    if (temporary != _L("jpg"))
      filename += _L(".jpg");
    break;
  case mafGUIApplicationSettings::BMP:
    if (temporary != _L("bmp"))
      filename += _L(".bmp");
    break;
  case mafGUIApplicationSettings::PNG:
    if (temporary != _L("png"))
      filename += _L(".png");
    break;
  }

  mafString basename = BaseName(filename);
  if (basename.empty())
  {
    filename = m_SaveDir + _R("\\") + filename;
  }

  ::wxBeginBusyCursor();

  long pixelXMeterX = 0;
  long pixelXMeterY = 0;
  vtkRenderWindow* rw = this->GetRenderWindow();
  if (rw)
  {
    vtkRendererCollection* rc = rw->GetRenderers();
    if (rc)
    {
      rc->InitTraversal();
      vtkRenderer* ren = rc->GetNextItem();
      if (ren)
      {
        //wxMessageBox(wxString::Format("%d", ren->GetActiveCamera()->GetParallelProjection()));
        double wp0x[4], wp1x[4];
        ren->SetDisplayPoint(0, 0, 0); //x
        ren->DisplayToWorld();
        ren->GetWorldPoint(wp0x);

        ren->SetDisplayPoint(10, 0, 0); //x
        ren->DisplayToWorld();
        ren->GetWorldPoint(wp1x);

        double pixelSpacingX = sqrt(vtkMath::Distance2BetweenPoints(wp1x, wp0x)) / 10;
        double meter = 1000; //millimeters
        pixelXMeterX = meter / pixelSpacingX;
        //wxMessageBox(wxString::Format("pixelXMeter = %f", meter/pixelSpacingX));

        double wp0y[4], wp1y[4];
        ren->SetDisplayPoint(0, 0, 0); //y
        ren->DisplayToWorld();
        ren->GetWorldPoint(wp0y);

        ren->SetDisplayPoint(0, 10, 0); //y
        ren->DisplayToWorld();
        ren->GetWorldPoint(wp1y);

        double pixelSpacingY = sqrt(vtkMath::Distance2BetweenPoints(wp1y, wp0y)) / 10;
        pixelXMeterY = meter / pixelSpacingY;
      }
    }

  }

  GetRenderWindow()->OffScreenRenderingOn();
  vtkNew<vtkWindowToImageFilter> w2i;
  w2i->SetInput(GetRenderWindow());
#if VTK_MAJOR_VERSION > 7
  w2i->SetScale(magnification);
#else
  w2i->SetMagnification(magnification);
#endif
  w2i->Update();
  GetRenderWindow()->OffScreenRenderingOff();

  mafSplitPath(filename, &path, &name, &ext);
  if (ext == _R("bmp"))
  {
    vtkNew<vtkBMPWriter> w;
    w->SetInputConnection(w2i->GetOutputPort());
    w->SetPixelPerMeterX(pixelXMeterX);
    w->SetPixelPerMeterY(pixelXMeterY);
    w->SetFileName(filename.GetCStr());
    w->Write();
  }
  else if (ext == _R("jpg"))
  {
    vtkNew<vtkJPEGWriter> w;
    w->SetInputConnection(w2i->GetOutputPort());
    w->SetFileName(filename.GetCStr());
    w->Write();
  }
  else if (ext == _R("tiff"))
  {
    vtkNew<vtkTIFFWriter> w;
    w->SetInputConnection(w2i->GetOutputPort());
    w->SetFileName(filename.GetCStr());
    w->Write();
  }
  else if (ext == _R("ps"))
  {
    vtkNew<vtkPostScriptWriter> w;
    w->SetInputConnection(w2i->GetOutputPort());
    w->SetFileName(filename.GetCStr());
    w->Write();
  }
  else if (ext == _R("png"))
  {
    vtkNew<vtkPNGWriter> w;
    w->SetInputConnection(w2i->GetOutputPort());
    w->SetPixelPerMeterX(pixelXMeterX);
    w->SetPixelPerMeterY(pixelXMeterY);
    w->SetFileName(filename.GetCStr());
    w->Write();
  }
  else
  {
    mafWarningMessage(_M(mafString(_L("Image can not be saved. Not valid file!"))));
  }
  ::wxEndBusyCursor();
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::SaveImageRecursive(const mafString& filename_, mafViewCompound* v, int magnification, int forceExtension)
//----------------------------------------------------------------------------
{
  if (v == NULL) return;
#pragma message ("argument is modified below, so we need to copy it, refactor")
  mafString filename = filename_;//argument is modified below, so we need to copy it, refactor

  mafString path, name, ext;
  mafSplitPath(filename, &path, &name, &ext);
  if (filename.empty() || ext.empty())
  {
    //wxString wildc = "Image (*.bmp)|*.bmp|Image (*.jpg)|*.jpg";
    mafString wildc = _R("Image (*.bmp)|*.bmp|Image (*.jpg)|*.jpg|Image (*.png)|*.png|Image (*.ps)|*.ps|Image (*.tiff)|*.tiff");
    mafString file = m_SaveDir + _R("\\") + filename + _R("Snapshot");
    switch (forceExtension)
    {
    case mafGUIApplicationSettings::JPG:
      wildc = _R("Image (*.jpg)|*.jpg");
      break;
    case mafGUIApplicationSettings::BMP:
      wildc = _R("Image (*.bmp)|*.bmp");
      break;
    case mafGUIApplicationSettings::PNG:
      wildc = _R("Image (*.png)|*.png");
      break;
    }
    //mafString file ;
    if (!mafDirExists(path))
    {
      file = m_SaveDir;
      file += _R("\\");
      filename = name;
    }

    file.append(filename);
    file = mafGetSaveFile(file, wildc);
    if (file.empty())
      return;
    filename = file;
  }

  mafString temporary = mafWxToString(filename.toWx().AfterLast('\\').AfterFirst('.'));

  switch (forceExtension)
  {
  case mafGUIApplicationSettings::JPG:
    if (temporary != _L("jpg"))
      filename += _L(".jpg");
    break;
  case mafGUIApplicationSettings::BMP:
    if (temporary != _L("bmp"))
      filename += _L(".bmp");
    break;
  case mafGUIApplicationSettings::PNG:
    if (temporary != _L("png"))
      filename += _L(".png");
    break;
  }

  mafString basename = BaseName(filename);
  if (basename.empty())
  {
    filename = m_SaveDir + _R("\\") + filename;
  }


  RecursiveSaving(filename, v, magnification);
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::RecursiveSaving(const mafString& filename, mafViewCompound* v, int magnification)
//----------------------------------------------------------------------------
{
  for (int i = 0; i < v->GetNumberOfSubView(); i++)
  {
    mafView* currentView;
    currentView = v->GetSubView(i);
    if (mafViewCompound::SafeDownCast(currentView) != NULL)
    {
      mafString subViewString;
      subViewString.append(filename);
      mafString pathName, fileName, extension;
      mafSplitPath(subViewString, &pathName, &fileName, &extension);

      subViewString.clear();
      subViewString.append(pathName);
      subViewString.append(_R("\\"));
      subViewString.append(fileName);
      subViewString.append(_R("_"));
      subViewString.append(currentView->GetLabel());
      subViewString.append(_R("."));
      subViewString.append(extension);

      RecursiveSaving(subViewString, mafViewCompound::SafeDownCast(currentView), magnification);
    }
    else
    {
      ///////////////////////////////////
      mafString temp, pathName, fileName, extension;
      temp.append(filename);
      mafSplitPath(temp, &pathName, &fileName, &extension);
      fileName.append(mafString::Format(_R("_%d"), i));
      temp.clear();
      temp.append(pathName);
      temp.append(_R("\\"));
      temp.append(fileName);
      temp.append(_R("."));
      temp.append(extension);

      ::wxBeginBusyCursor();

      long pixelXMeterX = 0;
      long pixelXMeterY = 0;
      vtkRenderWindow* rw = currentView->GetRWI()->GetRenderWindow();
      if (rw)
      {
        vtkRendererCollection* rc = rw->GetRenderers();
        if (rc)
        {
          rc->InitTraversal();
          vtkRenderer* ren = rc->GetNextItem();
          if (ren)
          {
            //wxMessageBox(wxString::Format("%d", ren->GetActiveCamera()->GetParallelProjection()));
            double wp0x[4], wp1x[4];
            ren->SetDisplayPoint(0, 0, 0); //x
            ren->DisplayToWorld();
            ren->GetWorldPoint(wp0x);

            ren->SetDisplayPoint(10, 0, 0); //x
            ren->DisplayToWorld();
            ren->GetWorldPoint(wp1x);

            double pixelSpacingX = sqrt(vtkMath::Distance2BetweenPoints(wp1x, wp0x)) / 10;
            double meter = 1000; //millimeters
            pixelXMeterX = meter / pixelSpacingX;
            //wxMessageBox(wxString::Format("pixelXMeter = %f", meter/pixelSpacingX));

            double wp0y[4], wp1y[4];
            ren->SetDisplayPoint(0, 0, 0); //y
            ren->DisplayToWorld();
            ren->GetWorldPoint(wp0y);

            ren->SetDisplayPoint(0, 10, 0); //y
            ren->DisplayToWorld();
            ren->GetWorldPoint(wp1y);

            double pixelSpacingY = sqrt(vtkMath::Distance2BetweenPoints(wp1y, wp0y)) / 10;
            pixelXMeterY = meter / pixelSpacingY;
          }
        }

      }
      currentView->GetRWI()->GetRenderWindow()->OffScreenRenderingOn();
      vtkNew<vtkWindowToImageFilter> w2i;
      w2i->SetInput(currentView->GetRWI()->GetRenderWindow());
#if VTK_MAJOR_VERSION > 7
      w2i->SetScale(magnification);
#else
      w2i->SetMagnification(magnification);
#endif
      w2i->Update();
      currentView->GetRWI()->GetRenderWindow()->OffScreenRenderingOff();

      if (extension == _R("bmp"))
      {
        vtkNew<vtkBMPWriter> w;
        w->SetInputConnection(w2i->GetOutputPort());
        w->SetPixelPerMeterX(pixelXMeterX);
        w->SetPixelPerMeterY(pixelXMeterY);
        w->SetFileName(temp.GetCStr());
        w->Write();
      }
      else if (extension == _R("jpg"))
      {
        vtkNew<vtkJPEGWriter> w;
        w->SetInputConnection(w2i->GetOutputPort());
        w->SetFileName(temp.GetCStr());
        w->Write();
      }
      else if (extension == _R("tiff"))
      {
        vtkNew<vtkTIFFWriter> w;
        w->SetInputConnection(w2i->GetOutputPort());
        w->SetFileName(temp.GetCStr());
        w->Write();
      }
      else if (extension == _R("ps"))
      {
        vtkNew<vtkPostScriptWriter> w;
        w->SetInputConnection(w2i->GetOutputPort());
        w->SetFileName(temp.GetCStr());
        w->Write();
      }
      else if (extension == _R("png"))
      {
        vtkNew<vtkPNGWriter> w;
        w->SetInputConnection(w2i->GetOutputPort());
        w->SetPixelPerMeterX(pixelXMeterX);
        w->SetPixelPerMeterY(pixelXMeterY);
        w->SetFileName(temp.GetCStr());
        w->Write();
      }
      else
      {
        mafWarningMessage(_M(mafString(_L("Image can not be saved. Not valid file!"))));
      }
      ::wxEndBusyCursor();
      ///////////////////////////////////
    }



  }
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::SaveAllImages(const mafString& filename_, mafViewCompound* v, int forceExtension)
//---------------------------------------------------------------------------
{
  if (v == NULL) return;
#pragma message ("argument is modified below, so we need to copy it, refactor")
  mafString filename = filename_;//argument is modified below, so we need to copy it, refactor
  mafString path, name, ext;
  mafSplitPath(filename, &path, &name, &ext);
  if (filename.empty() || ext.empty())
  {
    mafString wildc = _R("Image (*.jpg)|*.jpg|Image (*.bmp)|*.bmp|Image (*.png)|*.png");
    switch (forceExtension)
    {
    case mafGUIApplicationSettings::JPG:
      wildc = _R("Image (*.jpg)|*.jpg");
      break;
    case mafGUIApplicationSettings::BMP:
      wildc = _R("Image (*.bmp)|*.bmp");
      break;
    case mafGUIApplicationSettings::PNG:
      wildc = _R("Image (*.png)|*.png");
      break;
    }
    mafString file;
    if (!mafDirExists(path))
    {
      file = m_SaveDir;
      file += _R("\\");
      filename = name;
    }
    file.append(filename);
    file = mafGetSaveFile(file, wildc);
    if (file.empty())
      return;
    filename = file;
  }

  mafString temporary = mafWxToString(filename.toWx().AfterLast('\\').AfterFirst('.'));

  switch (forceExtension)
  {
  case mafGUIApplicationSettings::JPG:
    if (temporary != _L("jpg"))
      filename += _L(".jpg");
    break;
  case mafGUIApplicationSettings::BMP:
    if (temporary != _L("bmp"))
      filename += _L(".bmp");
    break;
  case mafGUIApplicationSettings::PNG:
    if (temporary != _L("png"))
      filename += _L(".png");
    break;
  }

  mafString basename = BaseName(filename);
  if (basename.empty())
  {
    filename = m_SaveDir + _R("\\") + filename;
  }

  ::wxBeginBusyCursor();

  wxBitmap imageBitmap;
  v->GetImage(imageBitmap);

  mafSplitPath(filename, &path, &name, &ext);
  if (ext == _R("bmp"))
  {
    imageBitmap.SaveFile(filename.toWx(), wxBITMAP_TYPE_BMP);
  }
  else if (ext == _R("jpg"))
  {
    wxJPEGHandler* jpegHandler = new wxJPEGHandler();
    jpegHandler->SetName(wxT("JPEGHANDLER"));
    wxImage::AddHandler(jpegHandler);
    wxImage image = imageBitmap.ConvertToImage();
    image.SetOption(_("quality"), 100);
    image.SaveFile(filename.toWx(), wxBITMAP_TYPE_JPEG);
    wxImage::RemoveHandler("JPEGHANDLER");
  }
  else if (ext == _R("png"))
  {
    /*wxPNGHandler *pngHandler = new wxPNGHandler();
    pngHandler->SetName("PNGHANDLER");
    wxImage::AddHandler(pngHandler);
    wxImage image = imageBitmap.ConvertToImage();
    image.SetOption(_("quality"), 100);
    image.SaveFile(filename.GetCStr(), wxBITMAP_TYPE_PNG);
    wxImage::RemoveHandler("PNGHANDLER");*/

    std::string fn = filename.toStd();
    fn = fn.substr(0, fn.size() - 3);
    fn.append("bmp");
    //imageBitmap.SetDepth(24);
    imageBitmap.SaveFile(fn.c_str(), wxBITMAP_TYPE_BMP);

    vtkBMPReader* r = vtkBMPReader::New();
    r->SetFileName(fn.c_str());
    r->Update();

    vtkPNGWriter* w = vtkPNGWriter::New();
    w->SetInputConnection(r->GetOutputPort());
    w->SetFileName(filename.GetCStr());
    w->Write();

    r->Delete();
    w->Delete();

    mafFileRemove(_R(fn.c_str()));
  }
  else
  {
    mafWarningMessage(_M("Must save with JPG, BMP or PNG extension"));
  }
  ::wxEndBusyCursor();
}
//----------------------------------------------------------------------------
vtkCamera* wxGLVTKWindow::GetCamera()
//---------------------------------------------------------------------------
{
  if (m_Camera == NULL)
  {
    vtkRenderWindow* rw = this->GetRenderWindow();
    if (rw)
    {
      vtkRendererCollection* rc = rw->GetRenderers();
      if (rc)
      {
        rc->InitTraversal();
        vtkRenderer* ren = rc->GetNextItem();
        if (ren)
        {
          m_Camera = ren->GetActiveCamera();
        }
      }
    }
  }
  return m_Camera;
}
//---------------------------------------------------------------------------
void wxGLVTKWindow::SetInteractorStyle(vtkInteractorObserver* o)
//---------------------------------------------------------------------------
{
  vtkRenderWindowInteractor::SetInteractorStyle(o);
  m_CustomInteractorStyle = o != NULL;
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::SetStereoMovieDirectory(const char* dir)
//----------------------------------------------------------------------------
{
  m_StereoMovieDir = _R(dir);
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::GenerateStereoFrames()
//----------------------------------------------------------------------------
{
  RenderWindow->SetStereoTypeToLeft();
  RenderWindow->Render();
  m_StereoMovieLeftEye->Modified();
  m_StereoMovieLeftEye->Update();
  RenderWindow->SetStereoTypeToRight();
  RenderWindow->Render();
  m_StereoMovieRightEye->Modified();
  m_StereoMovieRightEye->Update();
  m_StereoImage->Modified();
  m_StereoImage->Update();
  mafString filename;
  filename = m_StereoMovieDir;
  filename += _R("\\movie_");
  filename += mafString::Format(_R("%05d"), m_StereoMovieFrameCounter);
  filename += _R(".png");
  m_StereoMoviewFrameWriter->SetFileName(filename.GetCStr());
  m_StereoMoviewFrameWriter->Write();
  m_StereoMovieFrameCounter++;
}
//----------------------------------------------------------------------------
void wxGLVTKWindow::EnableStereoMovie(bool enable)
//----------------------------------------------------------------------------
{
  m_StereoMovieEnable = enable;

  if (m_StereoMovieEnable && m_StereoMovieLeftEye == NULL)
  {
    // Build Stereo Movie pipeline
    vtkNEW(m_StereoMovieLeftEye);
    m_StereoMovieLeftEye->SetInput(RenderWindow);

    vtkNEW(m_StereoMovieRightEye);
    m_StereoMovieRightEye->SetInput(RenderWindow);

    vtkNEW(m_StereoImage);
    m_StereoImage->AddInputConnection(m_StereoMovieLeftEye->GetOutputPort());
    m_StereoImage->AddInputConnection(m_StereoMovieRightEye->GetOutputPort());

    vtkNEW(m_StereoMoviewFrameWriter);
    m_StereoMoviewFrameWriter->SetInputConnection(m_StereoImage->GetOutputPort());
  }
}

IMPLEMENT_DYNAMIC_CLASS(wxGLVTKWindow, wxWindow)

wxGLVTKWindow::wxGLVTKWindow() = default;

wxGLVTKWindow::wxGLVTKWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
  Create(parent, id, pos, size, style, name);
}

wxGLVTKWindow::~wxGLVTKWindow() = default;

bool wxGLVTKWindow::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
  Bind(wxEVT_SIZE, &wxGLVTKWindow::OnSize, this);
  Bind(wxEVT_PAINT, &wxGLVTKWindow::OnPaint, this);
  Bind(wxEVT_ERASE_BACKGROUND, &wxGLVTKWindow::OnEraseBackground, this);
  Bind(wxEVT_DESTROY, &wxGLVTKWindow::OnDestroy, this);

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

void wxGLVTKWindow::OnDestroy(wxWindowDestroyEvent& event)
{
  this->SetRenderWindow(nullptr);
}

void wxGLVTKWindow::SetRenderWindow(vtkRenderWindow* win)
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

    vtkNew<wxGLVTKWindow> iren;
    //iren->SetInstallMessageProc(0);

    // setup the parent window
    this->m_renderWindow->SetWindowId(this->GetHandle());
    this->m_renderWindow->SetParentId(GetParent()->GetHandle());
    iren->SetRenderWindow(this->m_renderWindow);

    iren->Initialize();

    // update size
    wxRect cRect(0, 0, 1, 1);
    if (this->GetParent())
      cRect = this->GetParent()->GetClientRect();
    if (iren->GetInitialized())
      iren->UpdateSize(cRect.GetWidth(), cRect.GetHeight());
  }
}

vtkRenderWindow* wxGLVTKWindow::GetRenderWindow()
{
  return this->m_renderWindow;
}

vtkRenderWindowInteractor* wxGLVTKWindow::GetInteractor()
{
  if (!this->m_renderWindow)
  {
    return nullptr;
  }
  return this->m_renderWindow->GetInteractor();
}

void wxGLVTKWindow::OnPaint(wxPaintEvent& event)
{
  wxPaintDC pDC(this);
  if (this->GetInteractor() && this->GetInteractor()->GetInitialized())
  {
    this->GetInteractor()->Render();
  }
}

void wxGLVTKWindow::OnEraseBackground(wxEraseEvent& event)
{
  event.Skip(false);
}

#endif
