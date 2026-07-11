#include "OldGuiView.h"

#include "ftk/Gui/wx/OldGuiAdapter.h"

#include "ftk/Gui/IPropertyViewModel.h"

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	OldGuiView::OldGuiView(wxWindow* parent)
	{
		auto gui = new mafGUIHolder(parent, wxID_ANY, false, true);
		m_widget = gui;
		m_adapter = std::make_unique<OldGuiAdapter>(gui);
	}

	wxWindow* OldGuiView::widget()
	{
		return m_widget;
	}

	std::shared_ptr<IViewModel> OldGuiView::getModel() const
	{
		return m_viewModel;
	}

	void OldGuiView::setModel(std::shared_ptr<IViewModel> viewModel)
	{
		m_adapter->setModel(nullptr);
		m_viewModel = std::static_pointer_cast<IPropertyViewModel>(viewModel);
		if (viewModel)
		{
			m_adapter->setModel(m_viewModel.get());
		}
	}

	void OldGuiView::setActive(bool active)
	{
		// optional: highlight, focus, etc.
	}
}

END_FTK_NAMESPACE
