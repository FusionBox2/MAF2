#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/wx/Panel.h"

BEGIN_FTK_NAMESPACE

class mafView;

class mafGUIViewWin : public gui::wx::Panel
{
public:
	mafGUIViewWin(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);

	void OnSize(wxSizeEvent& event);

	mafView* m_Owner = nullptr;
};

END_FTK_NAMESPACE
