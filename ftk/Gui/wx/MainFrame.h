#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/wx/PaneFrame.h"

BEGIN_FTK_NAMESPACE

template<class BaseFrame>
class PaneFrame : public gui::wx::PaneFrame<BaseFrame>
{
public:

	PaneFrame();

	PaneFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

	~PaneFrame() override;

	bool Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

	void Busy();

	void Ready();

	void ProgressBarShow();

	void ProgressBarHide();

	void ProgressBarSetValue(int progress);

	int ProgressBarGetValue();

	void ProgressBarSetText(const wxString& msg);

protected:
	wxGauge* m_Gauge;
	size_t  m_ID_PBCall = 0;

private:
	void CreateControls();

	void CreateStatus();
};

template <class BaseFrame>
PaneFrame<BaseFrame>::PaneFrame() = default;

template <class BaseFrame>
PaneFrame<BaseFrame>::PaneFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	Create(parent, id, title, pos, size, style, name);
}

template <class BaseFrame>
PaneFrame<BaseFrame>::~PaneFrame() = default;

template <class BaseFrame>
bool PaneFrame<BaseFrame>::Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if (!gui::wx::PaneFrame<BaseFrame>::Create(parent, id, title, pos, size, style, name))
	{
		return false;
	}
	CreateControls();

	return true;
}

template <class BaseFrame>
void PaneFrame<BaseFrame>::CreateControls()
{
	CreateStatus();
}

template <class BaseFrame>
void PaneFrame<BaseFrame>::CreateStatus()
{
	if (auto statusBar = BaseFrame::CreateStatusBar())
	{
		int widths[] = { -1, 60, 60, 60, 210, 150 };
		std::for_each(std::begin(widths), std::end(widths), [this](int& w) {w = BaseFrame::FromDIP(w); });
		statusBar->SetFieldsCount(std::size(widths));
		statusBar->SetStatusWidths(std::size(widths), widths);
		statusBar->SetStatusText(_("welcome"), 0);
		statusBar->SetStatusText(" ", 1);
		statusBar->SetStatusText(" ", 2);
		statusBar->SetStatusText(" ", 3);

		wxRect pr;
		BaseFrame::GetStatusBar()->GetFieldRect(4, pr);
		m_Gauge = new wxGauge(BaseFrame::GetStatusBar(), -1, 100, pr.GetPosition(), pr.GetSize(), wxGA_SMOOTH);
		m_Gauge->SetForegroundColour(*wxRED);
		m_Gauge->Show(false);
		BaseFrame::GetStatusBar()->Bind(wxEVT_SIZE,
			[this](const wxSizeEvent& event)
			{
				wxRect r;
				BaseFrame::GetStatusBar()->GetFieldRect(4, r);
				m_Gauge->SetSize(r);
			}
		);
	}
}

template <class BaseFrame>
void PaneFrame<BaseFrame>::Busy()
{
	BaseFrame::SetStatusText("Busy", 2);
	BaseFrame::SetStatusText("", 3);
	m_Gauge->Show(true);
	m_Gauge->SetValue(0);
	BaseFrame::Refresh(false);
}

template <class BaseFrame>
void PaneFrame<BaseFrame>::Ready()
{
	BaseFrame::SetStatusText("", 2);
	BaseFrame::SetStatusText("", 3);
	m_Gauge->Show(false);
	BaseFrame::Refresh(false);
}

template <class BaseFrame>
void PaneFrame<BaseFrame>::ProgressBarShow()
{
	if (m_ID_PBCall == 0)
	{
		BaseFrame::SetStatusText("", 0);
		Busy();
	}
	++m_ID_PBCall;
}

template <class BaseFrame>
void PaneFrame<BaseFrame>::ProgressBarHide()
{
	--m_ID_PBCall;
	if (m_ID_PBCall != 0)
	{
		BaseFrame::SetStatusText("", 0);
		Ready();
	}
}

template <class BaseFrame>
void PaneFrame<BaseFrame>::ProgressBarSetValue(int progress)
{
	if (m_ID_PBCall != 0)
	{
		m_Gauge->SetValue(progress);
		BaseFrame::SetStatusText(wxString::Format(" %d%% ", progress), 3);
		wxYieldIfNeeded(); //fix on bug #2082
	}
}

template <class BaseFrame>
int PaneFrame<BaseFrame>::ProgressBarGetValue()
{
	return m_Gauge->GetValue();
}

template <class BaseFrame>
void PaneFrame<BaseFrame>::ProgressBarSetText(const wxString& msg)
{
	BaseFrame::SetStatusText(msg, 0);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#ifdef MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#include "vtkNew.h"
#include "vtkCommand.h"
#include "vtkAlgorithm.h"
#include "vtkViewport.h"

template<class Frame>
class mafGUIMDIFrameCallback : public vtkCommand
{
public:
	vtkTypeMacro(mafGUIMDIFrameCallback, vtkCommand);

	static mafGUIMDIFrameCallback* New() { return new mafGUIMDIFrameCallback; }
	mafGUIMDIFrameCallback() = default;
	void Execute(vtkObject* caller, unsigned long, void*) override;
	void SetMode(int mode) { m_mode = mode; }
	void SetFrame(Frame* frame) { m_Frame = frame; }

protected:
	int m_mode = 0;
	Frame* m_Frame = nullptr;
};

template <class BaseFrame>
class MainFrame : public PaneFrame<BaseFrame>
{
public:

	MainFrame();

	MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

	~MainFrame() override;

	bool Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

	void RenderStart();

	void RenderEnd();

	void BindToProgressBar(vtkObject* vtkobj);

	void BindToProgressBar(vtkAlgorithm* filter);

	void BindToProgressBar(vtkViewport* ren);

protected:
	vtkNew<mafGUIMDIFrameCallback<MainFrame<BaseFrame> > > m_StartCallback;
	vtkNew<mafGUIMDIFrameCallback<MainFrame<BaseFrame> > > m_EndCallback;
	vtkNew<mafGUIMDIFrameCallback<MainFrame<BaseFrame> > > m_ProgressCallback;
};

template<class Frame>
void mafGUIMDIFrameCallback<Frame>::Execute(vtkObject* caller, unsigned long, void*)
{
	assert(m_Frame);
	if (caller->IsA("vtkAlgorithm"))
	{
		auto po = static_cast<vtkAlgorithm*>(caller);

		if (m_mode == 0) // ProgressEvent-Callback
		{
			wxYieldIfNeeded(); //fix on bug #2082
			m_Frame->ProgressBarSetValue(po->GetProgress() * 100);
			//mafLogMessage("progress = %g", po->GetProgress()*100);
		}
		else if (m_mode == 1) // StartEvent-Callback
		{
			m_Frame->ProgressBarShow();
			m_Frame->ProgressBarSetValue(0);
			//m_Frame->ProgressBarSetText(&wxString(po->GetClassName()));
			{ wxString s = po->GetProgressText(); m_Frame->ProgressBarSetText(s); }
		}
		else if (m_mode == 2) // EndEvent-Callback
		{
			m_Frame->ProgressBarHide();
		}
	}
	else if (caller->IsA("vtkViewport"))
	{
		if (m_mode == 1) // StartRenderingEvent-Callback
		{
			m_Frame->RenderStart();
		}
		else if (m_mode == 2) // StartRenderingEvent-Callback
		{
			m_Frame->RenderEnd();
		}
	}
}

template <class BaseFrame>
void MainFrame<BaseFrame>::BindToProgressBar(vtkObject* vtkobj)
{
	if (auto viewport = vtkViewport::SafeDownCast(vtkobj))
	{
		BindToProgressBar(viewport);
	}
	else if (auto alg = vtkAlgorithm::SafeDownCast(vtkobj))
	{
		BindToProgressBar(alg);
	}
	else
	{
		//mafLogMessage(_M("wrong vtkObject passed to BindToProgressBar"));
	}
}

template <class BaseFrame>
void MainFrame<BaseFrame>::BindToProgressBar(vtkAlgorithm* filter)
{
	filter->AddObserver(vtkCommand::ProgressEvent, m_ProgressCallback);
	filter->AddObserver(vtkCommand::StartEvent, m_StartCallback);
	filter->AddObserver(vtkCommand::EndEvent, m_EndCallback);
}

template <class BaseFrame>
void MainFrame<BaseFrame>::BindToProgressBar(vtkViewport* ren)
{
	ren->AddObserver(vtkCommand::StartEvent, m_StartCallback);
	ren->AddObserver(vtkCommand::EndEvent, m_EndCallback);
}
using mafGUIMDIFrame = MainFrame<wxMDIParentFrame>;

template <class BaseFrame>
MainFrame<BaseFrame>::MainFrame() = default;

template <class BaseFrame>
MainFrame<BaseFrame>::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	Create(parent, id, title, pos, size, style, name);
}

template <class BaseFrame>
MainFrame<BaseFrame>::~MainFrame() = default;

template <class BaseFrame>
bool MainFrame<BaseFrame>::Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if (!PaneFrame<BaseFrame>::Create(parent, id, title, pos, size, style, name))
	{
		return false;
	}

	m_ProgressCallback->SetFrame(this);
	m_ProgressCallback->SetMode(0);
	m_StartCallback->SetFrame(this);
	m_StartCallback->SetMode(1);
	m_EndCallback->SetFrame(this);
	m_EndCallback->SetMode(2);
	return true;
}

template <class BaseFrame>
void MainFrame<BaseFrame>::RenderStart()
{
	BaseFrame::SetStatusText("Rendering", 1);
}

template <class BaseFrame>
void MainFrame<BaseFrame>::RenderEnd()
{
	BaseFrame::SetStatusText(" ", 1);
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#endif  //MAF_USE_VTK
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


END_FTK_NAMESPACE
