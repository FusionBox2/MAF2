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
		class OldGuiAdapter;

		class OldGuiView : public IView
		{
		public:
			OldGuiView(wxWindow* parent);

			wxWindow* widget() override;

			std::shared_ptr<IViewModel> getModel() const override;

			void setModel(std::shared_ptr<IViewModel> viewModel) override;

			void setActive(bool active) override;

		protected:
			wxWindow* m_widget = nullptr;
			std::shared_ptr<IPropertyViewModel> m_viewModel;
			std::unique_ptr<OldGuiAdapter> m_adapter;
		};
	}
}

END_FTK_NAMESPACE
