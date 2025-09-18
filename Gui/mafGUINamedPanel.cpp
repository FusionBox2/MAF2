#include "mafGUINamedPanel.h"
#include "mafDecl.h"
#include "mafPics.h"
#include "mafGUIPicButton.h"

namespace gui::wx
{
#define NamedPanelStyle wxNO_BORDER | wxCLIP_CHILDREN

	NamedPanel::NamedPanel(wxWindow* parent, wxWindowID id, bool CloseButton, bool HideTitle)
		:mafGUIPanel(parent, id, wxDefaultPosition, wxDefaultSize, NamedPanelStyle)
	{
		m_Sizer = new wxBoxSizer(wxVERTICAL);

		//m_Color = wxColour(133,162,185);
		m_Color = wxColour(110, 150, 200);
		//m_Color = wxColour(190,190,190);
		//m_Color = wxColour(255,255,255);

		if (!HideTitle)
		{
			//m_Top = new wxStaticBox( this, -1,"",wxDefaultPosition,wxSize(-1,30));
			m_Top = new wxPanel(this, -1);

			m_TopSizer = new wxBoxSizer(wxHORIZONTAL);

			m_Label = new wxStaticText(m_Top, ID_LABEL_CLICK, " Panel Title:");

			wxFont font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#if WIN32
			font.SetPointSize(9);
#endif
			font.SetWeight(wxBOLD);
			m_Label->SetFont(font);
			m_TopSizer->Add(m_Label, 1, wxEXPAND);

			if (CloseButton)
			{
				mafGUIPicButton* b = new mafGUIPicButton(m_Top, _R("CLOSE_SASH"), ID_CLOSE_SASH);
				b->SetEventId(ID_CLOSE_SASH);  //SIL. 7-4-2005: 
				m_TopSizer->Add(b, 0, wxRIGHT, 2);
			}

			m_Top->SetAutoLayout(TRUE);
			m_Top->SetSizer(m_TopSizer);
			m_TopSizer->Fit(m_Top);
			m_TopSizer->SetSizeHints(m_Top);

			m_Sizer->Add(m_Top, 0, wxEXPAND | wxTOP, 2);
		}

		SetTitleColor(&m_Color);

		this->SetAutoLayout(TRUE);
		this->SetSizer(m_Sizer);
		m_Sizer->Fit(this);
		m_Sizer->SetSizeHints(this);
	}

	NamedPanel::~NamedPanel() = default;

	void mafGUINamedPanel::SetTitleColor(wxColour* color)
	{
		if (color) m_Color = *color;
		if (m_Top) m_Top->SetBackgroundColour(m_Color);
		if (m_Label) m_Label->SetBackgroundColour(m_Color);
	}
}