#pragma once

#include "ftkConfigure.h"

#include <wx/mdi.h>

#include "mafPics.h"
#include "mafEvent.h"
#include "mafEventSender.h"

class mafView;

BEGIN_FTK_NAMESPACE

template<class BaseFrame, class ParentFrame, long DefaultStyle>
class ViewFrame : public BaseFrame, public mafEventSender
{
public:
	const int PH = 13; //panel height

	ViewFrame(mafView* view, ParentFrame* parent, const wxString& title = "child", const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = DefaultStyle, const wxString& name = wxASCII_STR(wxFrameNameStr));

	~ViewFrame() override;

protected:

	void OnActivate(wxActivateEvent& event);

	void OnCloseWindow(wxCloseEvent& event);

	void OnSize(wxSizeEvent& event);

	void OnMaximize(wxMaximizeEvent& event);

	wxWindow* m_Win;
	mafView* m_View;
};

template<class BaseFrame, class ParentFrame, long DefaultStyle>
ViewFrame<BaseFrame, ParentFrame, DefaultStyle>::ViewFrame(mafView* view, ParentFrame* parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	: BaseFrame(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, style, name)
{
	BaseFrame::Bind(wxEVT_ACTIVATE, &ViewFrame::OnActivate, this);
	BaseFrame::Bind(wxEVT_CLOSE_WINDOW, &ViewFrame::OnCloseWindow, this);
	BaseFrame::Bind(wxEVT_SIZE, &ViewFrame::OnSize, this);
	BaseFrame::Bind(wxEVT_MAXIMIZE, &ViewFrame::OnMaximize, this);

	assert(view);
	m_View = view;

	this->Show(false);
	m_Win = m_View->GetWindow();
	m_Win->Reparent(this);
	m_Win->Show(true);

	BaseFrame::SetIcon(mafPictureFactory::GetPictureFactory()->GetIcon(_R("MDICHILD_ICON")));
	BaseFrame::SetTitle(wxStripMenuCodes(mafStringToWx(m_View->GetLabel())));
}

template<class BaseFrame, class ParentFrame, long DefaultStyle>
ViewFrame<BaseFrame, ParentFrame, DefaultStyle>::~ViewFrame()
{

}

template<class BaseFrame, class ParentFrame, long DefaultStyle>
void ViewFrame<BaseFrame, ParentFrame, DefaultStyle>::OnActivate(wxActivateEvent& event)
{
	if (event.GetActive() && m_View)
	{
		{ mafEvent evUnq(this, VIEW_SELECT); evUnq.SetView(m_View); InvokeEvent(evUnq); }
		BaseFrame::Layout();
	}
}

template<class BaseFrame, class ParentFrame, long DefaultStyle>
void ViewFrame<BaseFrame, ParentFrame, DefaultStyle>::OnCloseWindow(wxCloseEvent& event)
{
	// VIEW_DELETE must be sent from here and not from the destructor
	// otherwise VIEW_DELETE is sent also on the closing of the application
	// when the listener (the ViewManager) has been already destroyed
	if (m_View && !m_View->Close(false) && event.CanVeto())
	{
		event.Veto();
		return;
	}
	{ mafEvent evUnq(this, VIEW_DELETE); evUnq.SetView(m_View); InvokeEvent(evUnq); }
	BaseFrame::Destroy();
	m_View = nullptr;
}

template<class BaseFrame, class ParentFrame, long DefaultStyle>
void ViewFrame<BaseFrame, ParentFrame, DefaultStyle>::OnSize(wxSizeEvent& event)
{
	wxSize sz = BaseFrame::GetClientSize();
	wxSize minSz = BaseFrame::FromDIP(wxSize(PH, PH));
	if (sz.GetWidth() < minSz.GetWidth() || sz.GetHeight() < minSz.GetHeight())
	{
		return;
	}

	m_Win->Move(0, 0);
	m_Win->SetSize(sz);
	m_Win->Layout();
#ifndef WIN32
	if (m_View)
	{
		m_View->SetWindowSize(sz.GetWidth(), sz.GetHeight());
	}
#endif
	m_View->CameraUpdate();
}

template<class BaseFrame, class ParentFrame, long DefaultStyle>
void ViewFrame<BaseFrame, ParentFrame, DefaultStyle>::OnMaximize(wxMaximizeEvent& event)
{
	if (m_View)
	{
		mafString msg = _R("MaximizeSelectedView");
		{ mafEvent evUnq(this, VIEW_MAXIMIZE_); evUnq.SetString(&msg); InvokeEvent(evUnq); }
	}
}

using mafGUIMDIChild = ViewFrame<wxMDIChildFrame, wxMDIParentFrame, wxDEFAULT_FRAME_STYLE>;
using mafGUIViewFrame = ViewFrame<wxFrame, wxFrame, wxDEFAULT_FRAME_STYLE>;

END_FTK_NAMESPACE
