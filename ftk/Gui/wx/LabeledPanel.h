#pragma once

#include "ftkConfigure.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	class LabeledPanel : public wxPanel
	{
	public:

		LabeledPanel(wxWindow* parent, wxWindowID id = wxID_ANY, bool CloseButton = false, bool HideTitle = false);

		~LabeledPanel() override;

		void Add(wxWindow* window, int proportion = 0, int flag = wxEXPAND, int border = 0);

		void Add(wxSizer* sizer, int proportion = 0, int flag = wxEXPAND, int border = 0);

		bool Remove(wxWindow* window);

		bool Remove(wxSizer* sizer);

		void SetTitle(const wxString& label);

		void SetTitleColor(const wxColour& color);

	protected:
		wxPanel* m_Top = nullptr;
		wxStaticText* m_Label = nullptr;
		wxBoxSizer* m_Sizer = nullptr;
		wxBoxSizer* m_TopSizer = nullptr;
		wxColour m_Color;

	};
}

END_FTK_NAMESPACE
