#pragma once

#include "ftkConfigure.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
//#include <wx/mdi.h>
//#include <wx/aui/aui.h>

BEGIN_FTK_NAMESPACE

namespace wxw
{
	class MainFrameAnyBase
	{
	public:
		MainFrameAnyBase(wxWindow *frame) : m_frame(frame)
		{
		}

	protected:
		bool TryProcessEvent(wxEvent& event)
		{
			return false;
		}

		wxWindow* const m_frame = nullptr;
	};

	template <class BaseFrame, long DefaultStyle = wxDEFAULT_FRAME_STYLE>
	class MainFrame : public BaseFrame, public MainFrameAnyBase
	{
	public:

		MainFrame();

		MainFrame(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = DefaultStyle, const wxString& name = wxASCII_STR(wxFrameNameStr));

		~MainFrame() override;

		bool Create(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = DefaultStyle, const wxString& name = wxASCII_STR(wxFrameNameStr));

	protected:
		bool TryBefore(wxEvent& event) override
		{
			// It is important to send the event to the base class first as
			// wxMDIParentFrame overrides its TryBefore() to send the menu events
			// to the currently active child frame and the child must get them
			// before our own TryProcessEvent() is executed, not afterwards.
			return BaseFrame::TryBefore(event) || TryProcessEvent(event);
		}

	private:
		void OnExit(wxCommandEvent& WXUNUSED(event))
		{
			this->Close();
		}

		void OnCloseWindow(wxCloseEvent& event)
		{
			/*if (m_docManager && !m_docManager->Clear(!event.CanVeto()))
			{
				// The user decided not to close finally, abort.
				event.Veto();
			}
			else*/
			{
				// Just skip the event, base class handler will destroy the window.
				event.Skip();
			}
		}
	};

	template <class BaseFrame, long DefaultStyle>
	MainFrame<BaseFrame, DefaultStyle>::MainFrame() : MainFrameAnyBase(this) {}

	template <class BaseFrame, long DefaultStyle>
	MainFrame<BaseFrame, DefaultStyle>::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
		: MainFrame()
	{
		Create(title, pos, size, style, name);
	}

	template <class BaseFrame, long DefaultStyle>
	MainFrame<BaseFrame, DefaultStyle>::~MainFrame()
	{
	}

	template <class BaseFrame, long DefaultStyle>
	bool MainFrame<BaseFrame, DefaultStyle>::Create(const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	{
		if (!BaseFrame::Create(nullptr, wxID_ANY, title, pos, size, style, name))
		{
			return false;
		}

		//mafSetFrame(this);
		//CreateControls();

		//m_auiManager.SetManagedWindow(this);
		//m_auiManager.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
		//m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR));
		//m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR));
		//m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR));
		//m_auiManager.Update();

		BaseFrame::Centre();

#ifdef __WIN32__
		BaseFrame::DragAcceptFiles(true);
#endif

		/*wxIconBundle ib;
		ib.AddIcon(mafPictureFactory::GetPictureFactory()->GetIcon(_R("FRAME_ICON16x16")));
		ib.AddIcon(mafPictureFactory::GetPictureFactory()->GetIcon(_R("FRAME_ICON32x32")));
		BaseFrame::SetIcons(ib);*/
		this->Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
		this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnCloseWindow, this);
		return true;
	}

}
#ifdef IUOIUOIUOU
namespace wxw_
{
	template <class BaseFrame, long DefaultStyle>
	class MainFrame : public BaseFrame
	{
	public:

		MainFrame();

		MainFrame(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = DefaultStyle, const wxString& name = wxASCII_STR(wxFrameNameStr));

		~MainFrame() override;

		bool Create(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = DefaultStyle, const wxString& name = wxASCII_STR(wxFrameNameStr));

		wxAuiManager& GetDockManager() { return m_auiManager; }

		void OnSwitchPane(wxCommandEvent&);

		void OnUpdatePaneUI(wxUpdateUIEvent& event);

		bool AddPane(wxWindow* window, wxAuiPaneInfo& pane_info);

		wxWindow* RemovePane(const wxString& pane_name);

		wxWindow* RemovePane(wxWindow* window);

		wxWindow* RemovePane(wxAuiPaneInfo& pane_info);

		void ShowPane(const wxString& pane_name, bool show = true);

		void ShowPane(wxWindow* window, bool show = true);

		void ShowPane(wxAuiPaneInfo& pane_info, bool show = true);

		bool IsPaneShown(const wxString& pane_name);

		bool IsPaneShown(wxWindow* window);

		bool IsPaneShown(const wxAuiPaneInfo& pane_info);

		void Busy();

		void Ready();

	protected:
		wxGauge* m_Gauge;

		wxAuiManager m_auiManager;
	private:
		void CreateControls();
		void CreateStatus();
	};

	template <class BaseFrame, long DefaultStyle>
	MainFrame<BaseFrame, DefaultStyle>::MainFrame() = default;

	template <class BaseFrame, long DefaultStyle>
	MainFrame<BaseFrame, DefaultStyle>::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
		: MainFrame()
	{
		Create(title, pos, size, style, name);
	}

	template <class BaseFrame, long DefaultStyle>
	MainFrame<BaseFrame, DefaultStyle>::~MainFrame()
	{
	}

	template <class BaseFrame, long DefaultStyle>
	bool MainFrame<BaseFrame, DefaultStyle>::Create(const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
	{
		if (!BaseFrame::Create(nullptr, wxID_ANY, title, pos, size, style, name))
		{
			return false;
		}

		//mafSetFrame(this);
		CreateControls();

		m_auiManager.SetManagedWindow(this);
		m_auiManager.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
		m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR));
		m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR));
		m_auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, m_auiManager.GetArtProvider()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR));
		m_auiManager.Update();

		BaseFrame::Centre();

#ifdef __WIN32__
		BaseFrame::DragAcceptFiles(true);
#endif

		/*wxIconBundle ib;
		ib.AddIcon(mafPictureFactory::GetPictureFactory()->GetIcon(_R("FRAME_ICON16x16")));
		ib.AddIcon(mafPictureFactory::GetPictureFactory()->GetIcon(_R("FRAME_ICON32x32")));
		BaseFrame::SetIcons(ib);*/
		return true;
	}

	template <class BaseFrame, long DefaultStyle>
	void MainFrame<BaseFrame, DefaultStyle>::CreateControls()
	{
		CreateStatus();
	}

	template <class BaseFrame, long DefaultStyle>
	void MainFrame<BaseFrame, DefaultStyle>::CreateStatus()
	{
		if (auto statusBar = BaseFrame::CreateStatusBar())
		{
			int widths[] = { -1,60,60,60,210,150 }; // enlarged the last tab for the "free mem watch" information
			std::transform(std::begin(widths), std::end(widths), std::begin(widths), [this](int w) {return BaseFrame::FromDIP(w); });
			statusBar->SetFieldsCount(6);
			statusBar->SetStatusWidths(6, widths);
			statusBar->SetStatusText(_("welcome"), 0);
			statusBar->SetStatusText(" ", 1);
			statusBar->SetStatusText(" ", 2);
			statusBar->SetStatusText(" ", 3);

			wxRect pr;
			BaseFrame::GetStatusBar()->GetFieldRect(4, pr);
			m_Gauge = new wxGauge(BaseFrame::GetStatusBar(), -1, 100, pr.GetPosition(), pr.GetSize(), wxGA_SMOOTH);
			m_Gauge->SetForegroundColour(*wxRED);
			m_Gauge->Show(false);
			BaseFrame::GetStatusBar()->Bind(wxEVT_SIZE,
				[this](const wxSizeEvent& event)
				{
					wxRect r;
					BaseFrame::GetStatusBar()->GetFieldRect(4, r);
					m_Gauge->SetPosition(r.GetPosition());
					m_Gauge->SetSize(r.GetSize());
				}
			);
		}
	}

	template <class BaseFrame, long DefaultStyle>
	bool MainFrame<BaseFrame, DefaultStyle>::AddPane(wxWindow* window, wxAuiPaneInfo& pane_info)
	{
		if (!m_auiManager.AddPane(window, pane_info))
		{
			return false;
		}

		if (pane_info.caption.IsEmpty())
		{
			return true;
		}

		wxMenuBar* menubar = BaseFrame::GetMenuBar();
		if (!menubar)
			return true;
		int idx = menubar->FindMenu("View");
		if (idx == wxNOT_FOUND)
			return true;
		wxMenu* viewMenu = menubar->GetMenu(idx);
		if (pane_info.IsToolbar())
		{
			wxMenu* toolbarsMenu = nullptr;
			int ids = viewMenu->FindItem("Toolbars");
			if (ids == wxNOT_FOUND)
			{
				toolbarsMenu = new wxMenu;
				wxMenuItem* toolbarsItem = viewMenu->Insert(0, wxID_ANY, "Toolbars", toolbarsMenu);
			}
			else
			{
				wxMenuItem* sm = viewMenu->FindItem(ids);
				toolbarsMenu = sm->GetSubMenu();
			}
			if (toolbarsMenu)
			{
				toolbarsMenu->Append(window->GetId(), pane_info.caption, "", wxITEM_CHECK);
			}
			BaseFrame::Bind(wxEVT_MENU, &MainFrame::OnSwitchPane, this, window->GetId(), window->GetId());
			BaseFrame::Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdatePaneUI, this, window->GetId(), window->GetId());
		}
		else
		{
			int ids = viewMenu->FindItem("Layout");
			if (ids != wxNOT_FOUND)
			{
				size_t pos = 0;
				wxMenuItem* sm = viewMenu->FindChildItem(ids, &pos);
				viewMenu->Insert(pos, window->GetId(), pane_info.caption, "", wxITEM_CHECK);
			}
			else
			{
				viewMenu->Append(window->GetId(), pane_info.caption, "", wxITEM_CHECK);
			}
			BaseFrame::Bind(wxEVT_MENU, &MainFrame::OnSwitchPane, this, window->GetId(), window->GetId());
			BaseFrame::Bind(wxEVT_UPDATE_UI, &MainFrame::OnUpdatePaneUI, this, window->GetId(), window->GetId());
		}
		return true;
	}

	template <class BaseFrame, long DefaultStyle>
	void MainFrame<BaseFrame, DefaultStyle>::OnSwitchPane(wxCommandEvent& event)
	{
		wxAuiPaneInfoArray& all_panes = m_auiManager.GetAllPanes();
		size_t count = all_panes.GetCount();
		for (size_t i = 0; i < count; i++)
		{
			if (event.GetId() == all_panes[i].window->GetId())
			{
				all_panes[i].Show(!all_panes[i].IsShown());
				m_auiManager.Update();
				return;
			}
		}
	}

	template <class BaseFrame, long DefaultStyle>
	void MainFrame<BaseFrame, DefaultStyle>::OnUpdatePaneUI(wxUpdateUIEvent& event)
	{
		wxAuiPaneInfoArray& all_panes = m_auiManager.GetAllPanes();
		size_t count = all_panes.GetCount();
		for (size_t i = 0; i < count; i++)
		{
			if (event.GetId() == all_panes[i].window->GetId())
			{
				event.Check(all_panes[i].IsShown());
			}
		}
	}

	template <class BaseFrame, long DefaultStyle>
	wxWindow* MainFrame<BaseFrame, DefaultStyle>::RemovePane(const wxString& pane_name)
	{
		wxAuiPaneInfo& pi = m_auiManager.GetPane(pane_name);
		if (pi.IsOk())
		{
			return RemovePane(pi);
		}
		return nullptr;
	}

	template <class BaseFrame, long DefaultStyle>
	wxWindow* MainFrame<BaseFrame, DefaultStyle>::RemovePane(wxWindow* window)
	{
		wxAuiPaneInfo& pi = m_auiManager.GetPane(window);
		if (pi.IsOk())
		{
			return RemovePane(pi);
		}
		return nullptr;
	}

	template <class BaseFrame, long DefaultStyle>
	wxWindow* MainFrame<BaseFrame, DefaultStyle>::RemovePane(wxAuiPaneInfo& pane_info)
	{
		wxMenuBar* menubar = BaseFrame::GetMenuBar();
		int idx = menubar->FindMenu("View");
		if (idx != wxNOT_FOUND)
		{
			wxMenu* viewMenu = menubar->GetMenu(idx);
			if (pane_info.IsToolbar())
			{
				int ids = viewMenu->FindItem("Toolbars");
				wxMenuItem* sm = viewMenu->FindItem(ids);
				wxMenu* toolbarsMenu = sm->GetSubMenu();
				BaseFrame::Unbind(wxEVT_MENU, &MainFrame::OnSwitchPane, this, pane_info.window->GetId(), pane_info.window->GetId());
				BaseFrame::Unbind(wxEVT_UPDATE_UI, &MainFrame::OnUpdatePaneUI, this, pane_info.window->GetId(), pane_info.window->GetId());
				delete toolbarsMenu->Remove(pane_info.window->GetId());
				if (toolbarsMenu->GetMenuItemCount() == 0)
				{
					delete viewMenu->Remove(sm);
				}
			}
			else
			{
				BaseFrame::Unbind(wxEVT_MENU, &MainFrame::OnSwitchPane, this, pane_info.window->GetId(), pane_info.window->GetId());
				BaseFrame::Unbind(wxEVT_UPDATE_UI, &MainFrame::OnUpdatePaneUI, this, pane_info.window->GetId(), pane_info.window->GetId());
				delete viewMenu->Remove(pane_info.window->GetId());
			}
		}
		wxWindow* w = pane_info.window;
		if (!m_auiManager.DetachPane(pane_info.window))
			return nullptr;
		return w;
	}

	template <class BaseFrame, long DefaultStyle>
	void MainFrame<BaseFrame, DefaultStyle>::ShowPane(const wxString& pane_name, bool show)
	{
		wxAuiPaneInfo& pi = m_auiManager.GetPane(pane_name);
		if (pi.IsOk())
		{
			pi.Show(show);
			m_auiManager.Update();
		}
	}

	template <class BaseFrame, long DefaultStyle>
	void MainFrame<BaseFrame, DefaultStyle>::ShowPane(wxWindow* window, bool show)
	{
		ShowPane(m_auiManager.GetPane(window), show);
	}

	template <class BaseFrame, long DefaultStyle>
	void MainFrame<BaseFrame, DefaultStyle>::ShowPane(wxAuiPaneInfo& pane_info, bool show)
	{
		if (pane_info.IsOk())
		{
			pane_info.Show(show);
			m_auiManager.Update(); // sync the MenuItems check 
		}
	}

	template <class BaseFrame, long DefaultStyle>
	bool MainFrame<BaseFrame, DefaultStyle>::IsPaneShown(const wxString& pane_name)
	{
		return IsPaneShown(m_auiManager.GetPane(pane_name));
	}

	template <class BaseFrame, long DefaultStyle>
	bool MainFrame<BaseFrame, DefaultStyle>::IsPaneShown(wxWindow* window)
	{
		return IsPaneShown(m_auiManager.GetPane(window));
	}

	template <class BaseFrame, long DefaultStyle>
	bool MainFrame<BaseFrame, DefaultStyle>::IsPaneShown(const wxAuiPaneInfo& pane_info)
	{
		return pane_info.IsOk() && pane_info.IsShown();

	}

	template <class BaseFrame, long DefaultStyle>
	void MainFrame<BaseFrame, DefaultStyle>::Busy()
	{
		BaseFrame::SetStatusText("Busy", 2);
		BaseFrame::SetStatusText("", 3);
		m_Gauge->Show(true);
		m_Gauge->SetValue(0);
		BaseFrame::Refresh(false);
	}

	template <class BaseFrame, long DefaultStyle>
	void MainFrame<BaseFrame, DefaultStyle>::Ready()
	{
		BaseFrame::SetStatusText("", 2);
		BaseFrame::SetStatusText("", 3);
		m_Gauge->Show(false);
		BaseFrame::Refresh(false);
	}

	using mafGUIMDIFrame = MainFrame<wxMDIParentFrame, wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL>;
}
#endif
END_FTK_NAMESPACE
