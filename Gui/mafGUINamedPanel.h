#pragma once

#include "ftkConfigure.h"

#include "mafDefines.h"
#include "ftk/Base/String.h"
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
		/** constructor.  set CloseButton to show the close button */
		NamedPanel(wxWindow* parent, wxWindowID id = -1, bool CloseButton = false, bool HideTitle = false);
		/** destructor. */
		~NamedPanel() override;

		/** Add a widget to the panel. */
		void Add(wxWindow* window, int option = 0, int flag = wxEXPAND, int border = 0) { window->Reparent(this); m_Sizer->Add(window, option, flag, border); }

		/** Add a sizer to the panel. */
		void Add(wxSizer* sizer, int option = 0, int flag = wxEXPAND, int border = 0) { m_Sizer->Add(sizer, option, flag, border); }

		/** Remove a widget from the panel. */
		bool Remove(wxWindow* window) { return m_Sizer->Detach(window); }

		/** Remove a sizer from the panel. */
		bool Remove(wxSizer* sizer) { return m_Sizer->Detach(sizer); }

		/** Set the text shown on the title-bar. */
		void SetTitle(const mafString& label) { if (m_Label) m_Label->SetLabel(mafStringToWx(label)); }

		/** Change the background color of the title. */
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
