#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/wx/IView.h"

#include <memory>

BEGIN_FTK_NAMESPACE

namespace gui
{
	class IPropertyViewModel;

	namespace wx
	{
		class PropertyAdapter;

		class PropertyView : public IView
		{
		public:
			PropertyView(wxWindow* parent);

			wxWindow* widget() override;

			std::shared_ptr<IViewModel> getModel() const override;

			void setModel(std::shared_ptr<IViewModel> viewModel) override;

			void setActive(bool active) override;

		protected:
			wxWindow* m_panel = nullptr;
			std::shared_ptr<IPropertyViewModel> m_viewModel;
			std::unique_ptr<PropertyAdapter> m_adapter;
		};
	}
}

END_FTK_NAMESPACE
