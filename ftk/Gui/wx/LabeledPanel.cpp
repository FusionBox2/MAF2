#include "ftk/Gui/wx/LabeledPanel.h"

namespace gui::wx
{
#define NamedPanelStyle wxNO_BORDER | wxCLIP_CHILDREN | wxCAPTION

	LabeledPanel::LabeledPanel(wxWindow* parent, wxWindowID id, bool CloseButton, bool HideTitle)
		: wxPanel(parent, id, wxDefaultPosition, wxDefaultSize, NamedPanelStyle, "BLaaaaaaaaaaaaaaa")
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

			m_Label = new wxStaticText(m_Top, wxID_ANY, " Panel Title:");

			wxFont font = wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#if WIN32
			font.SetPointSize(9);
#endif
			font.SetWeight(wxBOLD);
			m_Label->SetFont(font);
			m_TopSizer->Add(m_Label, 1, wxEXPAND);

			if (CloseButton)
			{
				//mafGUIPicButton* b = new mafGUIPicButton(m_Top, _R("CLOSE_SASH"), ID_CLOSE_SASH);
				//b->SetEventId(ID_CLOSE_SASH);  //SIL. 7-4-2005: 
				//m_TopSizer->Add(b, 0, wxRIGHT, 2);
			}

			m_Top->SetAutoLayout(TRUE);
			m_Top->SetSizer(m_TopSizer);
			m_TopSizer->Fit(m_Top);
			m_TopSizer->SetSizeHints(m_Top);

			m_Sizer->Add(m_Top, 0, wxEXPAND | wxTOP, 2);
		}

		SetTitleColor(m_Color);

		this->SetAutoLayout(TRUE);
		this->SetSizer(m_Sizer);
		m_Sizer->Fit(this);
		m_Sizer->SetSizeHints(this);
	}

	LabeledPanel::~LabeledPanel() = default;

	void LabeledPanel::Add(wxWindow* window, int proportion, int flag, int border)
	{
		window->Reparent(this);
		m_Sizer->Add(window, proportion, flag, border);
	}

	void LabeledPanel::Add(wxSizer* sizer, int proportion, int flag, int border)
	{
		m_Sizer->Add(sizer, proportion, flag, border);
	}

	bool LabeledPanel::Remove(wxWindow* window)
	{
		return m_Sizer->Detach(window);
	}

	bool LabeledPanel::Remove(wxSizer* sizer)
	{
		return m_Sizer->Detach(sizer);
	}

	void LabeledPanel::SetTitle(const wxString& label)
	{
		if (m_Label)
		{
			m_Label->SetLabel(label);
		}
	}

	void LabeledPanel::SetTitleColor(const wxColour& color)
	{
		m_Color = color;
		if (m_Top)
		{
			m_Top->SetBackgroundColour(m_Color);
		}
		if (m_Label)
		{
			m_Label->SetBackgroundColour(m_Color);
		}
	}
}