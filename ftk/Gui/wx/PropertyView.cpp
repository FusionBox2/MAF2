#include "PropertyView.h"

#include "ftk/Gui/wx/PropertyAdapter.h"

#include "ftk/Gui/IPropertyViewModel.h"

#include "wx/wx.h"

BEGIN_FTK_NAMESPACE

namespace gui::wx
{
	PropertyView::PropertyView(wxWindow* parent)
	{
		auto grid = new wxPropertyGrid(parent);
		m_panel = grid;
		m_adapter = std::make_unique<PropertyAdapter>(grid);
	}

	wxWindow* PropertyView::widget()
	{
		return m_panel;
	}

	std::shared_ptr<IViewModel> PropertyView::getModel() const
	{
		return m_viewModel;
	}

	void PropertyView::setModel(std::shared_ptr<IViewModel> viewModel)
	{
		m_adapter->setModel(nullptr);
		m_viewModel = std::static_pointer_cast<IPropertyViewModel>(viewModel);
		if (viewModel)
		{
			m_adapter->setModel(m_viewModel.get());
		}
	}

	void PropertyView::setActive(bool active)
	{
		// optional: highlight, focus, etc.
	}
}

END_FTK_NAMESPACE
