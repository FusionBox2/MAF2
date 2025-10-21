#include "mafGUIViewWin.h"
#include "mafView.h"

mafGUIViewWin::mafGUIViewWin(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
	:wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL)//SIL. 10-11-2003: added wxTAB_TRAVERSAL to intercept 'Enter' in the text widgets
{
	Bind(wxEVT_SIZE, &mafGUIViewWin::OnSize, this);
}

void mafGUIViewWin::OnSize(wxSizeEvent& event)
{
	if (m_Owner)
	{
		m_Owner->OnSize(event);
	}
}
