#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/wx/IView.h"

#include <memory>

BEGIN_FTK_NAMESPACE

namespace gui
{
	class ITreeViewModel;

	namespace wx
	{
		class TreeAdapter;

		class TreeView : public IView
		{
		public:
			TreeView(wxWindow* parent);

			~TreeView() override;

			wxWindow* widget() override;

			std::shared_ptr<IViewModel> getModel() const override;

			void setModel(std::shared_ptr<IViewModel> viewModel) override;

			void setActive(bool active) override;

			void setStateImages(const std::vector<wxBitmapBundle>& images);

		protected:
			wxWindow* m_widget = nullptr;
			std::shared_ptr<ITreeViewModel> m_viewModel;
			std::unique_ptr<TreeAdapter> m_adapter;
		};
	}
}

END_FTK_NAMESPACE
