#include "MainWindow.h"

#include "ftk/Gui/wxw/MainFrame.h"

//#include <wx/mdi.h>
#include <wx/aui/aui.h>

BEGIN_FTK_NAMESPACE

namespace wxw
{
	wxFrame* CreateFrame(const wxString& title, const wxPoint& pos,
		const wxSize& size, long style, const wxString& name)
	{
		wxSize sz = (size == wxDefaultSize) ? wxWindow::FromDIP(wxSize(1280, 720), nullptr) : size;
		auto frame0 = new wxw::MainFrame<wxMDIParentFrame, wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL>(title, pos, sz, style, name);
		//auto frame1 = new wxw::MainFrame<wxAuiMDIParentFrame, wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL>(title, pos, sz, style, name);
		//auto frame2 = new wxw::MainFrame<wxFrame, wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL>(title, pos, sz, style, name);
		//auto frame =  new wxFrame(nullptr, wxID_ANY, "FTK Demo App", pos, sz, style, name);
		frame0->SetIcon(wxICON(sample));
		//frame1->SetIcon(wxICON(sample));
		//frame2->SetIcon(wxICON(sample));
		//frame->Busy();
		frame0->Show();
		//frame1->Show();
		//frame2->Show();

		return frame0;
	}
}

END_FTK_NAMESPACE
