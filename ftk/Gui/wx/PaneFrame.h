#pragma once

#include "ftkConfigure.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/aui/aui.h>

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	template<class BaseFrame>
	class PaneFrame : public BaseFrame
	{
	public:

		PaneFrame();

		PaneFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

		~PaneFrame() override;

		bool Create(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxASCII_STR(wxFrameNameStr));

		wxAuiManager& GetDockManager() { return m_auiManager; }

		bool AddPane(wxWindow* window, wxAuiPaneInfo& pi);

		wxWindow* RemovePane(const wxString& pane_name);

		wxWindow* RemovePane(wxWindow* window);

		wxWindow* RemovePane(wxWindowID id);

		wxWindow* RemovePane(wxAuiPaneInfo& pi);

		void ShowPane(const wxString& name, bool show = true);

		void ShowPane(wxWindow* window, bool show = true);

		void ShowPane(wxWindowID id, bool show = true);

		void ShowPane(wxAuiPaneInfo& pi, bool show = true);

		bool IsPaneShown(const wxString& name);

		bool IsPaneShown(wxWindow* window);

		bool IsPaneShown(wxWindowID id);

		bool IsPaneShown(const wxAuiPaneInfo& pi);

		wxAuiPaneInfo& GetPane(const wxString& name);

		wxAuiPaneInfo& GetPane(wxWindow* window);

		wxAuiPaneInfo& GetPane(wxWindowID id);

		wxAuiPaneInfoArray& GetPanes();

		void ShowStatus(bool show = true);

		bool IsStatusShown();

	private:
		wxAuiManager m_auiManager;
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
		if (!BaseFrame::Create(parent, id, title, pos, size, style, name))
		{
			return false;
		}

		m_auiManager.SetManagedWindow(this);
		m_auiManager.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
		m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR));
		m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR));
		m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR));
		m_auiManager.Update();

		return true;
	}

	template <class BaseFrame>
	bool PaneFrame<BaseFrame>::AddPane(wxWindow* window, wxAuiPaneInfo& pi)
	{
		if (m_auiManager.AddPane(window, pi))
		{
			m_auiManager.Update();
			return true;
		}
		return false;
	}

	template <class BaseFrame>
	wxWindow* PaneFrame<BaseFrame>::RemovePane(const wxString& name)
	{
		return RemovePane(GetPane(name));
	}

	template <class BaseFrame>
	wxWindow* PaneFrame<BaseFrame>::RemovePane(wxWindow* window)
	{
		return RemovePane(GetPane(window));
	}

	template <class BaseFrame>
	wxWindow* PaneFrame<BaseFrame>::RemovePane(wxWindowID id)
	{
		return RemovePane(GetPane(id));
	}

	template <class BaseFrame>
	wxWindow* PaneFrame<BaseFrame>::RemovePane(wxAuiPaneInfo& pi)
	{
		if (pi.IsOk())
		{
			wxWindow* w = pi.window;
			if (m_auiManager.DetachPane(pi.window))
			{
				m_auiManager.Update();
				return w;
			}
		}
		return nullptr;
	}

	template <class BaseFrame>
	void PaneFrame<BaseFrame>::ShowPane(const wxString& name, bool show)
	{
		ShowPane(GetPane(name), show);
	}

	template <class BaseFrame>
	void PaneFrame<BaseFrame>::ShowPane(wxWindow* window, bool show)
	{
		ShowPane(GetPane(window), show);
	}

	template <class BaseFrame>
	void PaneFrame<BaseFrame>::ShowPane(wxWindowID id, bool show)
	{
		ShowPane(m_auiManager.GetPane(id), show);
	}

	template <class BaseFrame>
	void PaneFrame<BaseFrame>::ShowPane(wxAuiPaneInfo& pi, bool show)
	{
		if (pi.IsOk())
		{
			pi.Show(show);
			m_auiManager.Update();
		}
	}

	template <class BaseFrame>
	bool PaneFrame<BaseFrame>::IsPaneShown(const wxString& name)
	{
		return IsPaneShown(GetPane(name));
	}

	template <class BaseFrame>
	bool PaneFrame<BaseFrame>::IsPaneShown(wxWindow* window)
	{
		return IsPaneShown(GetPane(window));
	}

	template <class BaseFrame>
	bool PaneFrame<BaseFrame>::IsPaneShown(wxWindowID id)
	{
		return IsPaneShown(GetPane(id));
	}

	template <class BaseFrame>
	bool PaneFrame<BaseFrame>::IsPaneShown(const wxAuiPaneInfo& pi)
	{
		return pi.IsOk() && pi.IsShown();
	}

	template <class BaseFrame>
	wxAuiPaneInfo& PaneFrame<BaseFrame>::GetPane(const wxString& name)
	{
		return m_auiManager.GetPane(name);
	}

	template <class BaseFrame>
	wxAuiPaneInfo& PaneFrame<BaseFrame>::GetPane(wxWindow* window)
	{
		return m_auiManager.GetPane(window);
	}

	template <class BaseFrame>
	wxAuiPaneInfo& PaneFrame<BaseFrame>::GetPane(wxWindowID id)
	{
		for (auto& pane : m_auiManager.GetAllPanes())
		{
			if (id == pane.window->GetId())
			{
				return pane;
			}
		}
		return wxAuiNullPaneInfo;
	}

	template <class BaseFrame>
	wxAuiPaneInfoArray& PaneFrame<BaseFrame>::GetPanes()
	{
		return m_auiManager.GetAllPanes();
	}

	template <class BaseFrame>
	void PaneFrame<BaseFrame>::ShowStatus(bool show)
	{
		if (auto statusbar = BaseFrame::GetStatusBar())
		{
			statusbar->Show(show);
			m_auiManager.Update();
		}
	}

	template <class BaseFrame>
	bool PaneFrame<BaseFrame>::IsStatusShown()
	{
		if (auto statusbar = BaseFrame::GetStatusBar())
		{
			return statusbar->IsShown();
		}
		return false;
	}
}

END_FTK_NAMESPACE
