#include "TreeView.h"

#include "ftk/Gui/wx/TreeAdapter.h"

#include "ftk/Gui/ITreeViewModel.h"

#include "ftk/Gui/wx/LabeledPanel.h"

#include "wx/wx.h"

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	TreeView::TreeView(wxWindow* parent)
	{
		//auto panel = new gui::wx::LabeledPanel(parent, wxID_ANY, false, false);
		//panel->Bind(wxEVT_DESTROY, [this](wxWindowDestroyEvent&) {if (m_destroyed) return; m_destroyed = true; delete this; });
		auto tree = new wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, wxWindow::FromDIP(wxSize(100, 100), parent), wxNO_BORDER | wxTR_HAS_BUTTONS | wxTR_EDIT_LABELS);
		//panel->Add(m_tree, 1, wxEXPAND);
		//panel->SetSize(-1, wxWindow::FromDIP(300, parent));
		//panel->SetTitle("vme hierarchy:");
		m_widget = tree;
		m_adapter = std::make_unique<TreeAdapter>(tree);
	}

	TreeView::~TreeView() = default;

	wxWindow* TreeView::widget()
	{
		return m_widget;
	}

	std::shared_ptr<IViewModel> TreeView::getModel() const
	{
		return m_viewModel;
	}

	void TreeView::setModel(std::shared_ptr<IViewModel> viewModel)
	{
		m_adapter->setModel(nullptr);
		m_viewModel = std::static_pointer_cast<ITreeViewModel>(viewModel);
		m_adapter->setModel(m_viewModel.get());
	}

	void TreeView::setActive(bool active)
	{
		// optional: highlight, focus, etc.
	}

	void TreeView::setStateImages(const std::vector<wxBitmapBundle>& images)
	{
		static_cast<wxTreeCtrl*>(m_widget)->SetImages(images);
	}
}

END_FTK_NAMESPACE
