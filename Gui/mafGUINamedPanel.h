#pragma once

#include "ftkConfigure.h"

#include "mafDefines.h"
#include "ftk/Base/mfString.h"
#include "ftk/Gui/wx/Panel.h"

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	/**
	 Class Name: mafGUINamedPanel.
	- mafGUINamedPanel is a wxPanel with a title bar.
	- the title bar has a title and an optional close button.
	- mafGUINamedPanel provide a default BoxSizer making easy to place widgets on it,
	  just call Add(widget) or Remove(widget).

	\todo
	decide the ID to associate with the close button
	*/
	class FTK_GUI_EXPORT NamedPanel : public Panel
	{
	public:

		NamedPanel(wxWindow* parent, wxWindowID id = wxID_ANY, bool CloseButton = false, bool HideTitle = false);

		~NamedPanel() override;

		void Add(wxWindow* window, int proportion = 0, int flag = wxEXPAND, int border = 0) { window->Reparent(this); m_Sizer->Add(window, proportion, flag, border); }

		void Add(wxSizer* sizer, int proportion = 0, int flag = wxEXPAND, int border = 0) { m_Sizer->Add(sizer, proportion, flag, border); }

		bool Remove(wxWindow* window) { return m_Sizer->Detach(window); }

		bool Remove(wxSizer* sizer) { return m_Sizer->Detach(sizer); }

		void SetTitle(const mafString& label) { if (m_Label) m_Label->SetLabel(mafStringToWx(label)); }

		void SetTitleColor(wxColour* color = nullptr);

	protected:
		wxPanel* m_Top = nullptr;
		wxStaticText* m_Label = nullptr;
		wxBoxSizer* m_Sizer = nullptr;
		wxBoxSizer* m_TopSizer = nullptr;
		wxColour m_Color;

	};
}

using mafGUINamedPanel = gui::wx::NamedPanel;

END_FTK_NAMESPACE
